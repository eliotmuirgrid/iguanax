//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAstart
//
// Description:
//
// Implementation
//
// Author: Vismay Shah - refactored by Eliot
// Date:   Tuesday Aug 16 2022
//---------------------------------------------------------------------------

#include <BLUA/BLUAcall.h>
#include <BLUA/BLUAcore.h>
#include <BLUA/BLUAmanager.h>
#include <BLUA/BLUAqueue.h>
#include <BLUA/BLUAstart.h>
#include <BLUA/BLUAutils.h>
#include <CFG/CFGconfig.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DRX/DRXutils.h>
#include <GUID/GUIDid.h>
#include <INFO/INFOutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
COL_LOG_MODULE;

struct BLUAbatchStartState {
   BLUAinstanceManager* pManager;
   CFGmap*              pMap;
   SCKloop*             pLoop;
   COLarray<COLstring>  ComponentGuids;
   COLclosure0*         pDone;
   int                  Index;
   int                  BatchSize;
   int                  DelayMs;
   int                  TimerId;
   // Extra fields for polling
   COLstring            JobId;
   COLstring            Error;
   bool                 Tracked=false;
   bool                 Running=false;
};

static COLhashmap<COLstring, BLUAbatchStartState> BLUAstartJobs;

static void BLUAstartBatch(BLUAbatchStartState* pState, int BatchEnd){
   COL_FUNCTION(BLUAstartBatch);
   for(int i = pState->Index; i < BatchEnd; i++) {
      const COLstring& Guid    = pState->ComponentGuids[i];
      CFGconfig*       pConfig = &pState->pMap->ConfigMap[Guid];

      if(pState->pManager->m_Instances.count(Guid) == 1) {
         COL_TRC("Component " << Guid << " already running, skipping.");
         continue;
      }

      COL_TRC("Batch start component " << Guid);
      try {
         BLUAinstanceStart(pState->pManager, pConfig, pState->pLoop);
      } catch(const COLerror& Error) {
         COL_ERR("Failed to start component " << Guid << ": " << Error.description());
         SDBIlogComponent("Failed to start component: " + Error.description(), Guid, "#error #startup");
      }
   }
}

static void BLUAstartNextBatch(BLUAbatchStartState* pState) {
   COL_FUNCTION(BLUAstartNextBatch);
   if(pState->Index >= pState->ComponentGuids.size()) {
      COL_TRC("Done starting batch of components.");
      if(pState->pDone) { pState->pDone->runAndDelete(); }
      pState->Running = false;
      if (!pState->Tracked) { BLUAstartJobs.removeKey(pState->JobId); }
      return;
   }
   if (INFOapproachingFdLimit(pState->BatchSize + 10)){
      COLstring ErrMessage = "Failed to start some components due to insufficient available file descriptors. ";
      COL_ERR(ErrMessage);
      SDBlogService(ErrMessage + BLUAfdError(), "#error");
      pState->Running = false;
      pState->Error   = ErrMessage;
      if (!pState->Tracked) { BLUAstartJobs.removeKey(pState->JobId); } // If this start operation wasn't being tracked don't worry about keeping the error around.
      return;
   }
   int BatchEnd = pState->Index + pState->BatchSize;
   if(BatchEnd > pState->ComponentGuids.size()) { BatchEnd = pState->ComponentGuids.size(); }

   BLUAstartBatch(pState, BatchEnd);
   pState->Index   = BatchEnd;
   pState->TimerId = pState->pLoop->addTimer(COLnewClosure0(&BLUAstartNextBatch, pState), pState->DelayMs);
}

COLstring BLUAstartComponentList(const COLarray<COLstring>& ComponentGuids, bool Tracked, BLUAinstanceManager* pManager, CFGmap* pMap,
                            SCKloop* pLoop, COLclosure0* pDone, int BatchSize, int DelayMs) {
   COL_FUNCTION(BLUAstartComponentList);
   if(ComponentGuids.size() == 0) {
      COL_TRC("No components to start.");
      if(pDone) { pDone->runAndDelete(); }
      return "";
   }
   COLstring JobId = GUIDnewShortId();
   while (BLUAstartJobs.count(JobId)) { // Chance of collision is very low, but just in case
      JobId = GUIDnewShortId();
   }
   BLUAbatchStartState& StartState = BLUAstartJobs[JobId];
   StartState.pManager       = pManager;
   StartState.pMap           = pMap;
   StartState.pLoop          = pLoop;
   StartState.ComponentGuids = ComponentGuids;
   StartState.pDone          = pDone;
   StartState.Index          = 0;
   StartState.BatchSize      = BatchSize;
   StartState.DelayMs        = DelayMs;
   StartState.TimerId        = 0;
   // For tracking start
   StartState.JobId          = JobId;
   StartState.Tracked        = Tracked;
   StartState.Running        = true;

   BLUAstartNextBatch(&StartState);
   return JobId;
}

