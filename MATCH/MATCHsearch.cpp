//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: MATCHsearch.cpp
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-07, 8:38p.m.
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <MATCH/MATCHregex.h>
#include <MATCH/MATCHsearch.h>
#include <MATCH/MATCHsql.h>
COL_LOG_MODULE;

bool MATCHsearch(const COLstring& Filter, const COLstring& String, const bool ExactMatch, MATCHpositions* pPositions,
                 const int MaxMatches) {
   COL_FUNCTION(MATCHsearch);
   pPositions->clear();
   const COLstring& Trimmed = COLstripWhiteSpace(Filter);
   if(Trimmed.is_null()) { return true; }
   bool Match = false;
   if(MATCHisRegex(Trimmed)) {  // format= /myRegex/
      COL_TRC("Use regex matching");
      Match = MATCHregex(Trimmed, String, pPositions, MaxMatches);
   } else {
      COL_TRC("Use SQL-like syntax parsing");
      Match = MATCHsql(Trimmed, String, ExactMatch, pPositions, MaxMatches);
   }
   COL_VAR3(Trimmed, Match, pPositions->size());
   return Match;
}

bool MATCHsearch(const COLstring& Filter, const COLstring& String, const bool ExactMatch, const int MaxMatches) {
   COL_FUNCTION(MATCHsearch);
   MATCHpositions Positions;
   return MATCHsearch(Filter, String, ExactMatch, &Positions, MaxMatches);
}