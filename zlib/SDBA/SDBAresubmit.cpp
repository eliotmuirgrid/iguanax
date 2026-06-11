//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBAresubmit.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-09-03, 11:46a.m.
//  ---------------------------------------------------------------------------
#include <BLUA/BLUAcall.h>
#include <BLUA/BLUAmanager.h>
#include <BLUA/BLUAstart.h>
#include <BLUA/BLUAstop.h>
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLutils.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <LUA/LUAmessage.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
#include <SDB/SDBdateTime.h>
#include <SDB/SDBfile.h>
#include <SDB/SDBfilter2.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBreader.h>
#include <SDBA/SDBAresubmit.h>
#include <SDBU/SDBUlogFile.h>
#include <SDBU/SDBUparams.h>
#include <SDBU/SDBUutils.h>

#include <memory>
COL_LOG_MODULE;

#define SDBA_BULK_RESUB_ERROR_COMP_STOPPED 723681
#define SDBA_BULK_RESUB_ERROR_USER_ABORTED 346158

static SDBAresubmitBulkState SDBA_RESUBMIT_STATE;

void SDBAresubmitStatus::reset() {
   HasError = false;
   Count    = 0;
   Time.clear();
   StartTime.clear();
   StatusMessage.clear();
}

struct SDBAfetchState {
   COLstring            m_User;
   BLUAinstanceManager* m_pManager;
   COLclosure1<int>*    m_pDone;
   SCKloop*             m_pLoop;
   SDBfile              m_CurrentFile;

   SDBAfetchState(const COLstring& User, BLUAinstanceManager* pManager, COLclosure1<int>* pDone, SCKloop* pLoop)
       : m_User(User), m_pManager(pManager), m_pDone(pDone), m_pLoop(pLoop) {}
};

static void SDBAfetchNextFileWorker(std::shared_ptr<SDBAfetchState> pState);

void SDBAsetupResubmitState(const int Limit, const SDBfilterInfo2& Filter, const COLstring& Target) {
   COL_FUNCTION(SDBAsetupResubmitState);
   SDBA_RESUBMIT_STATE.m_Limit   = Limit;
   SDBA_RESUBMIT_STATE.m_Running = true;
   SDBA_RESUBMIT_STATE.m_Target  = Target;
   SDBA_RESUBMIT_STATE.m_Filter  = Filter;
   SDBA_RESUBMIT_STATE.m_Status.reset();
   SDBA_RESUBMIT_STATE.m_Status.StatusMessage = "Resubmitting...";
}

void SDBAresubmitBulkUpdateStateStatus(const unsigned long long DateTime) {
   COL_FUNCTION(SDBAresubmitBulkUpdateStateStatus);
   COL_VAR3(DateTime, SDBA_RESUBMIT_STATE.m_Status.Count, SDBA_RESUBMIT_STATE.m_Status.Time);
   SDBA_RESUBMIT_STATE.m_Status.Count++;
   SDBA_RESUBMIT_STATE.m_Status.Time = COLuint64ToString(DateTime);
   if (SDBA_RESUBMIT_STATE.m_Status.StartTime.is_null()) SDBA_RESUBMIT_STATE.m_Status.StartTime = COLuint64ToString(DateTime);
}

static void SDBAlogCompletionMessage(const COLstring& User, int messageCount) {
   COL_FUNCTION(SDBAlogCompletionMessage);
   COLstring Message;
   COLostream Stream(Message);
   Stream << "Resubmitted " + COLint32ToString(messageCount) + " messages between '"
          << SDBA_RESUBMIT_STATE.m_Filter.m_Start << "' and '" << SDBA_RESUBMIT_STATE.m_Filter.m_End
          << "' as requested by user '" << User << "'.";
   SDBIlogComponent(Message, SDBA_RESUBMIT_STATE.m_Target, "#resubmit");
}

