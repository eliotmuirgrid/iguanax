//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAcore
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Friday May 14 2021
//---------------------------------------------------------------------------
#include "BLUAcore.h"

#include <BLUA/BLUAidleConnection.h>
#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAkill.h>
#include <BLUA/BLUAqueue.h>
#include <BLUA/BLUArun.h>
#include <BLUA/BLUAsocket.h>
#include <BLUA/BLUAstart.h>
#include <BLUA/BLUAstop.h>
#include <BLUA/BLUAtask.h>
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigApi.h>
#include <CMPC/CMPCrunDir.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <LUA/LUAtimeout.h>
#include <LUA/LUAutils.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
COL_LOG_MODULE;

static void BLUAtruncateErrorMessage(const COLstring& ComponentID, COLstring* pError) {
   COL_FUNCTION(BLUAtruncateErrorMessage);
   if (pError->find(".lua:") == npos) {
      return;
   }
   COLstring Error = pError->c_str();
   COLstring TempErrorStr, TempWorkingDir;
   Error.split(TempErrorStr, TempWorkingDir, ComponentID.c_str());
   if (TempWorkingDir != "")  { *pError = TempWorkingDir.substr(1);      }
   else                       { COL_TRC("Unable to truncate error message"); }
}

static void BLUAhandleForceKill(const COLstring& Guid, BLUAinstance2* pInstance){
   COL_FUNCTION(BLUAhandleForceKill);
   COL_TRC("Task force killed. Stopping component...");
   SDBIlogComponent("Stopping component due to force kill", Guid, "#error", pInstance->m_DequeuePosition.MessageId);
   BLUAinstanceScheduleAllStop(pInstance);
}

static bool BLUAinstanceHandleJobError(COLstring Error, int Code, BLUAinstance2* pInstance) {
   COL_FUNCTION(BLUAinstanceHandleJobError);
   COLstring Guid  = pInstance->m_Config.CoreConfig.Guid;
   COL_VAR3(Guid, BLUAtaskResultAsString(Code), Error);
   BLUAtruncateErrorMessage(Guid, &Error);
   // Force kill takes precedence over StopOnError=false
   // Depending on how we get here, code may be 2 instead of BLUA_FORCE_KILLED so check both code and error string
   if (Code == BLUA_FORCE_KILLED || Code == 2 && Error == "Component was force killed"){
      BLUAhandleForceKill(Guid, pInstance);
      return true;
   }
   COL_VAR2(pInstance->m_StopOnError, pInstance->m_ResubmissionMode);
   pInstance->m_Config.Status.Light = CFG_LIGHT::ERR;
   pInstance->m_Config.Status.ErrorCount++;
   if (!pInstance->m_ResubmissionMode && (pInstance->m_StopOnError || Code == BLUA_ERROR_COMPILE || Code == BLUA_ERROR_INTERNAL)) {
      COL_TRC("Task completed with error. Stopping component...");
      COLstring Prefix;
      switch (Code) {
         case BLUA_ERROR_COMPILE  : Prefix = "Stopping component with compilation error:\n"; break;
         case BLUA_ERROR_INTERNAL : Prefix = "Stopping component with internal error:\n";    break;
         default:                   Prefix = "Stopping component with runtime error:\n";     break;
      }
      SDBIlogComponent(Prefix + Error, Guid, "#error", pInstance->m_DequeuePosition.MessageId);
      BLUAinstanceScheduleAllStop(pInstance);
      // pInstance->m_JobQueue.put(BLUAkillTask()); // stop on error
      return true;
   } else {
      COL_TRC("Task completed with error, but component configured to keep running.");
      SDBIlogComponent("Continuing to run with error:\n" + Error, Guid, "#error", pInstance->m_DequeuePosition.MessageId);
   }
   return false;
}

static void BLUAinstanceJobCompleted(COLstring Error, int Code, BLUAtaskId TaskId, BLUAinstance2* pInstance, BLUAinstanceManager* pManager, int WorkerId) {
   COL_FUNCTION(BLUAinstanceJobCompleted);
   COL_TRC("JobCompleted Task = " << TaskId);
   if (!Error.is_null()) {
      if (BLUAinstanceHandleJobError(Error, Code, pInstance)) {
         COL_TRC("Error: " << Error);
         return;
      }
   }
   switch (TaskId) {
      case BLUA_ON_DEQUEUE:  BLUAinstanceDequeued(pInstance);                          break;
      case BLUA_RUN:         BLUAinstanceStarted(pInstance, WorkerId);                 break;
      case BLUA_KILL:        BLUAinstanceStopped(pInstance, pManager);                 break;
      case BLUA_ON_ACCEPT:
      case BLUA_ON_CONNECT:
      case BLUA_ON_WRITE:
      case BLUA_ON_CLOSE:
      case BLUA_ON_DATA:     BLUAinstancePostSocketEvent(pInstance, TaskId);           break;
      default:                                                                         break;
   }
}

int BLUAprintThreadId(lua_State* L){
   lua_getfield(L, LUA_REGISTRYINDEX, "BLUAthreadId");
   int BLUAthreadId = lua_tonumber(L, -1);
   lua_pop(L, 1);
   COL_TRC("This was handled by thread: " << BLUAthreadId);
   return BLUAthreadId;
}

