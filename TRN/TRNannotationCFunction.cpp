// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNannotationCFunction
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Wednesday 03 May 2023 - 03:44PM
// ---------------------------------------------------------------------------
#include <TRN/TRNannotationCFunction.h>
#include <TRN/TRNargumentPopulateCache.h>
#include <TRN/TRNargumentFormat.h>
#include <TRN/TRNcfunction.h>
#include <TRN/TRNfunction.h>
#include <TRN/TRNscope.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void TRNhtmlCFunction(lua_State* L, TRNfunctionCall* pCaller, TRNcfunction* pCallee, int CallerLine, const COLstring& CalleeAddress){
   COL_FUNCTION(TRNhtmlCFunction);
   if (!pCaller || !TRNfunctionCallIsTargetCall(*pCaller)) return;
   COL_VAR3(!pCaller, !TRNfunctionCallIsTargetCall(*pCaller), pCaller->m_Call);
   COL_TRC("Adding html for " << TRNcFunctionDisplayName(pCallee) << " call");
   TRNline* pLine = TRNfunctionOnLine(pCaller->m_pFunction,CallerLine);
   if (pLine){
      COLstring Annotation;
      COLostream Stream(Annotation);
      Stream << TRNcFunctionDisplayName(pCallee) << '(';
      TRNargumentsFormat(L, 1, lua_gettop(L), CalleeAddress, false, Stream);
      Stream << ')';
      COL_VAR(Annotation);
      TRNlineAddAnnotation(pLine,Annotation);
   }
}

static void TRNhtmlCFunctionReturn(lua_State* L, TRNfunctionCall* pCaller, TRNcfunction* pCallee, int CallerLine, const COLstring& CalleeAddress){
   COL_FUNCTION(TRNhtmlCFunctionReturn);
   if (!pCaller || lua_gettop(L) == 0) return;
   COL_TRC("Adding html for " << TRNcFunctionDisplayName(pCallee) << " return");
   if (TRNfunctionCallIsTargetCall(*pCaller)){      
      TRNline* pLine = TRNfunctionOnLine(pCaller->m_pFunction,CallerLine);
      if (pLine){
         COLstring Annotation;
         COLostream Stream(Annotation);
         Stream << COL_T("->");
         TRNargumentsFormat(L, 1, lua_gettop(L), CalleeAddress, true, Stream);
         COL_VAR(Annotation);
         TRNlineAddReturn(pLine,Annotation);
      }
   }
}

static const COLstring TRNuntracedFunctionCallSpan("<span class=\'ufc\' title=\'Function was called, but parameters and return values could not be determined.\'>");

static void TRNhtmlUntracedCFunction(lua_State* L, const char* pName, TRNfunctionCall* pCaller, int CallerLine){
   COL_FUNCTION(TRNhtmlUntracedCFunction);
   COL_TRC("Adding html for untraced C function: " << pName);
   if (!pCaller || !TRNfunctionCallIsTargetCall(*pCaller)) return;
   TRNline* pLine = TRNfunctionOnLine(pCaller->m_pFunction,CallerLine);
   if (pLine){
      TRNlineAddAnnotation(pLine, TRNuntracedFunctionCallSpan + pName + COLstring("(?)</span>"));
   }
}

void TRNannotationUntracedCFunction(lua_State* L, lua_Debug* pD, COLlist<TRNframe>& Stack) {
   COL_FUNCTION(TRNannotationUntracedCFunction);
   COLPRECONDITION(Stack.size());
   // Only annotate named functions that are not metamethods.
   if (pD->name && strlen(pD->name) > 0 && pD->name[0] != '_'){
      COL_TRC("Reached untraced C function that was name and not a metamethod. Will add annotation html.");
      TRNhtmlUntracedCFunction(L, pD->name, TRNcurrentFunctionCall(Stack), TRNcurrentLine(Stack));
   } else {
      COL_TRC("Untraced C function either has no name or is a metamethod. Not annotating.");
   }
}

void TRNannotationCFunction(lua_State* L, TRNfunctionCall* pCaller, TRNcfunction* pCallee, int CallerLine, const COLstring& CalleeAddress, COLhashmap<COLstring, TRNarg>& FullArgumentCache, COLarray<TRNargRequest>& FullArgumentRequests){
   COL_FUNCTION(TRNannotationCFunction);
   COL_TRC("Reached C function call");
   TRNhtmlCFunction(L, pCaller, pCallee, CallerLine, CalleeAddress);
   TRNargumentCachePopulateCFunction(L, pCaller, pCallee, CallerLine, CalleeAddress, FullArgumentCache, FullArgumentRequests, false);
}

void TRNannotationCReturn(lua_State* L, TRNfunctionCall* pCaller, TRNcfunction* pCallee, int CallerLine, const COLstring& CalleeAddress, COLhashmap<COLstring, TRNarg>& FullArgumentCache, COLarray<TRNargRequest>& FullArgumentRequests){
   COL_FUNCTION(TRNannotationCReturn);
   COL_TRC("Reached C function return");
   TRNhtmlCFunctionReturn(L, pCaller, pCallee, CallerLine, CalleeAddress);
   TRNargumentCachePopulateCFunction(L, pCaller, pCallee, CallerLine, CalleeAddress, FullArgumentCache, FullArgumentRequests, true);
}

TRNcfunction* TRNaddCFunction(COLhashmap<COLstring, TRNcfunction*>& CFunctionList, const COLstring& Name){
   // This one is a little too noisy for regular tracing.
   // COL_FUNCTION(TRNaddCFunction);
   TRNcfunction*& cf = CFunctionList[Name];
   if (cf == 0) cf = new TRNcfunction(Name);
   return cf;
}

void TRNcleanUpCFunctions(COLhashmap<COLstring, TRNcfunction*>& List){
   COL_FUNCTION(TRNcleanUpCFunctions);
   for (auto i=List.begin(); i; ++i){
      delete i->second;
   }
}