static void SDBAstopTargetComponent(BLUAinstanceManager* pBluaManager) {
   COL_FUNCTION(SDBAstopTargetComponent);
   COL_TRC("Stopping component '" << SDBA_RESUBMIT_STATE.m_Target << "'");
   BLUAinstanceStop(pBluaManager, SDBA_RESUBMIT_STATE.m_Target, false);
}

static void SDBAhandleSuccessfulCompletion(const COLstring& User, BLUAinstanceManager* pBluaManager) {
   COL_FUNCTION(SDBAhandleSuccessfulCompletion);
   COL_TRC("bulk resub completed");
   SDBAlogCompletionMessage(User, SDBA_RESUBMIT_STATE.m_Status.Count);
   SDBAstopTargetComponent(pBluaManager);
   SDBA_RESUBMIT_STATE.m_Status.StatusMessage = "Resubmission completed.";
}

static void SDBAhandleUserAborted(BLUAinstanceManager* pBluaManager) {
   COL_FUNCTION(SDBAhandleUserAborted);
   COL_TRC("bulk resub aborted by user");
   SDBAstopTargetComponent(pBluaManager);
   SDBA_RESUBMIT_STATE.m_Status.StatusMessage = "Resubmission stopped.";
}

static void SDBAhandleComponentStopped() {
   COL_FUNCTION(SDBAhandleComponentStopped);
   COL_TRC("something happened to the target component. abort");
   SDBIlogComponent("Bulk message resubmission aborted because component '" + SDBA_RESUBMIT_STATE.m_Target + "' was stopped.",
                    SDBA_RESUBMIT_STATE.m_Target, "#warning #resubmit");
   SDBA_RESUBMIT_STATE.m_Status.StatusMessage = "Resubmission stopped. The target component is off. Check it is on or has any script errors";
   SDBA_RESUBMIT_STATE.m_Status.HasError = true;
}

static void SDBAhandleReaderError(int Code) {
   COL_FUNCTION(SDBAhandleReaderError);
   COL_TRC("bulk resub aborted due to reader error " << Code);
   SDBA_RESUBMIT_STATE.m_Status.StatusMessage = "Resubmission error. Check logs for details";
   SDBA_RESUBMIT_STATE.m_Status.HasError = true;
   SDBIlogComponent("Bulk message resubmission aborted due to queue reader error " + COLintToString(Code) + ".",
                    SDBA_RESUBMIT_STATE.m_Target, "#error #resubmit");
}

static void SDBAresubmittedAllMessagesInRange(const int Code, COLstring User, BLUAinstanceManager* pBluaManager) {
   COL_FUNCTION(SDBAresubmittedAllMessagesInRange);
   SDBA_RESUBMIT_STATE.m_Running = false;
   switch(Code) {
      case 0:                                  return SDBAhandleSuccessfulCompletion(User, pBluaManager);
      case SDBA_BULK_RESUB_ERROR_USER_ABORTED: return SDBAhandleUserAborted(pBluaManager);
      case SDBA_BULK_RESUB_ERROR_COMP_STOPPED: return SDBAhandleComponentStopped();
      default:                                 return SDBAhandleReaderError(Code);
   }
}

static void SDBAcompletedWithReturnCode(const int Code, COLclosure1<int>* pCallback) {
   COL_FUNCTION(SDBAcompletedWithReturnCode);
   pCallback->runAndDelete(Code);
}

static void SDBAlogMessageResubmission(const SDBmessage* pMessage, const COLstring& TargetComponentId,
                                       const COLstring& User) {
   COL_FUNCTION(SDBAlogMessageResubmission);
   COLstring  Message;
   COLostream Stream(Message);
   Stream << "User '" + User + "' resubmitted message: " << pMessage->Id.MessageId;
   SDBIlogComponent(Message, TargetComponentId, "#resubmit", pMessage->Id.MessageId);
}

static void SDBAresubmitCooldown() {
   const char* pValue = ::getenv("SDBA_BULK_RESUB_COOLDOWN");
   if(pValue == NULL) { return; }
   int Cooldown = ::atoi(pValue);  // ms
   if(Cooldown <= 0) { return; }
   COLsleep(Cooldown);
}

