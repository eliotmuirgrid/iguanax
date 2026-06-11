//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAXloops
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Wednesday, October 12th, 2011 @ 03:06:25 PM
//---------------------------------------------------------------------------
#include "LUAXloops.h"
#include "LUAXlexCode.h"

#include "lauxlib.h"
#include "llex.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

LUAXscope::LUAXscope() : m_Start(-1), m_End(-1), m_Type(FUNCTION), m_StartTokenPosition(-1), m_EndTokenPosition(-1) ,
m_CurrentIteration(0), m_TargetIteration(1), m_IterationOver(true), m_MostRecentLine(-1), m_Initialized(false) {
   COL_METHOD(LUAXscope::LUAXscope);
}
 
LUAXscope::~LUAXscope(){
   COL_METHOD(LUAXscope::~LUAXscope);
}

static const LUAXtoken* LUAXfindLoopsImpl(const LUAXtoken* pCurrentToken, LUAXscope& Scope){
   int NonRecursiveScopes = 1; // this function, "if" blocks, and inner functions
   LUAXscope::eType NextType = LUAXscope::DO_BLOCK;
   int NextLoopStart = -1;
   while (pCurrentToken){
      switch (pCurrentToken->Token){
         case TKX_END:
         case TKX_UNTIL:
            if (--NonRecursiveScopes == 0){
               Scope.m_End = pCurrentToken->Line;
               int CurrentTokenSize = (pCurrentToken->Token == TKX_END ? 3 : 5);
               Scope.m_EndTokenPosition = pCurrentToken->TokenEnd - CurrentTokenSize;
               return pCurrentToken;
            }
            break;
         case TKX_FUNCTION:
         case TKX_IF:
            ++NonRecursiveScopes;
            break;
         case TKX_DO:
         case TKX_REPEAT:{
            LUAXscope& InnerScope = Scope.m_Scopes.push_back();
            if (pCurrentToken->Token == TKX_REPEAT){
               NextType = LUAXscope::LOOP;
            }
            InnerScope.m_Type = NextType;
            if (NextLoopStart < 0){
               InnerScope.m_Start = pCurrentToken->Line;
            } else {
               InnerScope.m_Start = NextLoopStart;
            }
            InnerScope.m_StartTokenPosition = pCurrentToken->TokenEnd;
            pCurrentToken = LUAXfindLoopsImpl(pCurrentToken->pNext, InnerScope);
            NextType = LUAXscope::DO_BLOCK;
            NextLoopStart = -1;
            break;
         }
         case TKX_FOR:
         case TKX_WHILE:
            NextType = LUAXscope::LOOP;
            NextLoopStart = pCurrentToken->Line;
            break;
      }
      if (pCurrentToken) pCurrentToken = pCurrentToken->pNext;
   }

   return 0;
}

void LUAXfindLoops(const COLstring& LuaCode, int StartPosition, int StopPosition, LUAXscope& Scope){
   COL_FUNCTION(LUAXfindLoops);
   luax_State* L = lua_open();
   COLstring DummyString;
   bool DummyBool;
   COLauto<LUAXtoken> pHead = LUAXlexCode(L, LuaCode, "LUAXlexCode", StopPosition, false, DummyString, DummyBool);
   lua_close(L);

   const LUAXtoken* pCurrentToken = pHead.get();
   // Get to start position.
   while (pCurrentToken && pCurrentToken->TokenEnd < StartPosition){
      pCurrentToken = pCurrentToken->pNext;
   }
   // Get to beginning of function
   while (pCurrentToken && pCurrentToken->Token != TKX_FUNCTION){
      pCurrentToken = pCurrentToken->pNext;
   }

   if (pCurrentToken) Scope.m_StartTokenPosition = pCurrentToken->TokenEnd;
   if (pCurrentToken){
      Scope.m_Start = pCurrentToken->Line;
      LUAXfindLoopsImpl(pCurrentToken->pNext, Scope);
   }
}

void LUAXfindAllScopes(const COLstring& LuaCode, LUAXscope& Scope){
   COL_FUNCTION(LUAXfindAllScopes);
   luax_State* L = lua_open();\
   COLstring DummyString;
   bool DummyBool;
   COLauto<LUAXtoken> pHead = LUAXlexCode(L, LuaCode, "LUAXlexCode", LuaCode.size(), false, DummyString, DummyBool);
   lua_close(L);
   const LUAXtoken* pCurrentToken = pHead.get();
   while (pCurrentToken){
      if (pCurrentToken->Token == TKX_FUNCTION){
         LUAXscope& ChildScope = Scope.m_Scopes.push_back();
         ChildScope.m_Start = pCurrentToken->Line;
         ChildScope.m_StartTokenPosition = pCurrentToken->TokenEnd;
         pCurrentToken = LUAXfindLoopsImpl(pCurrentToken->pNext, ChildScope);
      }
      if (pCurrentToken) pCurrentToken = pCurrentToken->pNext;
   }
}

int LUAXscopeTargetIteration(const LUAXscope& Scope){
   if (Scope.m_TargetIteration == TRN_UNKNOWN_TARGET_ITERATION){
      return Scope.m_CurrentIteration;
   } else {
      return Scope.m_TargetIteration;
   }
}

COLostream& operator<<(COLostream& Stream, const LUAXscope& Scope){
   Stream << "S(" << Scope.m_Start << "," << Scope.m_End << ") target = " << LUAXscopeTargetIteration(Scope) << indent;
   // TODO - seems quite crazy we use a hash table for the lines rather than a COLmap?
   // Also the map is sparse
   for (int i=Scope.m_Start; i <= Scope.m_End; i++){
      if (Scope.m_Lines.count(i)){
         Stream << newline << "L" << i << " " << Scope.m_Lines[i].m_Data <<  " count = " << Scope.m_Lines[i].m_Count;   
      }
   }
   Stream << unindent << newline;
   if (Scope.m_Scopes.size() > 0){
      Stream << " children = " << Scope.m_Scopes.size() << indent;
      for (int i=0; i < Scope.m_Scopes.size(); i++){
         Stream << newline << Scope.m_Scopes[i];
      }
      Stream << unindent;
   }
   return Stream;
}