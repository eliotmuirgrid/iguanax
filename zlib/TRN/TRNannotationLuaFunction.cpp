// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNannotationLuaFunction
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Wednesday 03 May 2023 - 01:50PM
// ---------------------------------------------------------------------------

#include <TRN/TRNannotationLuaFunction.h>
#include <TRN/TRNannotationState.h>
#include <TRN/TRNargumentFormat.h>
#include <TRN/TRNargumentPopulateCache.h>
#include <TRN/TRNcountOfLocals.h>

#include <COL/COLlog.h>
#include <LUA/LUAerror.h>
COL_LOG_MODULE;

#define TRN_MAX_STACK_DEPTH 100  // enough to cover a tail function over 64k characters

static int TRNgetStackLevel(lua_Debug* pD){
   COL_VAR(TRNgetStackLevel);
   // "For LuaJIT, the cheapest way to check the stack level is to look
   // at the lowest 16 bits of the (private) ar->i_ci field"
   // -Mike Pall
   return 0x0000FFFF & pD->i_ci;
}

// The main problem with the Lua debug API is that we "don't have access to the return values".
// In actuality, the return values are on the top of the stack.  The problem is, we don't know
// how many return values there are (local variables are on the stack as well).
// Variable names have been shortened for easy reading (arithmetically).
static int TRNguessNumberOfReturnValues(const int StackSize, const int LocalVariableCount){
   COL_FUNCTION(TRNguessNumberOfReturnValues);
   if (StackSize <= 0){
      return 0; // empty stack means nothing is returned.
   } else if (LocalVariableCount < StackSize){
      // If there's anything on the stack other than local values,
      // it's return values.
      return StackSize - LocalVariableCount;
   } else { // StackSize <= LocalVariableCount
      // The only time the stack size is less than or equal to the number of
      // locals is when we are returning a single value which is already
      // on the stack (ie, a local or a parameter), so values are popped
      // or kept the same, so that the local value will be on top.
      return 1;
   }
}

// This is called after we actually enter a Lua function
static void TRNannotationLuaFunction(lua_State* L, COLlist<TRNframe>& Stack, COLlist<COLstring>& HashStack){
   COL_FUNCTION(TRNannotationLuaFunction);
   COLstring CalleeAddress = TRNcurrentAddress(HashStack); 
   TRNfunctionCall* pCallee = TRNcurrentFunctionCall(Stack);
   if (!pCallee->m_pFunction){
      COL_TRC("the callee is not a function - early return.");
      return;
   }
   int CallerLine = -1;
   TRNfunctionCall* pCaller = TRNfindCaller(Stack, &CallerLine);
   COL_VAR2(Stack.size(), pCaller);
   COL_TRC("Function " + pCallee->m_Name + " was called");
   // From debugging a require crash, IX-2511:
   // there's no guarantee that pCaller points to a valid location, nor that the frames in the stack are valid
   // trying to dereference pCaller or access the frame from the stack could lead to a crash
   int CountOfParam = Stack[Stack.last()].m_CountOfParam;
   COL_VAR4(CalleeAddress, *pCallee, CallerLine, CountOfParam);
   COL_TRC("Generate annotations for the function " << pCallee->m_Name);
   TRNhtmlCreateFunctionCall(L, CountOfParam, pCaller, pCallee, CallerLine, CalleeAddress);
}

// Return the number of items that were pushed onto the stack,
// so we can be sure to pop them when we're done with them.
static int TRNgetReturnInfo(lua_State* L, lua_Debug* pD, TRNreturnInfo* pReturnInfo){
   COL_FUNCTION(TRNgetReturnInfo);
   int T = lua_gettop(L);
   int ItemsToPop = 0;
   int FirstReturnIndex, LastReturnIndex;

   int CountOfLocal = TRNcountOfLocals(L, pD);
   int CountOfReturnValue = TRNguessNumberOfReturnValues(T, CountOfLocal); 
   COL_VAR3(T, CountOfLocal, CountOfReturnValue);
   FirstReturnIndex = T - CountOfReturnValue + 1;
   LastReturnIndex = T;

   pReturnInfo->StartParamIndex = FirstReturnIndex;
   pReturnInfo->EndParamIndex = LastReturnIndex;

   COL_VAR3(FirstReturnIndex, LastReturnIndex, ItemsToPop);
   return ItemsToPop;
}

