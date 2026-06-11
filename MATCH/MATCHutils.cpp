//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: MATCHutils.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-08, 9:57p.m.
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <MATCH/MATCHutils.h>
COL_LOG_MODULE;

// find all exclusion positions (one in our CHNK library) -- add to the vector for more
void MATCHfindExclusions(const COLstring& Input, MATCHpositions* pPositions) {
   COL_FUNCTION(MATCHfindExclusions);
   COLarray<COLstring> Exclusions;
   Exclusions.push_back(R"(<span class="line_cropped_warning">&hellip; This line is too long to display.</span>)");
   for(const auto& snippet : Exclusions) {
      size_t pos = 0;
      while((pos = Input.find(snippet, pos)) != npos) {
         pPositions->push_back({pos, pos + snippet.size() - 1});
         pos += snippet.size();
      }
   }
}

bool MATCHoverlapsExclusions(const size_t Start, const size_t End, const MATCHpositions& Exclusions) {
   COL_FUNCTION(MATCHoverlapsExclusions);
   for(const auto& pos : Exclusions) {
      if(!(End < pos.first || Start > pos.second)) { return true; }
   }
   return false;
}

COLstring MATCHescapeRegex(const COLstring& String) {
   COL_FUNCTION(MATCHescapeRegex);
   COLstring       Regex;
   const COLstring Meta(".^$+?()[]{}|\\");
   for(size_t i = 0; i < String.size(); ++i) {
      const char c = String[i];
      if(Meta.find(c) != npos) {
         Regex += '\\';
         Regex += c;
      } else if(c == '*') {
         Regex += "[^ ]*";  // wildcard matches zero or more non-space characters
      } else {
         Regex += c;
      }
   }
   return Regex;
}