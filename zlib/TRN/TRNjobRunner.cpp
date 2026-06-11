// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNjobRunner
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 08 May 2023 - 10:48AM
// ---------------------------------------------------------------------------
#include <TRN/TRNjobRunner.h>
#include <TRN/TRNjobQueue.h>
#include <TRN/TRNannotation.h>
#include <TRN/TRNannotationJob.h>
#include <TRN/TRNannotationState.h>
#include <TRN/TRNannotationBlock.h>
#include <TRN/TRNargumentExpand.h>
#include <TRN/TRNautocompleteEnd.h>
#include <TRN/TRNcreateJobQueue.h>
#include <TRN/TRNconnectionCheck.h>
#include <TRN/TRNrequestCheckValid.h>
#include <TRN/TRNwriteScriptError.h>
#include <TRN/TRNideGlobalFunctionListExpand.h>

#include <CFG/CFGconfigApi.h>

#include <COL/COLclosure.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <LUA/LUAutils.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;
#define TRN_KEEP_ALIVE_TIME 60 // in seconds

static void TRNupdateLastActivity(TRNjobQueue* pRequestQueue){
   COL_FUNCTION(TRNupdateLastActivity);
   if (pRequestQueue){
      pRequestQueue->LastActivity = time(0); // update last activity
   }
}

// Returns true if the compile succeeded or was not required.
static bool TRNcompileIfNeeded(TRNjobQueue* pRequestQueue, COLvar* pResult){
   COL_FUNCTION(TRNcompileIfNeeded);
   COL_VAR(pRequestQueue->pJob->m_CompileNeeded);
   if(!pRequestQueue->pJob->m_CompileNeeded) { return true; }
   COLstring CompileError;
   TRNannotationResetMaps(&pRequestQueue->pJob->m_AnnotationState); // Important! The state will be compiled, so free the lua files and c functions to avoid leaking memory
   if(!TRNcompileAnnotationProgram(pRequestQueue->pJob, &CompileError)){
      COL_TRC("Ran into compile error");
      TRNwriteScriptError(pResult, CompileError, pRequestQueue->pJob->m_AnnotationState, true);
      return false;
   }
   COL_TRC("Compile was ok, flipping compile flag");
   pRequestQueue->pJob->m_CompileNeeded = false;
   return true;
}

static bool TRNprepareToRun(const COLvar& Input, TRNjobQueue* pRequestQueue, COLvar* pResult, COLaddress Address) {
   COL_FUNCTION(TRNprepareToRun);
   TRNideBoilerPlate(Input, pRequestQueue->pJob);
   if (!TRNcompileIfNeeded(pRequestQueue, pResult)) {
      const COLstring Code = Input["code"];
      int Line = Input["line"].asInt();
      // TODO - commented out for release.
      // COL_TRC("Compile failed, going to try inserting an end for the code on line " << Line);
       TRNautocompleteEnd(Code, Line, pResult, &pRequestQueue->pJob->m_AnnotationState);
      return false;
   }
   TRNsetConnectionIdInLua(pRequestQueue->pJob->L, Address.ConnectionId);
   TRNresetJobCancelled(pRequestQueue->pJob);
   return true;
}

static void TRNrunTask(const COLvar& Input, TRNjobQueue* pRequestQueue, COLvar* pResult, COLaddress Address, void (* pFunc)(const COLvar&, TRNannotationJob*, COLvar*)) {
   COL_FUNCTION(TRNrunTask);
   try {
      if (!TRNcheckRequestStillValid(Address.ConnectionId)) { return; }  // Connection is gone, no need to respond
      if (!TRNprepareToRun(Input, pRequestQueue, pResult, Address)) {
         return COLrespondSuccess(Address, *pResult); 
      }
      COL_TRC("Running translator job");
      (*pFunc)(Input, pRequestQueue->pJob, pResult);
      if (!TRNcheckRequestStillValid(Address.ConnectionId)) { return; }  // Connection is gone, no need to respond
      COLrespondSuccess(Address, *pResult);
   } catch (const COLerror& Error) {
      COL_TRC("Caught error during translator job");
      if (!TRNcheckRequestStillValid(Address.ConnectionId)) { return; }  // Connection is gone, no need to respond
      COL_TRC("Responding with error");
      COLrespondError(Address, Error.description());
   }
}

void TRNshutdownTask(TRNjobQueue* pRequestQueue, COLvar* pResult){
   COL_FUNCTION(TRNshutdownTask);
   if (pRequestQueue->pJob->L) { 
      COL_TRC("Running lua idle callback to delete curl and db connections - see DBDluaIdle"); 
      LUArunLuaIdleCallBack(pRequestQueue->pJob->L); 
   }
   pRequestQueue->IsRunning = false;
   COL_TRC("Deleting annotation job (which closes the lua state)");
   delete pRequestQueue->pJob;
   pRequestQueue->pRemoveFromMap->runAndDelete();
}

// TODO - still seems like too much boilerplate
void TRNannotationTask(TRNjobQueue* pRequestQueue, COLvar* pResult, COLvar Input, COLaddress Address){
   COL_FUNCTION(TRNannotationTask);
   TRNrunTask(Input, pRequestQueue, pResult, Address, &TRNannotation);
}

