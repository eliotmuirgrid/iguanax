//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBAresubmitCustom
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
#include <COL/COLclosure6.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
#include <SDBU/SDBUutils.h>
COL_LOG_MODULE;


static void SDBAcheckMessageId(SDBmessageId Id, SCKloop* pLoop, COLclosure0* pSuccess, COLclosure0* pFailure) {
   COL_FUNCTION(SDBAcheckMessageId);
   COLstring Err;
   SDBmessage* pMessage = SDBUextractMessage(Id.ComponentId, Id.Index, Id.DateTime, &Err);
   COL_VAR2(pMessage, Err);
   if (pMessage) {
      pLoop->post(pSuccess);
   } else {
      pLoop->post(pFailure);
   }
}

class SDBAresubmitCustomData {
public:
   CFGconfig*  pTargetConfig;
   SCKloop*    pLoop;
   COLstring   User;
};

static void SDBAmessageResubmitted(COLvar Result, COLstring TargetId, SDBmessageId Id, BLUAinstanceManager* pManager) {
   SDBIlogComponent("Stopping component after message resubmission", TargetId, "#resubmit", Id);
   BLUAinstanceStop(pManager, TargetId, false);
}

static void SDBAgoodComponentId(COLaddress Address, SDBmessageId Id, COLstring* pMessage, BLUAinstanceManager* pManager, SDBAresubmitCustomData Args) {
   COL_FUNCTION(SDBAgoodComponentId);
   const COLstring& TargetId = Args.pTargetConfig->CoreConfig.Guid;
   COL_VAR(TargetId);
   if (BLUAinstanceRunning(*pManager, TargetId)) {
      delete pMessage;
      return COLrespondError(Address, "Please stop component '" + TargetId + "' before resubmitting messages.");
   }
   COLstring  Log;
   COLostream Stream(Log);
   Stream << "User '" << Args.User << "' modified and resubmitted message " << Id;
   SDBIlogComponent(Log, TargetId, "#resubmit", Id);
   SDBIlogComponent(*pMessage, TargetId, "#resubmit", Id);
   COLrespondSuccess(Address);

   SDBIlogComponent("Starting component for message resubmission ...", TargetId, "#resubmit", Id);
   BLUAinstanceStart(pManager, Args.pTargetConfig, Args.pLoop, true);
   COLclosure1<COLvar>* pDone = COLnewClosure1(&SDBAmessageResubmitted, TargetId, Id, pManager);
   BLUAmessageIdCall(pManager->m_Instances[TargetId], *pMessage, Id, pDone);
   delete pMessage;
}

static void SDBAbadComponentId(COLaddress Address, SDBmessageId Id, COLstring* pMessage) {
   COL_FUNCTION(SDBAbadComponentId);
   delete pMessage;
   return COLrespondError(Address, "Unable to find the message with ID '" + (COLstring) Id + ".");
}

// log/resubmit/custom
void SDBAresubmitCustom(const COLwebRequest& Request, CFGmap* pMap, BLUAinstanceManager* pManager, SCKloop* pLoop) {
   COL_FUNCTION(SDBAresubmitCustom);
   const COLvar& Input = Request.Data;
   COLstring TargetId    = COLvarRequiredString(Input, "target_id");
   COLstring ComponentId = COLvarRequiredString(Input, "component_id");
   COLint64  Timestamp   = COLvarRequiredInt   (Input, "time");
   int       Index       = COLvarRequiredInt   (Input, "index");
   if (!SDBUcomponentExists(pMap, ComponentId)) {
      return COLrespondError(Request.Address, "Component '" + ComponentId + "' doesn't exist.");
   }
   if (Timestamp < 0) {    // Timestamp is always stored as COLuint64
      return COLrespondError(Request.Address, "Invalid 'time' parameter.");
   }
   if (Index < 1) {        // sub-ms index always starts at 1
      return COLrespondError(Request.Address, "Invalid 'index' parameter.");
   }
   COLstring Err;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, RESUBMIT_MESSAGE, &Err)) {
      return COLrespondError(Request.Address, PRMsetError(RESUBMIT_MESSAGE));
   }
   if(!SDBUcomponentExists(pMap, TargetId)) {
      return COLrespondError(Request.Address, "The specified 'target_id' doesn't exist");
   }
   if(BLUAinstanceRunning(*pManager, TargetId)) {
      return COLrespondError(Request.Address, "Please stop component '" + TargetId + "' before resubmitting messages.");
   }

   SDBmessageId Id(ComponentId, Timestamp, Index);
   CFGconfig*   pTargetConfig = &(pMap->ConfigMap[TargetId]);
   COLstring*   pMessage = new COLstring(COLvarRequiredString(Input, "message"));
   COLclosure0* pFailure = COLnewClosure0(&SDBAbadComponentId,  Request.Address, Id, pMessage);
   SDBAresubmitCustomData Args { pTargetConfig, pLoop, Request.User };
   COLclosure0* pSuccess = COLnewClosure0(&SDBAgoodComponentId, Request.Address, Id, pMessage, pManager, Args);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBAcheckMessageId, Id, pLoop, pSuccess, pFailure));
}