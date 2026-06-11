//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SRCHfind.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   30/01/25 4:38 PM
//  ---------------------------------------------------------------------------
#include <CFG/CFGconfigApi.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <PRM/PRMcallbacks.h>
#include <SRCH/SRCHfind.h>
#include <SRCH/SRCHsearch.h>
#include <SRCH/SRCHutils.h>

#include <algorithm>
COL_LOG_MODULE;

static void SRCHfindPostProcess(const COLstring& Query, COLvar& Matches, const COLstring& ParentDir) {
   COL_FUNCTION(SRCHfindPostProcess);
   if(!Matches.isArray()) { return; }
   int QueryLen = Query.size();
   for(auto& it : Matches.array()) {
      const int Position  = it["position"].asInt() - 1;
      COLstring Line      = it["line"];
      COLstring FilePath  = it["file"];
      it["file"]          = FilePath.substr(ParentDir.size());
      int       MaxSize   = QueryLen + 15;
      int       start     = std::max(0, Position - MaxSize);
      int       end       = std::min(static_cast<int>(Line.size()), Position + QueryLen + MaxSize);
      COLstring Truncated = Line.substr(start, end - start);

      // Only add ellipsis if we actually truncated
      if(start > 0 && Line.size() > (end - start)) Truncated = "..." + Truncated;
      if(end < Line.size() && Line.size() > (end - start)) Truncated += "...";

      // Calculate new position relative to truncated line
      int NewPos = Position - start;
      if(start > 0 && Line.size() > (end - start)) NewPos += 3;  // +3 for leading ellipsis only when added
      SRCHhighlightMatch(Truncated, NewPos, Query.size());
      it["line"] = Truncated;
   }
}

// /ide/search/find
void SRCHfind(const COLwebRequest& Request) {
   COL_FUNCTION(SRCHsearch);
   const COLstring Component   = COLvarRequiredString(Request.Data, "component");
   const bool      Development = COLvarGetBool(Request.Data, "development", true);
   const ROLid     Id          = Development ? EDIT_SCRIPT : VIEW_SCRIPT;
   COLstring       Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, Component, Id, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(Id));
   }
   const COLstring Path      = CFGresolveRepoFile(Component, COLvarGetString(Request.Data, "path", ""), Development);
   const COLstring Query     = COLvarRequiredString(Request.Data, "query");
   const bool      MatchFull = COLvarRequiredBool(Request.Data, "match_full_word");
   const bool      MatchCase = COLvarRequiredBool(Request.Data, "match_case");
   if(Path.is_null()) { return COLrespondError(Request.Address, "Attempting to access above component root"); }
   COLvar Results;
   SRCHsearch(Path, Query, MatchCase, MatchFull, &Results);
   SRCHfindPostProcess(Query, Results["matches"], CFGresolveRepoFile(Component, "", Development));
   COLrespondSuccess(Request.Address, Results);
}