static void BLUAinstanceWorkerCompile(lua_State*& L, BLUAinstance2* pInstance, BLUAinstanceManager* pManager, int WorkerId){
   COL_FUNCTION(BLUAinstanceWorkerCompile);
   int CompileResult = 0;
   COLstring CompileError;
   COLvar Payload;
   Payload["worker"] = WorkerId;
   BLUAhandleRun(L, Payload, &CompileResult, pInstance); // got rid of run task and just manually call to prevent workers from taking the wrong run task
   if (CompileResult) {
      CompileError = LUAtoString(L, -1);
   } else {
      lua_pushinteger(L, WorkerId);
      lua_setfield(L, LUA_REGISTRYINDEX, "BLUAthreadId");
   }
   pInstance->m_pLoop->post(COLnewClosure0(&BLUAinstanceJobCompleted, CompileError, CompileResult, BLUA_RUN, pInstance, pManager, WorkerId));
}

// IDEA for the future - we could avoid having a persistent thread - if there is no work pending on a Lua instance why not end the worker thread and
// create another one later.
static void BLUAinstanceWorker(BLUAinstance2* pInstance, BLUAinstanceManager* pManager, int WorkerId) {
   COL_FUNCTION(BLUAinstanceWorker);
   lua_State* L = NULL;
   bool Running = true;
   BLUAinstanceWorkerCompile(L, pInstance, pManager, WorkerId);
   while (Running) {
      // grab a task from task queue
      BLUAtask T = pInstance->m_JobQueue.take();
      BLUAtaskId TaskId = T.Id;
      COL_TRC("Processing task " << TaskId);
      COLstring Error;
      int Result = BLUA_UNKNOWN;
      if (L) { LUAbeginScript(L); }
      BLUAprintThreadId(L);
      // Result is coming from lua_pcall and we are also using it for a special majik number which is BLUA_TERMINATE
      pInstance->m_TaskMap[TaskId]->run(L, T.Payload, &Result);
      if (Result == BLUA_TERMINATE) {
         COL_TRC("Lua instance terminated");
         COLASSERT(TaskId == BLUA_KILL);
         COLASSERT(L == NULL);
         Running = false;
      } else if (Result) {
         COL_TRC("Lua script encountered a runtime error");
         Error = LUAtoStringIncludeMeta(L, -1); // IX-4205 LUAtoString is not enough as errors from NODLdbExecute are on stack in table form. Need to call __tostring.
      }
      COL_TRC("Finished running task, posting back to main...");
      pInstance->m_pLoop->post(COLnewClosure0(&BLUAinstanceJobCompleted, Error, Result, TaskId, pInstance, pManager, WorkerId));
   }
}

void BLUAinstanceSpawn(SCKloop* pLoop, BLUAinstanceManager* pManager, BLUAinstance2* pInstance, int Count){
   for (int i = 1; i <= Count; i++){
      pLoop->threadPool()->scheduleTask(COLnewClosure0(&BLUAinstanceWorker, pInstance, pManager, i));
   }
}

// MAIN THREAD
static void BLUAlogRunDirPrepError(COLstring ComponentId, COLstring Error, BLUAinstance2* pInstance) {
   SDBIlogComponent("Stopping component with internal error:\n" + Error, ComponentId, "#error");
   pInstance->m_Config.Status.Light = CFG_LIGHT::ERR;
   pInstance->m_Config.Status.ErrorCount++;
   BLUAinstanceScheduleAllStop(pInstance);
}

// WORKER THREAD
static void BLUAprepareRunDir(BLUAinstance2* pInstance, SCKloop* pLoop, BLUAinstanceManager* pManager) {
   COL_FUNCTION(BLUAprepareRunDir);
   COLstring Error;
   const CFGcore&   Configs     = pInstance->m_Config.CoreConfig;
   const COLstring& ComponentId = Configs.Guid;
   const COLstring& CheckDir    = CMPCgetRunDir(ComponentId, Configs.RunComp);

   if (!FILfileExists(CheckDir)) { 
      // IX-4236 this should be taken care of when component/set_commit is called, or during component creation in CMPCsetupCompDir
      // If for some reason the directory does not exist, try to create it
      COL_TRC("Run dir not found, create it.");
      if (!CMPCrunDir(ComponentId, Configs.RunComp, Configs.Commit, &Error)) { 
         COL_ERR("Failed to set up run dir. Schedule STOP task immediately.");
         pLoop->post(COLnewClosure0(&BLUAlogRunDirPrepError, ComponentId, Error, pInstance));
      }
   }
   COL_TRC("Run dir exists good to start BLUAinstance");
   BLUAinstanceWorker(pInstance, pManager, 0);
}

BLUAinstance2* BLUAinstanceCreate(CFGconfig& Config, SCKloop* pLoop, BLUAinstanceManager* pManager, bool Resubmission) {
   COL_FUNCTION(BLUAinstanceCreate);
   BLUAinstance2* pInstance = new BLUAinstance2(Config, pLoop);
   pInstance->m_ResubmissionMode = Resubmission;
   BLUAinstanceSetUpDequeue(pInstance);
   BLUAstartIdleConnectionPurgeTimer(pInstance);

   bool IsDev = CFGdevelopmentMode(Config.CoreConfig.Commit, false);
   if (IsDev) {
      COL_TRC("start lua thread");
      pLoop->threadPool()->scheduleTask(COLnewClosure0(&BLUAinstanceWorker, pInstance, pManager, 0));
   } else {
      COL_TRC("prep run dir first");
      pLoop->threadPool()->scheduleTask(COLnewClosure0(&BLUAprepareRunDir, pInstance, pLoop, pManager));
   }
   return pInstance;
}
