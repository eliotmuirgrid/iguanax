// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNideGlobalFunctionListExpand
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Friday 29 November 2024 - 12:09PM
// ---------------------------------------------------------------------------

#include <TRN/TRNideGlobalFunctionListExpand.h>
#include <TRN/TRNannotationJob.h>

#include <LUA/LUAheader.h>
#include <LUA/LUAutils.h>

#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void TRNlistIterateTable(lua_State *L, COLvar* pList) {
   COL_FUNCTION(TRNlistIterateTable);
   COLvar& List = *pList;
   lua_pushnil(L); // First key for lua_next
   while (lua_next(L, -2) != 0) {// Key is at index -2, value is at index -1
      if (lua_isfunction(L, -1) && !lua_iscfunction(L, -1)) {
         List.push_back(LUAtoString(L, -2));
      } else if (lua_istable(L, -1)) {
         COL_VAR(LUAtoString(L, -2));
         //TRNlistIterateTable(L, pList);
      }
      lua_pop(L, 1); // Pop the value, keep the key for the next iteration
   }
}

void TRNlistGlobalFunctions(lua_State *L, COLvar* pList) {
   COL_FUNCTION(TRNlistGlobalFunctions);
   pList->setArrayType();
   lua_pushvalue(L,LUA_GLOBALSINDEX);
   TRNlistIterateTable(L, pList);
   lua_pop(L, 1);
}

void TRNideGlobalFunctionListExpand(const COLvar& Input, TRNannotationJob* pTranslator, COLvar* pResult){
   COL_FUNCTION(TRNideGlobalFunctionListExpand);
   COLvar& Result = *pResult;
   TRNlistGlobalFunctions(pTranslator->L, &Result);
   COL_VAR(Result);
}