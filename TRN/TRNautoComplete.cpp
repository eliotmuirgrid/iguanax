// ---------------------------------------------------------------------------
// Copyright (C) 1997-2026 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNautoComplete
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Friday 06 March 2026 - 03:23PM
// ---------------------------------------------------------------------------

#include "TRNautoComplete.h"
#include "TRNautoCompleteInfo.h"

#include <LUA/LUAheader.h>
#include <LUA/LUAutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void TRNvalueExpand(lua_State* L, COLvar* pValue){
   COL_FUNCTION(TRNvalueExpand);
   COLvar& Value = *pValue;
   switch (lua_type(L, -1)){
   case LUA_TSTRING: Value = LUAtoString(L, -1); return;
   case LUA_TTABLE : Value.setMapType(); 
   }
}

static void TRNiterateTable(lua_State* L, TRNautoCompleteInfo* pInfo){
   COL_FUNCTION(TRNiterateTable);
   COL_VAR(pInfo);
   int t = lua_gettop(L); 
   COL_VAR(t);
   lua_pushnil(L);  // first key
   COL_TRC("Pushed nil");
   while (lua_next(L, t) != 0) {
      COL_TRC("First key");
      if (lua_type(L, -2) == LUA_TSTRING){
         // value is in 1
         COLlistPlace Item = pInfo->Options.add();
         TRNoption& Option = pInfo->Options[Item];
         Option.TextExpression = LUAtoString(L, -2);
         TRNvalueExpand(L, &Option.Value);
         COL_VAR2(Option.TextExpression, Option.Value);
      } 
      lua_pop(L, 1);  // remove value, keep key for next iteration
   }
}

void TRNautoCompleteChildOptions(lua_State* L, TRNautoCompleteInfo* pInfo){
   COL_FUNCTION(TRNautoCompleteChildOptions);
   if (lua_istable(L,-1)){
      COL_TRC("Is table.");
      TRNiterateTable(L, pInfo);
   }
  // COL_VAR(*pInfo);
}