// ReturnLine is the line on which the top frame's return statement
// should be annotated.
// We only used to pop multiple frames in LuaJIT2, since it doesn't execute
// the "tail return" hook (we have to infer ourselves when tail
// calls have occurred).
static void TRNpopLuaFrameOrFrames(lua_State* L, int ReturnLine, TRNreturnInfo& MostRecentReturnInfo, COLstring* pCurrentAddress, COLlist<TRNframe>& Stack, COLlist<COLstring>& HashStack){
   COL_FUNCTION(TRNpopLuaFrameOrFrames);
   TRNfunctionCall Callee = Stack.back().m_Current;
   COLstring CalleeAddress = TRNcurrentAddress(HashStack);
   TRNpopFrame(pCurrentAddress, Stack, HashStack);
   COL_VAR4(TRNcurrentFunctionCall(Stack), TRNcurrentLine(Stack), ReturnLine, CalleeAddress);
   TRNhtmlLuaFunctionReturn  (L, MostRecentReturnInfo, TRNcurrentFunctionCall(Stack), &Callee, TRNcurrentLine(Stack), ReturnLine, CalleeAddress);
}

void TRNsetupLFunctionFrame(TRNfunction* pCallee, lua_State* L, lua_Debug* pD, const char* pSource, COLlist<TRNframe>& Stack, COLlist<COLstring>& HashStack,
                         COLhashmap<COLstring, TRNfile*>& LuaFileList, COLhashmap<const void*, COLstring, COLhashPointer<const void*> >& LFunctionNameMap, 
                         COLstring* pCurrentAddress, int& LastCallStackDepth){
   COL_FUNCTION(TRNsetupLFunctionFrame);
   COL_VAR(Stack.size());
   if (Stack.size() > TRN_MAX_STACK_DEPTH){
      // TODO could put more information on the call stack here.
      LUA_ERROR_STREAM("Call stack has exceeded the maximum of depth of " << TRN_MAX_STACK_DEPTH << "." << newline << "Verify a function is not calling itself by accident.");
   }
   // Make initial guess at number of parameters, in case TRNhookAfterCall doesn't work (because the function is empty).
   // Go backwards from the top of the stack, until we find the first non-nil item.  We'll guess that's the last parameter.
   int ParamCountGuess = lua_gettop(L);
   while (ParamCountGuess > 0 && lua_isnil(L, ParamCountGuess)){ ParamCountGuess--; }
   int InternalStackLevel = TRNgetStackLevel(pD);
   COL_VAR2(ParamCountGuess, InternalStackLevel);

   COLstring Name, Address;
   TRNgetFunctionNameAndAddress(LuaFileList, LFunctionNameMap, L, pD, Name, Address);
   COL_VAR(Name);
   TRNeventAppendAddress(Address, pCurrentAddress, Stack, HashStack);

   TRNfunctionCall* pCaller = TRNcurrentFunctionCall(Stack);
   if (pCaller && TRNfunctionCallIsTargetCall(*pCaller)) {
      TRNfunctionOnFunctionCalled(pCaller->m_pFunction,TRNcurrentAddress(HashStack));
   }
   COLlistPlace FramePlace = Stack.add();
   TRNframe& NewFrame = Stack[FramePlace];
   TRNframeInit(&NewFrame,InternalStackLevel, -1, Name, pCallee, ParamCountGuess, pSource);
   COL_TRC("Adding new stack frame");
   LastCallStackDepth = Stack.size();
   if (TRNfunctionCallIsTargetCall(*TRNcurrentFunctionCall(Stack))){
      TRNfunction* pFunction = TRNcurrentFunctionCall(Stack)->m_pFunction;
      TRNfunctionOnLine(pFunction,pFunction->m_Start); // Guarantee that pFunction's annotations get initialized
      pFunction->m_TargetCallName = Name;
      COL_TRC("Function " << Name << " is the target call, initializing annotations");
   }
}