static void SDBAmessageResubmitted(COLvar Result) {}

static bool SDBAonDataMessage(int Code, SDBmessage* pMessage, COLstring User, BLUAinstanceManager* pManager, COLclosure1<int>* pCallback) {
   COL_FUNCTION(SDBAonDataMessage);
   COL_VAR(Code);
   if(Code) {
      if(SDB_END_OF_LOG == Code) {  // reaching End of Log is not an error
         COL_TRC("Reached End of Log. Done");
         Code = 0;
      } else {
         COL_TRC("Fatal Error " << Code);
      }
      SDBAcompletedWithReturnCode(Code, pCallback);
      return true;
   }
   if(!BLUAinstanceRunning(*pManager, SDBA_RESUBMIT_STATE.m_Target)) {
      COL_TRC("Someone stopped the target component. Abort bulk resubmission");
      SDBAcompletedWithReturnCode(SDBA_BULK_RESUB_ERROR_COMP_STOPPED, pCallback);
      return true;
   }
   COL_VAR2(pMessage->Id, SDBA_RESUBMIT_STATE.m_Filter.m_End);
   if(pMessage->Id.MessageId > SDBA_RESUBMIT_STATE.m_Filter.m_End) {
      COL_TRC("Done bulk resubmit");
      SDBAcompletedWithReturnCode(0, pCallback);
      return true;
   }
   // else pMessage->Id <= ToId
   COL_TRC("Check run");
   if(!SDBA_RESUBMIT_STATE.m_Running) {
      COL_TRC("Resubmission job stopped");
      SDBAcompletedWithReturnCode(SDBA_BULK_RESUB_ERROR_USER_ABORTED, pCallback);
      return true;
   }
   COL_VAR(SDBA_RESUBMIT_STATE.m_Status.Count);
   SDBAresubmitBulkUpdateStateStatus(pMessage->Id.MessageId.DateTime);
   COL_TRC("resubmit data message");
   COLstring RelatedId = LUAserializeMessageId(pMessage->Id.MessageId.ComponentId, pMessage->Id.MessageId.DateTime, pMessage->Id.MessageId.Index);
   COLclosure1<COLvar>* pDone = COLnewClosure1(&SDBAmessageResubmitted);
   BLUAmessageIdCall(pManager->m_Instances[SDBA_RESUBMIT_STATE.m_Target], pMessage->Data, RelatedId, pDone);
   // TODO move code below into SDBAmessageResubmitted
   pManager->m_Instances[SDBA_RESUBMIT_STATE.m_Target].get()->m_Config.Status.LastActivity   = time(NULL);
   SDBAlogMessageResubmission(pMessage, SDBA_RESUBMIT_STATE.m_Target, User);
   SDBA_RESUBMIT_STATE.m_Limit--;
   if(SDBA_RESUBMIT_STATE.m_Limit == 0) {
      COL_TRC("Resubmission limit reached -- complete");
      SDBAcompletedWithReturnCode(0, pCallback);
      return true;
   }
   return false;
}

bool SDBAresubmitBulkValidate(const COLstring& User, CFGmap* pMap, const COLstring& Source,
                              const COLstring& Target, const SDBfilterInfo2& Filter, const COLaddress& Address) {
   COL_FUNCTION(SDBAresubmitBulkValidate);
   COLstring Err;
   if(!SDBUcomponentExists(pMap, Target)) {
      COLrespondError(Address, "The specified 'target' doesn't exist");
      return false;
   }
   if(!SDBUcomponentExists(pMap, Source)) {
      COLrespondError(Address, "The specified 'component' doesn't exist.");
      return false;
   }
   if(SDBA_RESUBMIT_STATE.m_Running) {
      COLrespondError(Address, "A bulk resubmission job is currently in progress.");
      return false;
   }
   if(SDBUbadBounds(Filter.m_Start, Filter.m_End, Filter.m_Forward)) {
      COL_TRC("Bad bounds for start and end time.");
      COLrespondError(Address, "Bad bounds for start and end time");
      return false;
   }
   return true;
}

