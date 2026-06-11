//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAstop
//
// Description:
//
// Implementation
//
// Author: Vismay Shah - refactored by Eliot
// Date:   Tuesday Aug 16 2022
//---------------------------------------------------------------------------
#include "BLUAstop.h"

#include <BLUA/BLUAidleConnection.h>
#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAkill.h>
#include <BLUA/BLUAmanager.h>
#include <BLUA/BLUAqueue.h>
#include <BLUA/BLUAsleep.h>
#include <BLUA/BLUAsocket.h>
#include <BLUA/BLUAtimer.h>
#include <BLUA/BLUAutils.h>
#include <CFG/CFGconfig.h>
#include <CFG/CFGmapApi.h>
#include <CFG/CFGmap.h>
#include <GUID/GUIDid.h>
#include <INFO/INFOutils.h>
#include <SDB/SDBcapi.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
#include <USR/USRuser.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

struct BLUAbatchStopState {
   BLUAinstanceManager* pManager;
   CFGmap*              pMap;
   SCKloop*             pLoop;
   COLarray<COLstring>  ComponentGuids;
   COLclosure0*         pDone;
   int                  Index;
   int                  BatchSize;
   int                  DelayMs;
   int                  TimerId;
   bool                 Force=false;
   // Extra fields for polling
   COLstring            JobId;
   COLstring            Error;
   bool                 Tracked=false;
   bool                 Running=false;
};

static COLhashmap<COLstring, BLUAbatchStopState> BLUAstopJobs;

static void BLUAstopBatch(BLUAbatchStopState* pState, int BatchEnd){
   COL_FUNCTION(BLUAstopBatch);
   for(int i = pState->Index; i < BatchEnd; i++) {
      const COLstring& Guid    = pState->ComponentGuids[i];

      if(pState->pManager->m_Instances.count(Guid) != 1) {
         COL_TRC("Component " << Guid << " already stopped, skipping.");
         continue;
      }

      COL_TRC("Batch stop component " << Guid);
      try {
         BLUAinstanceStop(pState->pManager, Guid, pState->Force);
      } catch(const COLerror& Error) {
         COL_ERR("Failed to stop component " << Guid << ": " << Error.description());
         SDBIlogComponent("Failed to stop component: " + Error.description(), Guid, "#error");
      }
   }
}

static void BLUAstopNextBatch(BLUAbatchStopState* pState) {
   COL_FUNCTION(BLUAstopNextBatch);
   if(pState->Index >= pState->ComponentGuids.size()) {
      COL_TRC("Done stopping batch of components.");
      pState->Running = false;
      if (!pState->Tracked) { BLUAstopJobs.removeKey(pState->JobId); }
      return;
   }
   if (INFOapproachingFdLimit(pState->BatchSize + 10)){
      COLstring ErrMessage = "Failed to stop some components due to insufficient available file descriptors. ";
      COL_ERR(ErrMessage);
      SDBlogService(ErrMessage + BLUAfdError(), "#error");
      pState->Running = false;
      pState->Error   = ErrMessage;
      if (!pState->Tracked) { BLUAstopJobs.removeKey(pState->JobId); } // If this stop operation wasn't being tracked don't worry about keeping the error around.
      return;
   }
   int BatchEnd = pState->Index + pState->BatchSize;
   if(BatchEnd > pState->ComponentGuids.size()) { BatchEnd = pState->ComponentGuids.size(); }

   BLUAstopBatch(pState, BatchEnd);
   pState->Index   = BatchEnd;
   pState->TimerId = pState->pLoop->addTimer(COLnewClosure0(&BLUAstopNextBatch, pState), pState->DelayMs);
}

COLstring BLUAstopComponentList(const COLarray<COLstring>& ComponentGuids, bool Force, bool Tracked, BLUAinstanceManager* pManager, CFGmap* pMap,
                                SCKloop* pLoop, int BatchSize, int DelayMs){
   COL_FUNCTION(BLUAstopComponentList);
   if(ComponentGuids.size() == 0) {
      COL_TRC("No components to stop.");
      return "";
   }
   COLstring JobId = GUIDnewShortId();
   while (BLUAstopJobs.count(JobId)) { // Chance of collision is very low, but just in case
      JobId = GUIDnewShortId();
   }
   BLUAbatchStopState& StopState = BLUAstopJobs[JobId];
   StopState.pManager       = pManager;
   StopState.pMap           = pMap;
   StopState.pLoop          = pLoop;
   StopState.ComponentGuids = ComponentGuids;
   StopState.Index          = 0;
   StopState.BatchSize      = BatchSize;
   StopState.DelayMs        = DelayMs;
   StopState.TimerId        = 0;
   StopState.Running        = true;
   StopState.Error          = "";
   StopState.Force          = false;

   BLUAstopNextBatch(&StopState);
   return JobId;
}

