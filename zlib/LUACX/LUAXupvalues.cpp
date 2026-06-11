//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAXupvalues
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Friday, September 30th, 2011 @ 02:07:15 PM
//---------------------------------------------------------------------------

#include "LUAXupvalues.h"
#include "LUAXlexCode.h"

#include "lauxlib.h"
#include "llex.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

enum LUAXupvalueCertainty{
   LUAX_EXCLUDE = 0,
   LUAX_MAYBE = 1,
   LUAX_INCLUDE = 2
};

// This function will assume syntactic correctness.  Any syntax error will produce an error elsewhere.
static const LUAXtoken* LUAXgetUpvalues(const LUAXtoken* pCurrentToken, const COLstring& LuaCode, int Position, COLlist<COLstring>& UpValues){
   int UpvaluesAddedThisScope = 0;
   // If this scope starts after the specified position, nothing in it will be an upvalue.
   // Otherwise, we have to add it (temporarily), because we don't know if this scope will end before Position.
   // By the end of the scope, if we're not in the state LUAX_INCLUDE, then we remove all upvalues we've added
   // from this scope.
   LUAXupvalueCertainty KeepUpvaluesFromThisScope = (pCurrentToken && pCurrentToken->TokenEnd > Position) ? LUAX_EXCLUDE : LUAX_MAYBE;

   while (pCurrentToken){
      switch(pCurrentToken->Token){
         case TKX_END:
         case TKX_UNTIL:
            if (KeepUpvaluesFromThisScope != LUAX_INCLUDE){
               while (UpvaluesAddedThisScope-- > 0) UpValues.remove(UpValues.last());
            }
            return pCurrentToken;
         case TKX_FUNCTION:{
            size_t FunctionBegin = pCurrentToken->TokenEnd;
            pCurrentToken = LUAXgetUpvalues(pCurrentToken->pNext, LuaCode, Position, UpValues);
            size_t FunctionEnd = pCurrentToken ? pCurrentToken->TokenEnd : LuaCode.size();
            if (FunctionBegin < Position && Position < FunctionEnd){
               KeepUpvaluesFromThisScope = LUAX_INCLUDE;
            }
            break;
         }
         case TKX_DO:
         case TKX_IF:
         case TKX_REPEAT:
            pCurrentToken = LUAXgetUpvalues(pCurrentToken->pNext, LuaCode, Position, UpValues);
            break;
         case TKX_LOCAL:
            if (KeepUpvaluesFromThisScope > LUAX_EXCLUDE){
               const LUAXtoken* pLookAhead = pCurrentToken->pNext;
               if (pLookAhead && pLookAhead->Token == TKX_FUNCTION){
                  pLookAhead = pLookAhead->pNext;
               }
               while (pLookAhead && pLookAhead->Token == TKX_NAME){
                  COLstring Name(LuaCode.substr(pLookAhead->TokenEnd - pLookAhead->TokenSize, pLookAhead->TokenSize));
                  UpValues.add(Name);
                  UpvaluesAddedThisScope++;
                  pLookAhead = pLookAhead->pNext;
                  if (pLookAhead && pLookAhead->Token == ','){
                     pLookAhead = pLookAhead->pNext;
                  }
               }
            }
            break;
      }
      if (pCurrentToken) pCurrentToken = pCurrentToken->pNext;
   }
   if (KeepUpvaluesFromThisScope != LUAX_INCLUDE){
      while (UpvaluesAddedThisScope-- > 0) UpValues.remove(UpValues.last());
   }
   return NULL;
}

void LUAXgetUpvalues(const COLstring& LuaCode, int Position, COLlist<COLstring>* pUpValues){
   COL_FUNCTION(LUAXgetUpvalues);
   COL_VAR2(LuaCode, Position);
   luax_State* L = lua_open();
   COLstring DummyString;
   bool DummyBool;
   COLauto<LUAXtoken> pHead = LUAXlexCode(L, LuaCode, "LUAXlexCode", LuaCode.size(), false, DummyString, DummyBool);
   lua_close(L);
   LUAXgetUpvalues(pHead.get(), LuaCode, Position, *pUpValues);
   COL_VAR(pUpValues->size());
}