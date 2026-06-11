//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBSprocessLogs.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-20, 3:50p.m.
//  ---------------------------------------------------------------------------
#include <SDBU/SDBUutils.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLarray.h>
#include <MATCH/MATCHhighlight.h>
#include <MATCH/MATCHsearch.h>
#include <SDB/SDBcapi.h>
#include <SDB/SDBcontentDelete.h>
#include <SDB/SDBserviceLog.h>
#include <SDBS/SDBSprocessLogs.h>
COL_LOG_MODULE;

static const COLuint64 SDBS_24H_MS = 86400000ULL; // 24h in milliseconds

static void SDBSaddTagIfMissing(const COLuint64 Type, COLstring& TagToAdd, COLarray<COLvar>& Tags) {
   COL_FUNCTION(SDBSaddTagIfMissing);
   if(Type != SDB_ERROR && Type != SDB_MESSAGE) { return; }
   TagToAdd = Type == SDB_ERROR ? "#error" : "#message";
   bool TagExists = false;
   for(const auto& it : Tags) {
      if(it.asString() == TagToAdd) {
         TagExists = true;
         break;
      }
   }
   if(!TagExists) {
      COL_TRC("Added missing tag: " << TagToAdd);
      Tags.push_back(TagToAdd);
   }
}

static COLstring SDBSmergeTagType(const COLuint64 Type, COLvar& Result) {
   COL_FUNCTION(SDBSmergeTagType);
   COL_VAR(Type);
   auto&     Tags = Result["tags"].array();
   COLstring TagToAdd;
   SDBSaddTagIfMissing(Type, TagToAdd, Tags);
   return TagToAdd;
}

static void SDBSreorderTagMatches(const COLstring& TagSearch, const bool ExactMatch, COLvar& Result) {
   COL_FUNCTION(SDBSreorderTagMatches);
   auto&                              Tags = Result["tags"].array();
   COLarray<COLstring>               Matched;
   COLarray<COLstring>               NonMatched;
   Matched.setCapacity(Tags.size());
   NonMatched.setCapacity(Tags.size());
   for(auto& tag : Tags) {
      MATCHsearch(TagSearch, tag, ExactMatch) ? Matched.push_back(tag) : NonMatched.push_back(tag);
   }
   // Overwrite the tags array in one go
   Tags.clear();
   for(auto& t : Matched)    { Tags.push_back(t); }
   for(auto& t : NonMatched) { Tags.push_back(t); }
}

static bool SDBSprocessMessage(const SDBmessage* pMessage, const COLstring& MessageSearch, const COLstring& TagSearch,
                               const bool ExactMatch,const bool IncludeNonMatches,
                               const bool Formatted, const bool Minify, COLarray<COLvar>& Results) {
   COL_FUNCTION(SDBSprocessMessage);
   COLvar      Item;
   SDBUcontentToVar(*pMessage, &Item);
   const COLstring                    TagAdded  = SDBSmergeTagType(pMessage->Type, Item);
   const COLstring                    TagString = pMessage->Tags.is_null() ? TagAdded : pMessage->Tags + " " + TagAdded;
   MATCHpositions MessagePositions, TagPositions;
   COL_VAR3(MessageSearch, ExactMatch, pMessage->Data);
   const bool MessageMatch = MATCHsearch(MessageSearch, pMessage->Data, ExactMatch, &MessagePositions, 50);
   const bool TagsMatch    = MATCHsearch(TagSearch, TagString, ExactMatch, &TagPositions);
   const bool Match        = MessageMatch && TagsMatch;
   COL_VAR2(Match, IncludeNonMatches);
   if(!Match && !IncludeNonMatches) {
      COL_TRC("Only include Matches");
      return false;
   }
   if(TagsMatch) { SDBSreorderTagMatches(TagSearch, ExactMatch, Item); }
   if(!Minify)   { Item["content"] = MATCHgenerateMarkup(pMessage->Data, MessagePositions, Formatted, true, 1000, 5); }
   Item["match"]   = Match;
   Results.push_back(Item);
   COL_VAR2(Match, Item.json(1));
   return Match;
}