static void BLUAinstanceTearDown(BLUAinstance2* pInstance, BLUAinstanceManager* pManager){
   COL_FUNCTION(BLUAinstanceTearDown);
   BLUAinstanceTearDownDequeue(pInstance);
   // BLUAinstanceStopDequeue(pInstance);
   // New timers could be set up right before tear down. Clear them again. See IX-3127
   // Probably need to do the same for sleep and socket.
   BLUAinstanceClearScriptTimers(pInstance);
   BLUAinstanceTearDownSleep  (pInstance);
   BLUAinstanceTearDownSocket (pInstance);
   BLUAcancelIdleConnectionPurgeTimer(pInstance);
   pManager->m_Instances.removeKey(pInstance->m_Config.CoreConfig.Guid);
   if (pManager->m_ShutdownRequested && pManager->m_Instances.empty()) {
      COL_TRC("In shutdown so calling callback.");  // TODO kind of ugly.
      pManager->m_pOnShutdown->run();
   }
}

void BLUAinstanceScheduleAllStop(BLUAinstance2* pInstance){
   COL_FUNCTION(BLUAinstanceScheduleAllStop);
   COL_VAR(pInstance->m_CloneCount);
   if (!pInstance->m_CloneCount) { pInstance->m_JobQueue.put(BLUAkillTask()); }
   for (int i = 0; i < pInstance->m_CloneCount; i++){
      pInstance->m_JobQueue.put(BLUAkillTask());
   }
}

void BLUAinstanceStopped(BLUAinstance2* pInstance, BLUAinstanceManager* pManager) {
   COL_FUNCTION(BLUAinstanceStopped);
   pInstance->m_CloneCount--;
   if (pInstance->m_CloneCount > 0){
      return;
   }
   CFGstatus& Status = pInstance->m_Config.Status;
   Status.StartTime = 0;
   Status.Border.clear();
   Status.HtmlBlock.clear();
   Status.HtmlTooltipBlock.clear();
   if (Status.Light != CFG_LIGHT::ERR){
      Status.Light = CFG_LIGHT::OFF;
      SDBIlogComponent("Component stopped", pInstance->m_Config.CoreConfig.Guid);
   } else {
      COL_TRC("Not changing the configuration light since it's in error state.");
      // SDBIlogComponent("Component stopped with error", pInstance->m_Config.CoreConfig.Guid, "#error");
   }
   BLUAinstanceTearDown(pInstance, pManager);
}

void BLUAinstanceDelete(BLUAinstance2* pInstance, bool Force) {
   COL_FUNCTION(BLUAinstanceDelete);
   if (Force) {
      COL_TRC("Signal all LUAawait() to abort");
      pInstance->m_ForceKill = true;
   }
   bool RetryDequeue = pInstance->m_pDequeueRetryTimer;
   COL_TRC("clear dequeue callbacks.");
   BLUAinstanceStopDequeue(pInstance);
   COL_TRC("Shutting down timer.");
   BLUAinstanceClearScriptTimers(pInstance);
   COL_TRC("Shutting down sleep.");
   BLUAinstanceTearDownSleep (pInstance);
   COL_TRC("Shutting down socket.");
   BLUAinstanceTearDownSocket(pInstance);
   if (pInstance->m_JobQueue.size() > 0) {
      COL_TRC("Shutdown requested. " << pInstance->m_JobQueue.size() << " task(s) in queue.");
   }
   // Note the dequeue retry timer is already cancelled by BLUAinstanceStopDequeue() above,
   // we need to schedule the Kill task now.
   if (pInstance->m_Dequeueing && !RetryDequeue) {
      COL_TRC("Schedule shutdown task after the current dequeue operation is completed.");
      pInstance->m_pShutdown = COLnewClosure0(&pInstance->m_JobQueue, &COLthreadQueue<BLUAtask>::put, BLUAkillTask());
   } else {
      COL_TRC("Schedule shutdown task now");
      BLUAinstanceScheduleAllStop(pInstance);
      // pInstance->m_JobQueue.put(BLUAkillTask());
   }
}

