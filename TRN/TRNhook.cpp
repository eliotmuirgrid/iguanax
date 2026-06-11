//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNhook
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday, November 18th, 2010 @ 02:43:52 PM
//---------------------------------------------------------------------------
#include <TRN/TRNhook.h>
#include <TRN/TRNargumentPopulateCache.h>
#include <TRN/TRNannotationState.h>
#include <TRN/TRNannotationCFunction.h>
#include <TRN/TRNannotationLuaFunction.h>
#include <TRN/TRNannotationNode.h>
#include <TRN/TRNconnectionCheck.h>
#include <TRN/TRNcountOfLocals.h>
#include <TRN/TRNfile.h>
#include <TRN/TRNfileFind.h>
#include <TRN/TRNloop.h>
#include <TRN/TRNtrace.h>
#include <TRN/TRNluaDebug.h>

#include <LND/LNDdebug.h>

#include <FIL/FILpathUtils.h>
#include <LUA/LUAerror.h>

#include <LUA/LUAtimeout.h>
#include <LUA/LUAutils.h>
#include <LUAL/LUALutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void TRNhookAfterCall(lua_State* L, lua_Debug* pD){
   COL_FUNCTION(TRNhookAfterCall);
   if (!TRNgetAnnotationState(L)->m_AnnotationsEnabled){
      COL_TRC("Annotations disabled.");
      return;
   }
   lua_getinfo(L, "ln", pD);  // important to understand that we need to call lua_getinfo to fill in the pD structure.
   COL_VAR3(*pD, TRNluaDebugName(pD), pD->currentline);
   TRNhookAnnotationSet(L); // reset original hook

   // At this point, the only named local variables should be the input parameters.
   int CountOfParam = TRNcountOfLocals(L, pD);
   COL_VAR3(lua_gettop(L),CountOfParam, LUAlocalsAsString(L, pD, 0, CountOfParam));

   TRNannotationState* pHandler = TRNgetAnnotationState(L);
   TRNannotationLuaCall(L, CountOfParam, pD->currentline, pHandler->m_Stack, pHandler->m_HashStack, pHandler->m_LastCallStackDepth);
   TRNmapAddressCacheClear(&pHandler->m_LastDestination, &pHandler->m_LastSource);
   TRNargumentCachePopulateLuaCall(L, pHandler->m_Stack, pHandler->m_HashStack, pHandler->m_FullArgumentCache, pHandler->m_FullArgumentRequests);
}

static void TRNhookCallCFunction(lua_State* L, lua_Debug* pD){
   COL_FUNCTION(TRNhookCallCFunction);
   COL_TRC("C function");
   lua_CFunction pCFunction = lua_tocfunction(L, -1);
   if (pCFunction != &TRNtraceC){
      COL_TRC("C function not wrapped by TRNtraceC: " << pD->name);
      TRNannotationState* pHandler = TRNgetAnnotationState(L);
      TRNannotationUntracedCFunction(L, pD, pHandler->m_Stack);
   } else {
      COL_TRC("C function - tracing handled in TRNtraceC");
   }
   lua_pop(L, 1);
}

static void TRNhookCallLuaFunction(lua_State* L, lua_Debug* pD){
   COL_FUNCTION(TRNhookCallLuaFunction);
   TRNannotationState* pHandler = TRNgetAnnotationState(L);
   if (TRNloopIsSecretFunc(pD->name)){
      // So far we only have one type of secret function - loop end.
      // If we add more, we can make TRNloopIsSecretFunc return an enum value.
      TRNloopOnEnd(pHandler->m_Stack);
      pHandler->m_IgnoreNextReturn = true;
   } else {
      TRNfunction* pFunction = NULL;
      // If pD->what is "main", it is referring to an actual chunk of code
      // (for example, the entire contents of a file), rather than a function
      // defined within a chunk.  Currently we don't annotate these.
      COLstring SourceWithoutAt(pD->source);
      SourceWithoutAt.replace("@","");
      SourceWithoutAt = FILpathSepToNative(SourceWithoutAt);
      COL_VAR(SourceWithoutAt);
      if (TRNfileLoaded(pHandler->m_LuaFileList, SourceWithoutAt) && strcmp(pD->what, "main") != 0){
         // TRNfileFindOrAdd calls FILpathSimplify so we don't need to worry about ./
         TRNfile* pFile = TRNfileFindOrAdd(pHandler->m_LuaFileList, pHandler->m_ProjectPath, SourceWithoutAt);
         pFunction = TRNfindOrCreateFunction(pFile, pD->linedefined);
         pFunction->m_End = pD->lastlinedefined;  // This is an important detail since we want to be able to create functions without knowing there end sometimes.
      }
      lua_sethook(L, &TRNhookAfterCall, LUA_MASKCOUNT, 1); // will be called before the first instruction in the function
      lua_pop(L, 1);
      TRNsetupLFunctionFrame(pFunction, L, pD, SourceWithoutAt.c_str(), pHandler->m_Stack, pHandler->m_HashStack, 
                     pHandler->m_LuaFileList, pHandler->m_LFunctionNameMap, &pHandler->m_CurrentAddress, pHandler->m_LastCallStackDepth);
   }
}

static void TRNhookCall(lua_State* L, lua_Debug* pD){
   COL_FUNCTION(TRNhookCall);
   if (!TRNgetAnnotationState(L)->m_AnnotationsEnabled){
      COL_TRC("Annotations disabled.");
      return;
   }
   lua_getinfo(L, "nSf", pD); // https://pgl.yoyo.org/luai/i/lua_getinfo
   COL_VAR3(*pD, TRNluaDebugName(pD), TRNluaDebugSource(pD)); 
   COL_VAR2(lua_gettop(L), LUAstackAsStringCustom(L, &LNDuserDataFormatter));
   if (lua_iscfunction(L, -1)){ TRNhookCallCFunction  (L, pD); } 
   else                       { TRNhookCallLuaFunction(L, pD); }
}

