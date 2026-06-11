// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBAlogQueuePosition
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday 05 May 2023 - 03:49PM
// ---------------------------------------------------------------------------
#include <SDBU/SDBUutils.h>
#include <BLUA/BLUAmanager.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
#include <SDB/SDBdateTime.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBsystem.h>
#include <SDBA/SDBAlogQueuePosition.h>
COL_LOG_MODULE;


static void SDBAmessageRetrieved(SDBmessage* pMessage, COLstring Error, COLaddress Address, COLstring ConsumerId, COLstring User) {
   COL_FUNCTION(SDBAmessageRetrieved);
   if (pMessage == NULL) {
      COL_VAR(Error);
      Error.prepend("Invalid 'component', 'datetime', and/or 'position' argument: ");
      return COLrespondError(Address, Error);
   }
   if (SDB_MESSAGE != pMessage->Type) {
      return COLrespondError(Address, "Invalid queue position.");
   }
   SDBid Position = pMessage->Id;
   delete pMessage;
   if (!SDBIrepositionQueue(Position, ConsumerId)) {
      return COLrespondError(Address, "Invalid 'component' and/or 'consumer' argument.");
   }
   COLstring Message = "Accepted new queue position requested by user '" + User + "'.\n";
   Message += "New queue position: " + Position.MessageId.ComponentId + "-" + SDBdateMs(Position.MessageId.DateTime) + "-" + COLintToString(Position.MessageId.Index);
   SDBIlogComponent(Message, ConsumerId, "", Position.MessageId);
   COLrespondSuccess(Address, "Repositioning queue for consumer " + ConsumerId);
}

// WORKER
static void SDBAretrieveMessage(COLstring ComponentId, int Index, COLuint64 Datetime, SCKloop* pLoop, COLclosure2<SDBmessage*, COLstring>* pDone) {
   COL_FUNCTION(SDBAretrieveMessage);
   COLstring Err;
   SDBmessage* pMessage = SDBUextractMessage(ComponentId, Index, Datetime, &Err);
   pLoop->post(COLnewClosure0(pDone, &COLclosure2<SDBmessage*, COLstring>::runAndDelete, pMessage, Err));
}

static bool SDBAisComponentRunning(const COLstring& Id, BLUAinstanceManager* pManager){
   COL_FUNCTION(SDBAisComponentRunning);
   return pManager->m_Instances.count(Id) == 1;
}

// /log/queue/position
void SDBAlogQueuePosition(const COLwebRequest& Request, CFGmap* pMap, BLUAinstanceManager* pBluaManager, SDBsystem* pSystem){
   COL_FUNCTION(SDBAlogQueuePosition);
   COL_VAR(Request.Data.json(1));
   const COLvar& Data = Request.Data;
   COLstring ComponentId, Error;
   if(!SDBUcomponentId(COLvarGetString(Data, "component", ""), &ComponentId, &Error)){ 
      return COLrespondError(Request.Address, "Invalid source queue component " + ComponentId); 
   }
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::REPOSITION_QUEUE, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::REPOSITION_QUEUE));
   }
   COLstring Position    = COLvarRequiredString(Data, "position");
   COLstring DateTime    = COLvarRequiredString(Data, "datetime");
   COLstring ConsumerId  = COLvarRequiredString(Data, "consumer");
   if (ConsumerId.is_null() || ConsumerId.isWhitespace()) { 
      return COLrespondError(Request.Address, "Consumer cannot be the Iguana Service logs."); 
   } 
   if(!SDBUcomponentId(ConsumerId, &ConsumerId, &Error)){ // name -> guid lookup
      return COLrespondError(Request.Address, "Failed to reposition queue. This component does not exist anymore. Maybe it was deleted?"); 
   }
   if (SDBAisComponentRunning(ConsumerId, pBluaManager)) {
      return COLrespondError(Request.Address, "Failed to reposition queue. Please stop the component before attempting to reposition the queue.");
   }
   // TODO - is COLvar int big enough?
   COLint64 Index = ::atoll(Position.data());
   COLint64 Time  = ::atoll(DateTime.data());
   COL_VAR(Time);
   if (Index < 1 || Time < 1) {
      return COLrespondError(Request.Address, "Invalid 'position' or 'datetime' argument.");
   }
   // extract the actual message to make sure ['component', 'position', 'datetime'] is valid
   SCKloop* pLoop = pSystem->pLoop;
   COLclosure2<SDBmessage*, COLstring>* pDone = COLnewClosure2(&SDBAmessageRetrieved, Request.Address, ConsumerId, Request.User);
   SDBAretrieveMessage(ComponentId, Index, Time, pLoop, pDone);
}