void BLUAinstanceStop(BLUAinstanceManager* pManager, const COLstring& ComponentId, bool Force) {
   COL_FUNCTION(BLUAinstanceStop);
   COL_VAR(ComponentId);
   if (pManager->m_Instances.count(ComponentId) == 0) {
      COL_TRC("Component stopped already.");
      throw COLerror("Component was not running.");  // THROW ugh
   }
   BLUAinstance2* pInstance = pManager->m_Instances[ComponentId];
   pInstance->m_Config.Status.Light = CFG_LIGHT::IN_PROGRESS;
   pInstance->m_IsStopping = true;
   BLUAinstanceDelete(pInstance, Force);
}

// /component/stop
// This api works like /component/start, with its associated polling api /component/stop/check. See comment in start for details
void BLUAcomponentStop(const COLwebRequest& Request, BLUAinstanceManager* pManager, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(BLUAcomponentStop);
   const COLvar& Input = Request.Data;
   COLvar ComponentIds = COLvarRequiredStringOrStringList(Input, "components");
   bool   Tracked      = COLvarGetBool                   (Input, "tracked", false);
   bool   Force        = COLvarGetBool                   (Input, "force", false);
   COLvar Result;
   Result["switched"].setArrayType();
   Result["skipped"].setArrayType();
   Result["errors"].setArrayType();

   COLarray<COLstring> GoodComponents;
   COLarray<COLstring> BadComponents;
   CFGconvertNameToIdIfNeeded(ComponentIds, pMap, &GoodComponents, &BadComponents);
   for (const auto& BadComponent : BadComponents) {
      Result["skipped"].push_back(BadComponent);
      Result["errors"].push_back("No component exists with name or id " + BadComponent);
   }

   COLstring Error;
   COLarray<COLstring> ComponentGuids;
   for (int i = 0; i < GoodComponents.size(); i++){
      const COLstring Guid = GoodComponents[i];
      CFGconfig* pTransNode = CFGmapNode(pMap, Guid);
      if (!pTransNode){
         Result["skipped"].push_back(Guid);
         Result["errors"].push_back("No component exists with name or id " + Guid);
         continue;
      }
      if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pTransNode->CoreConfig.Guid, ROLid::STOP, &Error)){
         Result["skipped"].push_back(Guid);
         Result["errors"].push_back(PRMsetError(ROLid::STOP));
         continue;
      }

      if(pManager->m_Instances.count(Guid) != 1) {
         Result["skipped"].push_back(Guid);
         Result["errors"].push_back("Component " + pTransNode->CoreConfig.Name + " is not running.");
         continue;
      }

      ComponentGuids.push_back(Guid);
      Result["switched"].push_back(Guid);
   }
   COL_VAR(Result);
   if(ComponentGuids.size() > 0) {
      COLstring JobId = BLUAstopComponentList(ComponentGuids, Force, Tracked, pManager, pMap, pLoop, 20, 1000);
      if (!JobId.is_null()) Result["job_id"] = JobId;
   }
   COLrespondSuccess(Request.Address, Result);
}

// /component/stop/check
void BLUAcomponentStopCheck(const COLwebRequest& Request){
   COL_FUNCTION(BLUAcomponentStopCheck);
   COLstring JobId = COLvarRequiredString(Request.Data, "job_id");
   COLvar Result;
   if (!BLUAstopJobs.count(JobId)) { return COLrespondError(Request.Address, "No component stop job found."); }
   const BLUAbatchStopState& Job = BLUAstopJobs[JobId];
   Result["running"] = Job.Running;
   if (Result["running"].asBool()){
      COL_DBG("Component stop request in progress");
      return COLrespondSuccess(Request.Address, Result);
   }
   // Component stop is either done or has run into error
   if (Job.Error.is_null()){
      COL_TRC("Component stop completed successfully");
      COLrespondSuccess(Request.Address, Result);
   } else {
      COL_TRC("Component stop encountered an error: " << Job.Error);
      COLrespondError(Request.Address, Job.Error + "See Iguana Service logs for more details.");
   }
   // Last status update sent, clear job id from map
   BLUAstopJobs.removeKey(JobId);
}