void BLUAinstanceStarted(BLUAinstance2* pInstance, int WorkerId) {
   COL_FUNCTION(BLUAinstanceStarted);
   if (WorkerId != 0) { return; } // We only need to run all of this once, when the main lua script initially dequeues a message.
   CFGconfig* pConfig = &pInstance->m_Config;
   COL_TRC("Component successfully started!");
   COLstring StartMsg;
   pConfig->Status.Light     = CFG_LIGHT::ON;
   pConfig->Status.StartTime = time(NULL); //component is currently running

   SDBIlogComponent("Component started successfully.", pConfig->CoreConfig.Guid);
   // Enable dequeue if we have source components pushing messages
   if (pConfig->CoreConfig.Sources.size() > 0) {   // this is a destination component
      // Message resubmission gets the data from the caller, not the queue.
      // Don't start the dequeue cycle and shut it down immediately to cause memory leaks.
      if (!pInstance->m_ResubmissionMode) {
         BLUAinstanceDequeueMessage(pInstance);
      }
   } else {    // this is a source component
      COL_TRC("Will call main with \"INIT\"");
      SDBIlogComponent("Calling main function with INIT.", pConfig->CoreConfig.Guid);
      pInstance->m_JobQueue.put(BLUAcallTask("main", "INIT"));
   }
}

// On MAIN Thread
void BLUAinstanceStart(BLUAinstanceManager* pManager, CFGconfig* pConfig, SCKloop* pLoop, bool Resubmission) {
   COL_FUNCTION(BLUAinstanceStart);
   const COLstring& ComponentId = pConfig->CoreConfig.Guid;
   COL_VAR(ComponentId);
   if (pManager->m_Instances.count(ComponentId) == 1) {
      COL_VAR2(ComponentId, pManager->m_Instances.count(ComponentId));
      COL_TRC("Component running already.");
      throw COLerror("Component " + pConfig->CoreConfig.Name + " is already running.");
   }
   SDBIlogComponent("Starting component ...", ComponentId);
   BLUAinstance2* pInstance = BLUAinstanceCreate(*pConfig, pLoop, pManager, Resubmission);
   pManager->m_Instances[ComponentId] = pInstance;
   pConfig->Status.Light = CFG_LIGHT::IN_PROGRESS;
}

// /component/start
// This api asynchronously starts one or more components. Start jobs are placed in the static hashmap in this file
// while they are running. The status of a start job can be polled by the /component/start/check api.
// If the request has the "tracked" argument set to true, the job will stay in the map once finished until a
// call to /component/start/check gets its status and clears it from the map. Otherwise, the job will clear itself
// on completion or error.
void BLUAcomponentStart(const COLwebRequest& Request, BLUAinstanceManager* pManager, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(BLUAcomponentStart);
   bool   Tracked      = COLvarGetBool(Request.Data, "tracked", false);
   COLvar ComponentIds = COLvarRequiredStringOrStringList(Request.Data, "components");
   COLvar Result;
   Result["switched"].setArrayType();
   Result["skipped"].setArrayType();
   Result["errors"].setArrayType();
   COLstring Reason;
   if(!DRXcomponentCountCheck(pManager->m_Instances.size() + ComponentIds.size(), &Reason)){
      COLstring Error = "Could not start component";
      if (ComponentIds.size() > 1) { Error.append("s"); }
      Error.append(". " + Reason);
      return COLrespondError(Request.Address, Error);
   }

   COLarray<COLstring> GoodComponents;
   COLarray<COLstring> BadComponents;
   CFGconvertNameToIdIfNeeded(ComponentIds, pMap, &GoodComponents, &BadComponents);
   for (const auto& BadComponent : BadComponents) {
      Result["skipped"].push_back(BadComponent);
      Result["errors"].push_back("No component exists with name or id " + BadComponent);
   }

   COLarray<COLstring> ComponentGuids;
   for(int i = 0; i < GoodComponents.size(); i++) {
      const COLstring  Guid       = GoodComponents[i];
      const CFGconfig* pTransNode = CFGmapNode(pMap, Guid);
      if(!pTransNode) {
         Result["skipped"].push_back(Guid);
         Result["errors"].push_back("No component exists with name or id " + Guid);
         continue;
      }
      COLstring Error;
      if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, Guid, START, &Error)) {
         Result["skipped"].push_back(Guid);
         Result["errors"].push_back(PRMsetError(START));
         continue;
      }

      if(pManager->m_Instances.count(Guid) == 1) {
         Result["skipped"].push_back(Guid);
         Result["errors"].push_back("Component " + pTransNode->CoreConfig.Name + " is already running.");
         continue;
      }

      ComponentGuids.push_back(Guid);
      Result["switched"].push_back(Guid);
   }

   if(ComponentGuids.size() > 0) {
      COLstring JobId = BLUAstartComponentList(ComponentGuids, Tracked, pManager, pMap, pLoop, NULL, 20, 1000);
      if (!JobId.is_null()) Result["job_id"] = JobId;
   }
   COLrespondSuccess(Request.Address, Result);
}

// /component/start/check
void BLUAcomponentStartCheck(const COLwebRequest& Request){
   COL_FUNCTION(BLUAcomponentStartCheck);
   COLstring JobId = COLvarRequiredString(Request.Data, "job_id");
   COLvar Result;
   if (!BLUAstartJobs.count(JobId)) { return COLrespondError(Request.Address, "No component start job found."); }
   const BLUAbatchStartState& Job = BLUAstartJobs[JobId];
   Result["running"] = Job.Running;
   if (Result["running"].asBool()){
      COL_DBG("Component start request in progress");
      return COLrespondSuccess(Request.Address, Result);
   }
   // Component start is either done or has run into error
   if (Job.Error.is_null()){
      COL_TRC("Component start completed successfully");
      COLrespondSuccess(Request.Address, Result);
   } else {
      COL_TRC("Component start encountered an error: " << Job.Error);
      COLrespondError(Request.Address, Job.Error + "See Iguana Service logs for more details.");
   }
   // Last status update sent, clear job id from map
   BLUAstartJobs.removeKey(JobId);
}