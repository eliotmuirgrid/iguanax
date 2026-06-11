//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAXreturn
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, May 17th, 2011 @ 03:08:15 PM
//---------------------------------------------------------------------------

#include "LUAXreturn.h"
#include "LUAXlexCode.h"

#include "lauxlib.h"
#include "llex.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool LUAXdoesChunkReturn(const COLstring& LuaCode){
   COL_FUNCTION(LUAXdoesChunkReturn);
   luax_State* L = lua_open();
   COLstring DummyString;
   bool DummyBool;
   COLauto<LUAXtoken> pTail = LUAXlexCode(L, LuaCode, "LUAXlexCode", LuaCode.size(), true, DummyString, DummyBool);
   lua_close(L);
   const LUAXtoken* pCurrentToken = pTail.get();
   int CurrentScopeDepth = 0;
   while (pCurrentToken){
      switch(pCurrentToken->Token){
         case TKX_END:
         case TKX_UNTIL:
            CurrentScopeDepth++;
            break;
         case TKX_DO:
         case TKX_FUNCTION:
         case TKX_IF:
         case TKX_REPEAT:
            CurrentScopeDepth--;
            break;
         case TKX_RETURN:
            if (CurrentScopeDepth == 0){
               return true;
            }
            break;
      }
      pCurrentToken = pCurrentToken->pNext;
   }
   return false;
}
