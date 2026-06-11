// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBAlogQueueConsumers
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday 05 May 2023 - 03:49PM
// ---------------------------------------------------------------------------
#include <SDBU/SDBUutils.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <SDB/SDBcapi.h>
#include <SDBA/SDBAlogQueueConsumers.h>
COL_LOG_MODULE;


void SDBAlogQueueConsumersImp(const COLstring& ComponentId, COLvar& Result){
   COL_FUNCTION(SDBAlogQueueConsumersImp);
   COL_VAR(ComponentId);
   COLarray<COLstring> Consumers = SDBIgetQueueConsumers(ComponentId);
   Result.setArrayType();
   for (int i = 0; i < Consumers.size(); ++i) {
      Result.push_back(Consumers[i]);
   }
}

// /log/queue/consumers
void SDBAlogQueueConsumers(const COLwebRequest& Request){
   COL_FUNCTION(SDBAlogQueueConsumers);
   COL_VAR(Request.Data.json(1));
   const COLvar& Data = Request.Data;
   COLstring ComponentId, Error;
   if(!SDBUcomponentId(COLvarGetString(Request.Data, "component", ""), &ComponentId, &Error)){ 
      return COLrespondError(Request.Address, Error); 
   }
   COLvar Result;
   SDBAlogQueueConsumersImp(ComponentId, Result);
   COLrespondSuccess(Request.Address, Result);
}