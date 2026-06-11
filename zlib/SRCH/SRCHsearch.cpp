//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SRCHsearch.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   30/01/25 12:27 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILutils.h>
#include <SRCH/SRCHcache.h>
#include <SRCH/SRCHsearch.h>

#include <cctype>
COL_LOG_MODULE;

static constexpr int SRCH_MAX_TOTAL_COUNT = 1000;

static bool SRCHisWhiteSpace(const char c) {
   return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

// we follow vscode's definition of a whole word match
static bool SRCHisFullWordMatch(const COLstring& Line, const size_t Pos, const size_t QueryLength) {
   COL_FUNCTION(SRCHisFullWordMatch);
   if(Pos > 0) {
      const char prev = Line[Pos - 1];
      if(isalnum(prev) || prev == '_') { return false; }
   }
   if(Pos + QueryLength < Line.size()) {
      const char next = Line[Pos + QueryLength];
      if(isalnum(next) || next == '_') { return false; }
   }
   return true;
}

static void SRCHprocessLineMatches(const COLstring& Line, const COLstring& Query, const size_t LineNum,
                                   const COLstring& FilePath, const bool MatchFullWord, const bool MatchCase,
                                   COLvar& Results) {
   COL_FUNCTION(SRCHprocessLineMatches);
   const COLstring SearchLine  = MatchCase ? Line : COLlowerCase(Line);
   const COLstring SearchQuery = MatchCase ? Query : COLlowerCase(Query);
   size_t          Pos         = 0;
   while(true) {
      if(Results["matches"].size() == SRCH_MAX_TOTAL_COUNT) {
         COL_TRC("Reached max count for matches");
         Results["max_results"] = true;
         break;
      }
      Pos = SearchLine.find(SearchQuery, Pos);
      if(Pos == npos) break;

      if(!MatchFullWord || SRCHisFullWordMatch(Line, Pos, Query.size())) {
         COL_VAR2(FilePath, Line);
         COLvar Match;
         Match["file"]        = FilePath;
         Match["line_number"] = static_cast<int>(LineNum + 1);
         Match["line"]        = Line;
         Match["position"]    = static_cast<int>(Pos + 1);
         Results["matches"].push_back(Match);
      }
      Pos += Query.size();
   }
}

static void SRCHprocessFile(const COLstring& FilePath, const COLstring& FileContent, const COLstring& Query,
                            const bool MatchFullWord, const bool MatchCase, COLvar& Results) {
   COL_FUNCTION(SRCHprocessFile);
   COL_VAR(FilePath);
   COLstring NormalizedContent = FileContent;  // need copy since replaceAll uses this code to write to file
   NormalizedContent.replace('\r', '\n');
   size_t Start   = 0;
   size_t End     = NormalizedContent.find('\n');
   size_t LineNum = 0;
   while(Start != npos) {
      const size_t LineEnd = (End == npos) ? NormalizedContent.size() : End;
      COLstring    Line    = NormalizedContent.substr(Start, LineEnd - Start);
      SRCHprocessLineMatches(Line, Query, LineNum, FilePath, MatchFullWord, MatchCase, Results);
      if(End == npos) break;  // Move to next line
      Start = End + 1;
      End   = NormalizedContent.find('\n', Start);
      LineNum++;
   }
}

static bool SRCHisWordCharacter(char c) {
   return isalnum(c) || c == '_';
}

static bool SRCHisWordQuery(const COLstring& Query) {
   for(int i = 0; i < Query.size(); i++) {
      const char c = Query[i];
      if(!SRCHisWordCharacter(c)) { return false; }
   }
   return true;
}

void SRCHsearch(const COLstring& Path, const COLstring& Query, const bool MatchCase, const bool MatchFullWord,
                COLvar* pResults) {
   COL_FUNCTION(SRCHsearch);
   COLvar& Results = *pResults;
   Results["too_large"].setArrayType(); // limit of 100mb applied in SRCHcache
   Results["matches"].setArrayType();
   Results["max_results"] = false;
   COLmap<COLstring, COLstring> Cache;
   SRCHcache(&Cache, Path, pResults);

   // VSCode-style whole word validation
   const bool ActualFullWord = MatchFullWord && SRCHisWordQuery(Query);
   for(const auto& it : Cache) {
      SRCHprocessFile(it.first, it.second, Query, ActualFullWord, MatchCase, Results);
      if(Results["max_results"].asBool()) {
         COL_TRC("Reached result limit");
         break;
      }
   }
   COL_TRC(Results.json(1));
}