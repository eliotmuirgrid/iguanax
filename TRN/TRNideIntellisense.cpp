// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNideIntellisense
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 25 April 2023 - 01:23PM
// ---------------------------------------------------------------------------

#include <TRN/TRNideIntellisense.h>
#include <TRN/TRNannotationJob.h>
#include <TRN/TRNintellisenseApi.h>
#include <TRN/TRNrequestCheckValid.h>

#include <PRM/PRMcallbacks.h>

#include <ROL/ROLid.h>

#include <SCK/SCKloop.h>

#include <COL/COLvarUtils.h>
#include <COL/COLclosure6.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

// on worker thread
static void TRNideIntellisenseWorker(TRNannotationJob* pJob, COLvar P, SCKloop* pLoop, COLaddress Address){
   COL_FUNCTION(TRNideIntellisenseWorker);
   COL_VAR(P.json(1));
   if (!TRNcheckRequestStillValid(Address.ConnectionId)) { COL_TRC("invalid job - exit."); delete pJob; return; }
   TRNideBoilerPlate(P, pJob);
   COLvar Result;
   try{
      TRNintellisenseApi(P, pJob, &Result);
      COL_VAR(Result.json(1));
      COLrespondSuccess(Address, Result);
   } catch(const COLerror& Error){
      COL_VAR(Error);
      COLrespondError(Address, Error.description());      
   }
   delete pJob;
}

// Note that unlike annotation jobs, each intellisense request creates its own annotation state. This is because it needs to 
// re-compile the "current file" to take in any user input which we call "modified code". If we were to try and cache these, 
// then each intellisense request would require two compiles:
//  - one at the start of the request to get the modified code and generate the intellisense options, and then 
//  - another at the end where we re-compile with the unmodified code in order to not cache the modified code changes to the state.
// Thus it makes more sense to just have each intellisense request be its own job.

// ELIOT - perhaps we could come up with a better algorithm where we don't use a brute force approach of altering the code - instead could we evaluate the expression?

// /ide/intellisense
void TRNideIntellisense(const COLwebRequest& Request, SCKloop* pLoop){
   COL_FUNCTION(TRNideIntellisense);
   if (!TRNcheckRequestStillValid(Request.Address.ConnectionId)) { COL_TRC("invalid job - exit."); return; }
   const COLstring Component = COLvarRequiredString(Request.Data, "component");
   bool IsDev = COLvarRequiredBool(Request.Data, "development");
   ROLid Id = IsDev ? ROLid::EDIT_SCRIPT : ROLid::VIEW_SCRIPT;
   COLstring Error;
   if (!PRMcheckRoleAccess(false, Request.User, Component, Id, &Error)){
      return COLrespondError(Request.Address, PRMsetError(Id));
   }
   TRNannotationJob* pTranslatorJob = new TRNannotationJob();
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&TRNideIntellisenseWorker, pTranslatorJob, Request.Data, pLoop, Request.Address));
}
