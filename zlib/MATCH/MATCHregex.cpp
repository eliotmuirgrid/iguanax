//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: MATCHregex.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-07, 8:51p.m.
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <MATCH/MATCHregex.h>
#include <REX/REXmatcher.h>
COL_LOG_MODULE;

static COLstring MATCHisolateRegex(const COLstring& Filter) {
   COL_FUNCTION(MATCHisolateRegex);
   if(Filter.size() >= 2 && Filter[0] == '/' && Filter[Filter.size() - 1] == '/') {
      return Filter.substr(1, Filter.size() - 2);
   }
   return Filter;
}

bool MATCHisRegex(const COLstring& Filter) {
   COL_FUNCTION(MATCHisRegex);
   try {
      if(Filter[0] != '/' || Filter[Filter.size() - 1] != '/') return false;
      const REXmatcher Regex(MATCHisolateRegex(Filter));
      const bool       IsRegex = Regex.isExpressionValid();
      COL_VAR(IsRegex);
      return IsRegex;
   } catch(const COLerror& E) {
      COL_ERR("Invalid regex -- " << E.description());
      return false;
   }
}

bool MATCHregex(const COLstring& Filter, const COLstring& String, MATCHpositions* pPositions,
                const int MaxMatches) {
   COL_FUNCTION(MATCHregex);
   if(String.is_null() || String.isWhitespace()) { return false; }
   try {
      const REXmatcher Matcher(MATCHisolateRegex(Filter));
      size_t           Start, End;
      const bool       Match = Matcher.findMatch(String.c_str(), 0, Start, End);
      while(Match && Start != npos && End != npos) {
         pPositions->push_back({Start, End});
         if(pPositions->size() == MaxMatches) {
            COL_TRC("Reached limit of " << MaxMatches << " matches");
            break;
         }
         if(End >= String.size()) {
            COL_TRC("Matching reached end of string");
            break;
         }
         Matcher.findMatch(String.c_str(), End + 1, Start, End);
      }
      COL_VAR3(Match, Start, End);
      return Match;
   } catch(const COLerror& E) {
      COL_ERR("Invalid regex -- " << E.description());
      return false;
   }
}