// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNargumentPopulateCache
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Tuesday 02 May 2023 - 10:19AM
// ---------------------------------------------------------------------------
#include <TRN/TRNargumentPopulateCache.h>
#include <TRN/TRNannotationState.h>
#include <TRN/TRNargumentMatchIterator.h>
#include <TRN/TRNcfunction.h>
#include <TRN/TRNfile.h>
#include <TRN/TRNfunction.h>
#include <TRN/TRNreturnInfo.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TRNargumentCachePopulateLuaCall(lua_State* L, COLlist<TRNframe>& Stack, COLlist<COLstring>& HashStack, COLhashmap<COLstring, TRNarg>& FullArgumentCache, COLarray<TRNargRequest>& FullArgumentRequests){
   COL_FUNCTION(TRNargumentCachePopulateLuaCall);
   COLstring CalleeAddress = TRNcurrentAddress(HashStack);
   TRNfunctionCall* pCallee = TRNcurrentFunctionCall(Stack);
   int CallerLine = -1;
   TRNfunctionCall* pCaller = TRNfindCaller(Stack, &CallerLine);
   TRNargumentMatchIterator MatchIterator(FullArgumentRequests, CalleeAddress, false);
   while (TRNargRequest* pRequest = TRNargumentMatchIteratorFindNext(&MatchIterator)){
      TRNargumentReached(*pRequest, L, pRequest->m_ArgumentIndex, ((pCaller && pCaller->m_pFunction) ? pCaller->m_pFunction->m_pFile->m_Label : ""), pCallee->m_Name, pCallee->m_Call, CallerLine, FullArgumentCache);
   }
}

void TRNargumentCachePopulateCFunction(lua_State* L, TRNfunctionCall* pCaller, TRNcfunction* pCallee, int CallerLine, const COLstring& CalleeAddress, 
                                      COLhashmap<COLstring, TRNarg>& FullArgumentCache, COLarray<TRNargRequest>& FullArgumentRequests, bool IsReturn){
   COL_FUNCTION(TRNargumentCachePopulateCFunction);
   TRNargumentMatchIterator MatchIterator(FullArgumentRequests, CalleeAddress, IsReturn);
   while (TRNargRequest* pRequest = TRNargumentMatchIteratorFindNext(&MatchIterator)){
      TRNargumentReached(*pRequest, L, pRequest->m_ArgumentIndex, ((pCaller && pCaller->m_pFunction) ? pCaller->m_pFunction->m_pFile->m_Label : ""), TRNcFunctionDisplayName(pCallee), pCallee->m_Calls, CallerLine, FullArgumentCache);
   }
}

void TRNargumentCachePopulateLuaReturn(lua_State* L, const TRNreturnInfo& ReturnInfo, const COLstring& CalleeAddress, TRNfunctionCall* pCaller, int CallerLine,
                                         TRNfunctionCall* pCallee, COLhashmap<COLstring, TRNarg>& FullArgumentCache, COLarray<TRNargRequest>& FullArgumentRequests){
   COL_FUNCTION(TRNargumentCachePopulateLuaReturn);
   TRNargumentMatchIterator MatchIterator(FullArgumentRequests, CalleeAddress, true);
   while (TRNargRequest* pRequest = TRNargumentMatchIteratorFindNext(&MatchIterator)){
      COL_TRC("Found match");
      int RequestedReturnValue = ReturnInfo.StartParamIndex - 1 + pRequest->m_ArgumentIndex;
      TRNargumentReached(*pRequest, L, RequestedReturnValue, ((pCaller && pCaller->m_pFunction) ? pCaller->m_pFunction->m_pFile->m_Label : ""), pCallee->m_Name, pCallee->m_Call, CallerLine, FullArgumentCache);
   }
}