static void SDBAonEndOfLogMain(std::shared_ptr<SDBAfetchState> pState) {
   COL_FUNCTION(SDBAonEndOfLogMain);
   SDBAonDataMessage(SDB_END_OF_LOG, NULL, pState->m_User, pState->m_pManager, pState->m_pDone);
}

static void SDBAprocessFileMessagesOnMain(std::shared_ptr<SDBAfetchState> pState, std::shared_ptr<COLlist<SDBcontent*>> pList) {
   COL_FUNCTION(SDBAprocessFileMessagesOnMain);
   for(const auto& pContent : *pList) {
      auto pMessage = (SDBmessage*)pContent;
      if(SDBAonDataMessage(0, pMessage, pState->m_User, pState->m_pManager, pState->m_pDone)) { return; }
      SDBAresubmitCooldown();
   }

   pState->m_CurrentFile = SDBUfindNextFileToRead(pState->m_CurrentFile, SDBA_RESUBMIT_STATE.m_Filter.m_Start.ComponentId, true);

   if(pState->m_CurrentFile.is_null()) {
      COL_TRC("No more files to process, calling SDB_END_OF_LOG");
      SDBAonDataMessage(SDB_END_OF_LOG, NULL, pState->m_User, pState->m_pManager, pState->m_pDone);
   } else {
      pState->m_pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBAfetchNextFileWorker, pState));
   }
}

static void SDBAfetchNextFileWorker(std::shared_ptr<SDBAfetchState> pState) {
   COL_FUNCTION(SDBAfetchNextFileWorker);
   auto pList = std::make_shared<COLlist<SDBcontent*>>();
   if(SDBUloadFileToList(pState->m_CurrentFile, SDBA_RESUBMIT_STATE.m_Filter, pList.get())) {
      return pState->m_pLoop->post(COLnewClosure0(&SDBAprocessFileMessagesOnMain, pState, pList));
   }
   COL_TRC("Failed to load file. Trying next");
   pState->m_CurrentFile = SDBUfindNextFileToRead(pState->m_CurrentFile, SDBA_RESUBMIT_STATE.m_Filter.m_Start.ComponentId, true);

   if(pState->m_CurrentFile.is_null()) {
      COL_TRC("No more files and current file failed to load - we're done");
      pState->m_pLoop->post(COLnewClosure0(&SDBAonEndOfLogMain, pState));
   } else {
      SDBAfetchNextFileWorker(pState);
   }
}

static void SDBAfetch(const COLstring User, BLUAinstanceManager* pManager, COLclosure1<int>* pDone, SCKloop* pLoop) {
   COL_FUNCTION(SDBAfetch);
   auto pState         = std::make_shared<SDBAfetchState>(User, pManager, pDone, pLoop);
   pState->m_CurrentFile = SDBUfindStartingLogFile(SDBA_RESUBMIT_STATE.m_Filter.m_Start);
   if(!pState->m_CurrentFile.is_null()) {
      return pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBAfetchNextFileWorker, pState));
   }
   COL_TRC("No files found to process");
   SDBAonDataMessage(SDB_END_OF_LOG, NULL, User, pManager, pDone);
}