// A Lua function was called, here we set the parameters to the stack frame.
void TRNannotationLuaCall(lua_State* L, int CountOfParam, int FirstBodyLine, COLlist<TRNframe>& Stack, COLlist<COLstring>& HashStack, int LastCallStackDepth){
   COL_FUNCTION(TRNannotationLuaCall);
   COL_VAR2(CountOfParam, FirstBodyLine);
   COL_VAR2(Stack.size(), LastCallStackDepth);
   COLPRECONDITION(Stack.size() > 0);
   if (Stack.size() != LastCallStackDepth) {
      COL_TRC("Ignoring.");
      return; // ignore
   }

   TRNframe& StackFrame = Stack[Stack.last()];
   StackFrame.m_CurrentLine = FirstBodyLine;
   StackFrame.m_ReceivedParams = true;
   COL_VAR2(StackFrame.m_CountOfParam, CountOfParam);
   if (StackFrame.m_CountOfParam == 0 || CountOfParam > 0){
      COL_TRC("Using CountOfParam since it's better than the initial guess");
      StackFrame.m_CountOfParam = CountOfParam;
      COL_VAR(StackFrame.m_CountOfParam);
   } else {
      COL_TRC("Sticking with StackFrame.m_CountOfParam - we probably have an empty function.");
   }
   COL_TRC("Now entering function");
   TRNannotationLuaFunction(L, Stack, HashStack);
}

void TRNannotationLuaReturn(lua_State* L, lua_Debug* pD, COLlist<TRNframe>& Stack, COLlist<COLstring>& HashStack, bool& IgnoreNextReturn, 
                  int LastCallStackDepth, TRNreturnInfo* pMostRecentReturnInfo, COLstring* pCurrentAddress, COLhashmap<COLstring, TRNarg>& FullArgumentCache, COLarray<TRNargRequest>& FullArgRequests){
   COL_FUNCTION(TRNannotationLuaReturn);
   COLPRECONDITION(Stack.size() > 0);
   int ReturnLine = pD->currentline;
   COL_VAR2(ReturnLine, IgnoreNextReturn);
   if (IgnoreNextReturn){
      IgnoreNextReturn = false;
   } else {
      // Get the params here, in the case that the function has no body.
      if (!Stack.back().m_ReceivedParams){
         TRNannotationLuaCall(L, Stack.back().m_CountOfParam, ReturnLine, Stack, HashStack, LastCallStackDepth);
         TRNargumentCachePopulateLuaCall(L, Stack, HashStack, FullArgumentCache, FullArgRequests);
      }
      int ItemsToPop = TRNgetReturnInfo(L, pD, pMostRecentReturnInfo);
      try {
         COL_VAR2(Stack.size(), HashStack.size());
         COL_TRC("Now popping frames");
         // We have to store the calle and callee address so we can correctly populate the arg cache for lua function returns.
         TRNfunctionCall CachedCallee = Stack.back().m_Current;
         COLstring CachedCalleeAddress = TRNcurrentAddress(HashStack);
         TRNpopLuaFrameOrFrames(L, TRNcurrentLine(Stack), *pMostRecentReturnInfo, pCurrentAddress, Stack, HashStack);
         COL_VAR2(Stack.size(), HashStack.size());
         TRNargumentCachePopulateLuaReturn(L, *pMostRecentReturnInfo, CachedCalleeAddress, TRNcurrentFunctionCall(Stack), TRNcurrentLine(Stack), &CachedCallee, FullArgumentCache, FullArgRequests);
         lua_pop(L, ItemsToPop);
      } catch (...) {
         lua_pop(L, ItemsToPop);
         COL_TRC("throw");
         throw;
      }
   }
}

static const COLstring TRNreturnString("return ");
static const COLstring TRNreturnNothingString("<span class=\'lc\'>-- nothing returned</span>");
static const COLstring TRNarrow("->");

void TRNannotationLuaReturnString(lua_State* L, const TRNreturnInfo& ReturnInfo, bool IsReturn, const COLstring& FunctionAddress, COLstring* pArgString){
   COL_FUNCTION(TRNannotationLuaReturnString);
   COL_VAR2(IsReturn, FunctionAddress);
   if (!pArgString->is_null()) return;  // done already.
   COLostream Stream(*pArgString);
   int T = lua_gettop(L);
   TRNargumentsFormat(L, ReturnInfo.StartParamIndex, ReturnInfo.EndParamIndex, FunctionAddress, IsReturn, Stream);
}