static bool SDBSshouldStopRelevant(const bool Relevant, const bool HaveFirstMatch, const bool Forward,
                                   const COLuint64 Ts, const COLuint64 CutoffLower, const COLuint64 CutoffUpper) {
   COL_FUNCTION(SDBSshouldStopRelevant);
   if(!Relevant || !HaveFirstMatch) { return false; }
   return Forward ? (Ts > CutoffUpper) : (Ts < CutoffLower);
}

static void SDBSstartRelevantWindow(const bool Relevant, const bool Matched, const bool Forward, const COLuint64 Ts,
                                    bool& HaveFirstMatch, COLuint64& CutoffLower, COLuint64& CutoffUpper) {
   COL_FUNCTION(SDBSstartRelevantWindow);
   if(!Relevant || !Matched || HaveFirstMatch) { return; }
   HaveFirstMatch = true;
   if(Forward) {
      CutoffUpper = Ts + SDBS_24H_MS;
      COL_TRC("Relevant forward window set: first=" << Ts << " cutoffUpper=" << CutoffUpper);
   } else {
      CutoffLower = (Ts > SDBS_24H_MS) ? (Ts - SDBS_24H_MS) : 0;
      COL_TRC("Relevant reverse window set: first=" << Ts << " cutoffLower=" << CutoffLower);
   }
}

bool SDBSprocessList(const COLlist<SDBcontent*>& List, const int Limit, const bool Forward,
                     const COLstring& MessageSearch, const COLstring& TagSearch, const bool Exact,
                     const bool IncludeNonMatches, const bool Formatted, const bool Minify, const bool Relevant,
                     COLarray<COLvar>& Results) {
   COL_FUNCTION(SDBSprocessList);
   COL_VAR3(Limit, Forward, Relevant);

   bool      HaveFirstMatch = false;
   COLuint64 CutoffLower = 0, CutoffUpper = 0;

   if(Forward) {
      for(auto it = List.begin(); it != List.end(); ++it) {
         const auto* pMsg = (const SDBmessage*)(*it);
         const COLuint64 Ts = pMsg->Id.MessageId.DateTime;
         if(SDBSshouldStopRelevant(Relevant, HaveFirstMatch, true, Ts, CutoffLower, CutoffUpper)) {
            COL_TRC("Relevant forward cutoff reached: " << Ts << " > " << CutoffUpper);
            return false;
         }
         const bool Matched = SDBSprocessMessage(pMsg, MessageSearch, TagSearch, Exact, IncludeNonMatches, Formatted, Minify, Results);
         SDBSstartRelevantWindow(Relevant, Matched, true, Ts, HaveFirstMatch, CutoffLower, CutoffUpper);
         if(Results.size() >= Limit) {
            COL_TRC("Reached limit of " << Limit << ", returning.");
            return false;
         }
      }
   } else {
      for(auto it = List.rbegin(); it != List.rend(); ++it) {
         const auto* pMsg = (const SDBmessage*)(*it);
         const COLuint64 ts = pMsg->Id.MessageId.DateTime;
         if(SDBSshouldStopRelevant(Relevant, HaveFirstMatch, false, ts, CutoffLower, CutoffUpper)) {
            COL_TRC("Relevant reverse cutoff reached: " << ts << " < " << CutoffLower);
            return false;
         }
         const bool Matched = SDBSprocessMessage(pMsg, MessageSearch, TagSearch, Exact, IncludeNonMatches, Formatted, Minify, Results);
         SDBSstartRelevantWindow(Relevant, Matched, false, ts, HaveFirstMatch, CutoffLower, CutoffUpper);
         if(Results.size() >= Limit) {
            COL_TRC("Reached limit of " << Limit << ", returning.");
            return false;
         }
      }
   }
   return true;
}
