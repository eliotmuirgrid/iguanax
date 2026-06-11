// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUTILfindPattern
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 21 April 2025 - 01:22PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAtest.h>
#include <LUTIL/LUTILfindPattern.h>
#include <MATCH/MATCHsearch.h>

#include <COL/COLsplit.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// util.findPattern
int LUTILfindPattern(lua_State* L) {
   COL_FUNCTION(LUTILfindPattern);
   COLstring SearchString, StringToSearch;
   bool      ExactMatch;
   if(lua_istable(L, 1)) {
      SearchString   = LUAexpectedStringParam(L, 1, "searchString");
      StringToSearch = LUAexpectedStringParam(L, 1, "stringToSearch");
      ExactMatch     = LUAexpectedBoolParam(L, 1, "exactMatch");
   } else {
      return LUAraiseLuaError(L, "Table Required: {searchString= '', stringToSearch='', exactMatch=''}");
   }
   COL_VAR2(SearchString, StringToSearch);
   COLarray<COLstring> SearchTerms;
   COLsplit(&SearchTerms, SearchString, " ");
   const bool Found = MATCHsearch(SearchString, StringToSearch, ExactMatch);
   COL_VAR(Found);
   lua_pushboolean(L, Found);
   return 1;
}