static void TRNwriteNoReturnValues(TRNfunctionCall* pCaller, int CallerLine, TRNfunctionCall* pCallee, int ReturnLine, const COLstring& FunctionAddress){
   COL_FUNCTION(TRNwriteNoReturnValues);
   TRNline* pLine = TRNfunctionOnLine(pCallee->m_pFunction,ReturnLine);
   if (pLine){
      if (pCaller){
         COLstring ReturnAnnotation;
         COLostream ReturnStream(ReturnAnnotation);
         TRNstreamFcout(ReturnStream, TRNreturnNothingString, "fr", pCaller, pCallee, CallerLine, FunctionAddress);
         TRNlineAddAnnotation(pLine,ReturnAnnotation);
         COL_VAR(ReturnAnnotation);
      } else {
         TRNlineAddAnnotation(pLine, TRNreturnNothingString);
      }
   }
}

static void TRNformatLuaFunctionReturn(lua_State* L, TRNfunctionCall* pCaller, int CallerLine, TRNfunctionCall* pCallee, int ReturnLine, const TRNreturnInfo& ReturnInfo, const COLstring& FunctionAddress, COLstring* pArgString){
   COL_FUNCTION(TRNformatLuaFunctionReturn);
   if (!TRNfunctionCallIsTargetCall(*pCallee)) {
      return;
   }
   COL_TRC("Annotating return data for " << pCallee->m_Name);
   if (ReturnInfo.noReturns()){
      COL_TRC("Function " << pCallee->m_Name << " does not return any values");
      TRNwriteNoReturnValues(pCaller, CallerLine, pCallee, ReturnLine, FunctionAddress);
      return;
   }
   TRNannotationLuaReturnString(L, ReturnInfo, true, FunctionAddress, pArgString);
   TRNline* pLine = TRNfunctionOnLine(pCallee->m_pFunction,ReturnLine);
   if (pLine){
      COLstring ReturnAnnotation;
      COLostream ReturnStream(ReturnAnnotation);
      if (pCaller){
         TRNstreamFcout(ReturnStream, TRNreturnString, "fr", pCaller, pCallee, CallerLine, FunctionAddress);
      } else {
         ReturnStream << TRNreturnString;
      }
      ReturnStream << *pArgString;
      TRNlineAddAnnotation(pLine, ReturnAnnotation);
      COL_VAR(ReturnAnnotation);
   }
}

static void TRNformatReturnAnnotation(TRNfunctionCall* pCaller, const TRNreturnInfo& ReturnInfo, TRNfunctionCall* pCallee, int Line, lua_State* L, const COLstring& FunctionAddress, COLstring* pArgString){
   COL_FUNCTION(TRNformatReturnAnnotation);
   if (!pCaller || ReturnInfo.noReturns() || !TRNfunctionCallIsTargetCall(*pCaller)) return;
   TRNannotationLuaReturnString(L, ReturnInfo, true, FunctionAddress, pArgString);
   TRNline* pLine = TRNfunctionOnLine(pCaller->m_pFunction,Line);
   if (pLine){
      TRNlineAddReturn(pLine, TRNarrow + *pArgString);
   }
}

void TRNhtmlLuaFunctionReturn(lua_State* L, const TRNreturnInfo& ReturnInfo, TRNfunctionCall* pCaller, TRNfunctionCall* pCallee, int CallerLine, int CalleeLine, const COLstring& FunctionAddress){
   COL_FUNCTION(TRNhtmlLuaFunctionReturn);
   COLstring ArgString;
   TRNformatLuaFunctionReturn(L, pCaller, CallerLine, pCallee, CalleeLine, ReturnInfo, FunctionAddress, &ArgString);
   TRNformatReturnAnnotation(pCaller, ReturnInfo, pCallee, CallerLine, L, FunctionAddress, &ArgString);  
}

COLhashmapPlace TRNfindLuaFunctionName(COLhashmap<const void*, COLstring, COLhashPointer<const void*> >& LFunctionNameMap, const void* pFunction){
   COL_FUNCTION(TRNfindLuaFunctionName);
   return LFunctionNameMap.find(pFunction);
}

const COLstring& TRNluaFunctionName(COLhashmap<const void*, COLstring, COLhashPointer<const void*> >& LFunctionNameMap, COLhashmapPlace FunctionPlace){
   COL_FUNCTION(TRNluaFunctionName);
   return LFunctionNameMap[FunctionPlace];
}

void TRNsetLuaFunctionName(COLhashmap<const void*, COLstring, COLhashPointer<const void*> >& LFunctionNameMap, const void* pFunction, const COLstring& FunctionName){
   COL_FUNCTION(TRNsetLuaFunctionName);
   LFunctionNameMap[pFunction] = FunctionName;
}