static void TRNhookRetLuaFunction(lua_State* L, lua_Debug* pD){
   COL_FUNCTION(TRNhookRetLuaFunction);
   COL_TRC("Lua function return");
   if (!TRNgetAnnotationState(L)->m_AnnotationsEnabled){
      return;
   }
   TRNannotationState* pHandler = TRNgetAnnotationState(L);
   lua_pop(L, 1);
   TRNmapAddressCacheClear(&pHandler->m_LastDestination, &pHandler->m_LastSource);
   TRNannotationLuaReturn(L, pD, pHandler->m_Stack, pHandler->m_HashStack, pHandler->m_IgnoreNextReturn, pHandler->m_LastCallStackDepth,
                   &pHandler->m_MostRecentReturnInfo, &pHandler->m_CurrentAddress, pHandler->m_FullArgumentCache, pHandler->m_FullArgumentRequests);
}

static void TRNhookRet(lua_State* L, lua_Debug* pD){
   COL_FUNCTION(TRNhookRet);
   lua_getinfo(L, "lSf", pD);
   if (lua_iscfunction(L, -1)){ 
      COL_TRC("C function return"); 
      lua_pop(L, 1);
   } else { 
      TRNhookRetLuaFunction(L, pD); 
   }
   COL_VAR(lua_gettop(L));
}

static void TRNhookTailRet(lua_State* L, lua_Debug* pD){
   COL_FUNCTION(TRNhookTailRet);
   TRNannotationState* pHandler = TRNgetAnnotationState(L);
   TRNframe& StackFrame = pHandler->m_Stack[pHandler->m_Stack.last()];
   int ReturnLine = StackFrame.m_CurrentLine;
   if (!StackFrame.m_ReceivedParams){
      TRNannotationLuaCall(L, StackFrame.m_CountOfParam, ReturnLine, pHandler->m_Stack, pHandler->m_HashStack, pHandler->m_LastCallStackDepth);
      TRNmapAddressCacheClear(&pHandler->m_LastDestination, &pHandler->m_LastSource);
      TRNargumentCachePopulateLuaCall(L, pHandler->m_Stack, pHandler->m_HashStack, pHandler->m_FullArgumentCache, pHandler->m_FullArgumentRequests);
   }

   TRNfunctionCall Callee = StackFrame.m_Current;
   COLstring CalleeAddress = TRNcurrentAddress(pHandler->m_HashStack);
   TRNpopFrame(&pHandler->m_CurrentAddress, pHandler->m_Stack, pHandler->m_HashStack);
   TRNhtmlLuaFunctionReturn  (L, pHandler->m_MostRecentReturnInfo, TRNcurrentFunctionCall(pHandler->m_Stack), &Callee, TRNcurrentLine(pHandler->m_Stack), ReturnLine, CalleeAddress);
   TRNargumentCachePopulateLuaReturn(L, pHandler->m_MostRecentReturnInfo, CalleeAddress, TRNcurrentFunctionCall(pHandler->m_Stack), TRNcurrentLine(pHandler->m_Stack), &Callee, pHandler->m_FullArgumentCache, pHandler->m_FullArgumentRequests);
}

void TRNhookLine(lua_State* L, lua_Debug* pD){
   COL_FUNCTION(TRNhookLine);
   lua_getinfo(L, "S", pD);
   COL_VAR2(pD->source, *pD);
   TRNannotationState* pCallback = TRNgetAnnotationState(L);
   TRNeventOnLine(pCallback->m_Stack, pD->currentline);
}

static void TRNeventDebugHook(lua_State* L, lua_Debug* pD){
   COL_FUNCTION(TRNeventDebugHook);
   COL_VAR(pD->event);
   if (!TRNgetAnnotationState(L)->m_AnnotationsEnabled){
      COL_TRC("Annotations are off.");
      return;
   }
   // Useful line for debugging this code --> COLcout << LUAstackAsStringCustom(L, &LNDuserDataFormatter) << newline;
   switch(pD->event){
   case LUA_HOOKCALL:    TRNhookCall           (L, pD); break;
   case LUA_HOOKRET:     TRNhookRet            (L, pD); break;
   case LUA_HOOKTAILRET: TRNhookTailRet        (L, pD); break;
   case LUA_HOOKLINE:    TRNhookLine           (L, pD); break;
   default:
      COLPRECONDITION(false);
   }
   // TODO need symmetric changes in BLUA to handling when we request a stop on a translator.
   if(!TRNconnectionCheck(L)){   // We cannot use HOOKCOUNT since we constantly reset the Lua hook.
      throw COLerror("Connection to browser has gone away, abandon annotation job"); //TODO -see if this can just be a LUAraiseError
   }
   LUAcheckTimeout(L);     // TODO unify LUAcheckTimeout and TRNconnectionCheck.
}

// Note we are constantly resetting the Lua debug hook.  I think this is required in order to 
// work around problems with getting the number of parameters for a Lua call.

void TRNhookAnnotationSet(lua_State* L){
   COL_FUNCTION(TRNhookAnnotationSet);
   lua_sethook(L, &TRNhookAfterCall, 0, 0); // turn off hook
   lua_sethook(L, &TRNeventDebugHook, LUA_MASKLINE | LUA_MASKCALL | LUA_MASKRET, 0); 
}

void TRNhookAnnotationUnset(lua_State* L){
   COL_FUNCTION(TRNhookAnnotationUnset);
   lua_sethook(L, &TRNeventDebugHook, 0, 0);
}