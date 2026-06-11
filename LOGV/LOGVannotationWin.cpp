//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LOGVannotationWindow.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   23/04/25 1:35 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <LOGV/LOGVannotationWin.h>
#include <LOGV/LOGVstate.h>
#include <TRN/TRNannotationState.h>
#include <TRN/TRNargumentRequestParse.h>
#include <TRN/TRNargumentResponse.h>
#include <TRN/TRNfile.h>
#include <TRN/TRNargumentInCache.h>
COL_LOG_MODULE;

void LOGVargumentExpand(const COLvar& Input, TRNannotationState* pState, COLvar* pResult) {
   COL_FUNCTION(LOGVargumentExpand);
   COL_VAR(Input);
   TRNargRequest ArgRequest;
   TRNargumentPopulate(Input, &ArgRequest);
   COLhashmapPlace ArgumentPlace = TRNargumentInCache(pState->m_FullArgumentCache, ArgRequest);
   if(ArgumentPlace) {
      COL_TRC("Found argument in cache");
      TRNarg& Arg = pState->m_FullArgumentCache[ArgumentPlace];
      TRNargumentPopulateResponse(Arg, ArgRequest);
      *pResult = ArgRequest.m_VarResult;
   } else {
      COL_TRC("Argument not found in cache.");
      (*pResult)["ErrorMessage"] = "Argument not found in cache.";
   }
   COL_VAR(pResult->json(1));
}

// log/viewer/annotation/window
void LOGVannotationWin(const COLwebRequest& Request) {
   COL_FUNCTION(LOGVannotationWin);
   TRNannotationState State;
   COLvar Result;
   COLstring Err;
   if(!LOGVinitState(Request.Data, &State, &Result, &Err)) { return COLrespondError(Request.Address, Err); }
   COLvar NewData = Request.Data;
   NewData["id"]  = Result["id"].asString();
   Result.reset();
   LOGVargumentExpand(NewData, &State, &Result);
   COLrespondSuccess(Request.Address, Result);
}