//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SRCHutils
//
// Description:
//
// Implementation
//
// Author: Matthew Sobkowski
// Date:   Monday, June 20th, 2022
//---------------------------------------------------------------------------
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
#include <COL/COLmath.h>
#include <COL/COLvar.h>
#include <SFI/SFIhtmlEscape.h>
#include <SRCH/SRCHutils.h>
#include <ctype.h>
COL_LOG_MODULE;

// replace non-printable bytes with colorized '.'
// not the most efficient algo, but easy to understand.
COLstring SRCHescapeUnprintable(const COLstring& Raw) {
   COL_FUNCTION(SRCHescapeUnprintable);
   COLstring Out;
   for(int i = 0; i < Raw.size(); i++) {
      if(isprint(Raw[i])) {
         Out.append(1, Raw[i]);
      } else {
         Out.append("<span class='SRCHspecialChar'>.</span>");
      }
   }
   return Out;
}

void SRCHhighlightMatch(COLstring& Line, const size_t Pos, const size_t Length) {
   COL_FUNCTION(SRCHhighlightMatch);
   // Split line into parts
   const COLstring Before = Line.substr(0, Pos);
   const COLstring Match  = Line.substr(Pos, Length);
   const COLstring After  = Line.substr(Pos + Length);

   // Escape all parts
   COLstring EscapedBefore, EscapedMatch, EscapedAfter;
   SFIhtmlEscapeNoNewline(Before.c_str(), Before.size(), EscapedBefore);
   SFIhtmlEscapeNoNewline(Match.c_str(), Match.size(), EscapedMatch);
   SFIhtmlEscapeNoNewline(After.c_str(), After.size(), EscapedAfter);

   // Combine with highlight (strip whitespace BEFORE wrapping)
   Line = EscapedBefore + "<span style=\"background-color:#FFFBB9;\">" + COLstripWhiteSpace(EscapedMatch) + "</span>" +
          EscapedAfter;
   Line = SRCHescapeUnprintable(COLstripWhiteSpace(Line));
}

static COLstring SRCHgetMatchKey(const COLvar& item) {
   if(!item.exists("file") || !item.exists("line_number") || !item.exists("position")) { return COLstring(); }
   return item["file"].asString() + ":" + item["line_number"].asString() + ":" + item["position"].asString();
}

void SRCHfilterMatches(const COLvar& Selections, COLvar* pMatches) {
   COL_FUNCTION(SRCHfilterMatches);
   COL_VAR2(Selections.json(1), pMatches->json(1));
   if(!Selections.isArray()) {
      COL_TRC("Selections not array type -- skip filter " << Selections);
      return;
   }
   COLmap<COLstring, bool> SelectionsMap;
   for(const auto& sel : Selections.array()) {
      const COLstring key = SRCHgetMatchKey(sel);
      if(!key.is_null()) { SelectionsMap[key] = true; }
   }
   // Filter matches and duplicates
   COLmap<COLstring, bool> SeenMap;
   for(int i = pMatches->size() - 1; i >= 0; --i) {
      const COLstring key = SRCHgetMatchKey((*pMatches)[i]);
      if(key.is_null() || !SelectionsMap.find(key) || SeenMap.find(key)) {
         pMatches->remove(i);
         continue;
      }
      SeenMap[key] = true;
   }
}