void TRNannotationUpdateTask(TRNjobQueue* pRequestQueue, COLvar* pResult, COLvar Input, COLaddress Address){
   COL_FUNCTION(TRNannotationUpdateTask);
   TRNrunTask(Input, pRequestQueue, pResult, Address, &TRNannotationBlock);
}

void TRNannotationWindowTask(TRNjobQueue* pRequestQueue, COLvar* pResult, COLvar Input, COLaddress Address){
   COL_FUNCTION(TRNannotationWindowTask);
   TRNrunTask(Input, pRequestQueue, pResult, Address, &TRNargumentExpand);
}

void TRNannotationFunctionListTask(TRNjobQueue* pRequestQueue, COLvar* pResult, COLvar Input, COLaddress Address){
   COL_FUNCTION(TRNannotationFunctionListTask);
   TRNrunTask(Input, pRequestQueue, pResult, Address, &TRNideGlobalFunctionListExpand);
}

void TRNaddJob(COLclosure2<TRNjobQueue*, COLvar*>* pJob, TRNjobQueue* pRequestQueue, SCKloop* pLoop){
   COL_FUNCTION(TRNaddJob);
   pRequestQueue->JobList.push_back(pJob);
   if (!pRequestQueue->IsRunning){
      COL_TRC("No job currently running, so start the job runner again");
      TRNjobRunnerStart(pRequestQueue, pLoop);
   } else {
      COL_TRC("Job already running, cancel the existing one");
      TRNsetJobCancelledFlag(pRequestQueue->pJob);
   }
}

void TRNjobRunnerStart(TRNjobQueue* pRequestQueue, SCKloop* pLoop){
   COL_FUNCTION(TRNjobRunnerStart);
   pRequestQueue->LastActivity = time(0);

   if (pRequestQueue->JobList.size() > 0) {
      COL_TRC("Starting timed loop");
      pLoop->threadPool()->scheduleTask(COLnewClosure0(&TRNrunJob, pRequestQueue->JobList.front(), pRequestQueue, pLoop));
      pRequestQueue->JobList.pop_front();
      pRequestQueue->IsRunning = true;
   }
}

void TRNonJobFinished(SCKloop* pLoop, TRNjobQueue* pRequestQueue){
   COL_FUNCTION(TRNonJobFinished);
   if(pRequestQueue->IsShutdown){
      COL_TRC("Job queue has shutdown - delete");
      delete pRequestQueue;
      return;
   }
   TRNupdateLastActivity(pRequestQueue);
   if(pRequestQueue->JobList.size()){
      COL_TRC("Another job in queue, running it.");
      pLoop->threadPool()->scheduleTask(COLnewClosure0(&TRNrunJob, pRequestQueue->JobList.front(), pRequestQueue, pLoop));
      pRequestQueue->JobList.pop_front();
   } else {
      pRequestQueue->IsRunning = false;
      COL_TRC("No more jobs remaining. This queue will shutdown once it has idled for a bit.");
   }
}

void TRNrunJob(COLclosure2<TRNjobQueue*, COLvar*>* pJob, TRNjobQueue* pRequestQueue, SCKloop* pLoop){
   COL_FUNCTION(TRNrunJob);
   COLvar Result;
   pJob->runAndDelete(pRequestQueue, &Result);
   pLoop->post(COLnewClosure0(&TRNonJobFinished, pLoop, pRequestQueue));
}

void TRNjobRunnerShutdown(TRNjobQueue* pRequestQueue, SCKloop* pLoop){
   COL_FUNCTION(TRNjobRunnerShutdown);
   if (pRequestQueue->IsRunning) {
      COL_TRC("Script still running, update activity timestamp");
      TRNupdateLastActivity(pRequestQueue);
      return;
   }
   if (pRequestQueue->LastActivity + TRN_KEEP_ALIVE_TIME > time(0)){
      COL_TRC("Script not running, but keep alive time has not passed. Keep alive");
      return;
   }
   COL_TRC("Idled too long, sending shutdown request");
   TRNjobRunnerSendShutdownCommand(pRequestQueue, pLoop);
}

void TRNjobRunnerSendShutdownCommand(TRNjobQueue* pRequestQueue, SCKloop* pLoop){
   COL_FUNCTION(TRNjobRunnerSendShutdownCommand);
   COL_TRC("Sending shutdown command");
   if (pLoop->isSet(pRequestQueue->ShutdownTimerId)){
      COL_TRC("Cancelling shutdown timer " << pRequestQueue->ShutdownTimerId);
      pLoop->cancelTimer(pRequestQueue->ShutdownTimerId);
      pRequestQueue->ShutdownTimerId = 0;
   }
   TRNclearJobQueue(pRequestQueue); // After this, the JobList size is 0
   pRequestQueue->IsShutdown = true;
   TRNaddJob(COLnewClosure2(&TRNshutdownTask), pRequestQueue, pLoop);
}

void TRNsetupShutdownTimer(TRNjobQueue* pRequestQueue, SCKloop* pLoop){
   COL_FUNCTION(TRNsetupShutdownTimer);
   COL_TRC("Setting up shutdown timer");
   int ShutdownTimerId = pLoop->addTimer(COLnewClosure0(&TRNjobRunnerShutdown, pRequestQueue, pLoop), TRN_KEEP_ALIVE_TIME * 1000, true); // repeating timer to clean up runner if it idles too long
   COL_VAR(ShutdownTimerId);
   pRequestQueue->ShutdownTimerId = ShutdownTimerId;
}