// bulk_resubmit/start
void SDBAresubmitStart(const COLwebRequest& Request, CFGmap* pMap, BLUAinstanceManager* pManager, SCKloop* pLoop) {
   COL_FUNCTION(SDBAresubmitStart);
   COL_VAR(Request.Data);
   COLstring Component, Err;
   if(!SDBUcomponentId(COLvarGetString(Request.Data, "component", ""), &Component, &Err)){ 
      return COLrespondError(Request.Address, Err); 
   }
   const COLstring Target       = COLvarRequiredString(Request.Data, "target");
   COLvar          RequestData  = Request.Data;
   RequestData["forward"]       = true;
   RequestData["only_messages"] = true;
   RequestData["limit"]         = COLvarGet(Request.Data, "limit", COL_INT_32_MAX);
   const SDBUparams Params(Component, RequestData);

   if(!PRMcheckRoleAccess(false, Request.User, Target, RESUBMIT_MESSAGE, &Err)) { return COLrespondError(Request.Address, PRMsetError(RESUBMIT_MESSAGE)); }

   if(!SDBAresubmitBulkValidate(Request.User, pMap, Params.m_Component, Target, Params.m_LogFilter, Request.Address)) { return; }

   if(BLUAinstanceRunning(*pManager, Target)) {
      return COLrespondError(Request.Address, "Please stop component '" + Target + "' before resubmitting messages.");
   }
   COL_TRC("Starting component '" << Target << "'");
   BLUAinstanceStart(pManager, &(pMap->ConfigMap[Target]), pLoop, true);

   SDBAsetupResubmitState(Params.m_Limit, Params.m_LogFilter, Target);
   COLstring  Message;
   COLostream Stream(Message);
   Stream << "Resubmitting messages between '" << SDBA_RESUBMIT_STATE.m_Filter.m_Start << "' and '"
          << SDBA_RESUBMIT_STATE.m_Filter.m_End << "' as requested by user '" << Request.User << "'.";
   SDBIlogComponent(Message, Target, "#resubmit");

   COLclosure1<int>* pDone = COLnewClosure1(&SDBAresubmittedAllMessagesInRange, Request.User, pManager);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBAfetch, Request.User, pManager, pDone, pLoop));
   COLrespondSuccess(Request.Address);
}

// bulk_resubmit/poll
void SDBAresubmitPoll(const COLwebRequest& Request) {
   COL_FUNCTION(SDBAresubmitPoll);
   COLvar Result;
   Result["done"]   = !SDBA_RESUBMIT_STATE.m_Running;
   Result["count"]  = SDBA_RESUBMIT_STATE.m_Status.Count;
   Result["time"]   = SDBA_RESUBMIT_STATE.m_Status.Time;
   Result["start"]  = SDBA_RESUBMIT_STATE.m_Status.StartTime;
   Result["status"] = SDBA_RESUBMIT_STATE.m_Status.StatusMessage;
   Result["end"]    = COLuint64ToString(SDBA_RESUBMIT_STATE.m_Filter.m_End.DateTime);
   Result["error"]  = SDBA_RESUBMIT_STATE.m_Status.HasError;
   Result["target"] = SDBA_RESUBMIT_STATE.m_Target;
   COLrespondSuccess(Request.Address, Result);
}

// log/resubmit/bulk_stop
void SDBAresubmitStop(const COLwebRequest& Request) {
   COL_FUNCTION(SDBAresubmitStop);
   COLstring Err;
   if(!PRMcheckRoleAccess(false, Request.User, SDBA_RESUBMIT_STATE.m_Target, RESUBMIT_MESSAGE, &Err)) {
      return COLrespondError(Request.Address, PRMsetError(RESUBMIT_MESSAGE));
   }
   if(!SDBA_RESUBMIT_STATE.m_Running) { return COLrespondError(Request.Address, "No running bulk resubmission job"); }
   COL_TRC("Stop requested by: " << Request.User);
   SDBA_RESUBMIT_STATE.m_Running = false;
   COLstring  Message;
   COLostream Stream(Message);
   Stream << "Bulk resubmission of messages between '" << SDBA_RESUBMIT_STATE.m_Filter.m_Start << "' and '"
          << SDBA_RESUBMIT_STATE.m_Filter.m_End << "' was aborted by user '" << Request.User << "'.";
   SDBIlogComponent(Message, SDBA_RESUBMIT_STATE.m_Target, "#resubmit");
   COLrespondSuccess(Request.Address);
}
