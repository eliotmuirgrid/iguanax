//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNtrace
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, November 17th, 2010 @ 04:27:12 PM
//---------------------------------------------------------------------------
#include <TRN/TRNtrace.h>
#include <TRN/TRNannotationCFunction.h>
#include <TRN/TRNfunction.h>
#include <TRN/TRNcfunction.h>

#include <LUA/LUAutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

class TRNfunction;

int TRNcallWrappedFunc(lua_State* L, int C){
   COL_FUNCTION(TRNcallWrappedFunc);
   lua_pushvalue(L, lua_upvalueindex(1));
   lua_insert(L, 1);
   int Result = lua_pcall(L, C, LUA_MULTRET, 0);
   return Result;   
}

int TRNtraceC(lua_State* L){
   COL_FUNCTION(TRNtraceC);
   if (!TRNgetAnnotationState(L)->m_AnnotationsEnabled){
      COL_TRC("Debug hook is off so we call the wrapped function but nothing else.");
      if (0 != TRNcallWrappedFunc(L, lua_gettop(L))){
         COL_TRC(LUAtoString(L, -1)); 
         lua_error(L);
      }
      // This is to deal with when the user implements meta __tostring methods which would result in a loop of calls since
      // we use __tostring and __display meta methods in our implementation of annotations - See LUALsafeCallMeta
      // See IX-3548 too.
      return lua_gettop(L);
   }
   int C = lua_gettop(L);
   TRNcfunction* pCFunction = (TRNcfunction*)lua_touserdata(L, lua_upvalueindex(2));
   TRNcFunctionIncrementCalls(pCFunction);
   COL_TRC(COL_T("Call ") << pCFunction->m_Name << '(' << LUAstackAsString(L) << ')');
   TRNannotationState* pHandler = TRNgetAnnotationState(L);
   TRNeventAppendAddress(pCFunction->m_Name, &pHandler->m_CurrentAddress, pHandler->m_Stack, pHandler->m_HashStack);
   COL_TRC(pCFunction->m_Name << '(' << LUAstackAsString(L) << ')');
   if (pHandler->m_Stack.size()){ COL_TRC(TRNcurrentFunctionCall(pHandler->m_Stack)->m_Name << ':' << TRNcurrentLine(pHandler->m_Stack)); } // stack could be empty, if it is don't trace as that will segfault
   TRNannotationCFunction(L, TRNcurrentFunctionCall(pHandler->m_Stack), pCFunction, TRNcurrentLine(pHandler->m_Stack), TRNcurrentAddress(pHandler->m_HashStack), pHandler->m_FullArgumentCache, pHandler->m_FullArgumentRequests);

   if (pCFunction->m_FunctionId == TRNcfunction::ERROR){
      COL_TRC("The stack level argument for error() will be off, since we wrap C Functions.");
      if (lua_isnumber(L, 2)){
         int OriginalLevel = lua_tointeger(L, 2);
         TRNonErrorCall(pHandler->m_ErrorFile, pHandler->m_ErrorLine, pHandler->m_Stack, OriginalLevel);
         int NewLevel = OriginalLevel + 1;
         lua_pushinteger(L, NewLevel);
         lua_replace(L, 2);
      }
   }
   const int CallStackDepth = pHandler->m_Stack.size();
  
   int Result = TRNcallWrappedFunc(L, C);
   if (0 != Result){
      COL_TRC(LUAtoString(L, -1));
      TRNeventRemoveLastAddress(&pHandler->m_CurrentAddress, pHandler->m_HashStack);
      lua_error(L);    
   }
   COL_TRC(COL_T("->(") << LUAstackAsString(L) << ')');
   if (pCFunction->m_FunctionId == TRNcfunction::PCALL){
      // We do this in case an error was thrown the pcall. If this is the case, the event
      // handler may be "lost" somewhere inside the pcall's stack trace (ie, its call
      // stack will be out of date, containing extra functions).
      if (pHandler->m_Stack.size() != CallStackDepth){
         TRNreduceCallStackDepth(CallStackDepth, &pHandler->m_Stack, &pHandler->m_HashStack, &pHandler->m_CurrentAddress);
      }
      TRNclearErrorCall(&pHandler->m_ErrorFile, pHandler->m_ErrorLine);
   }   
   TRNannotationCReturn(L, TRNcurrentFunctionCall(pHandler->m_Stack), pCFunction, TRNcurrentLine(pHandler->m_Stack), TRNcurrentAddress(pHandler->m_HashStack), pHandler->m_FullArgumentCache, pHandler->m_FullArgumentRequests);
   TRNeventRemoveLastAddress(&pHandler->m_CurrentAddress, pHandler->m_HashStack);
   return lua_gettop(L);
}
