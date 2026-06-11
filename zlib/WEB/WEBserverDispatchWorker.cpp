// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBserverDispatchWorker
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday 22 May 2023 - 10:27PM
// ---------------------------------------------------------------------------

#include <WEB/WEBserverDispatchWorker.h>
#include <WEB/WEBrequestData.h>
#include <WEB/WEBsessionHandler.h>
#include <WEB/WEBresponse.h>

#include <SCK/SCKloop.h>
#include <WEB/WEBcookie.h>
#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void WEBserverDispatchWorkerThread(COLwebHandlerFunc pFunc, COLwebRequest Request){
   COL_FUNCTION(WEBserverDispatchWorkerThread);
   try{
      (*pFunc)(Request);
   } catch (const COLerror& Error){
      // TODO - this gives us a memory leak if the request is internal....
      COLstring Result;
      WEBresponseErrorGen(Error.description(), &Result);
      COL_VAR2(Error.description(), Result);
      WEBresponseSend(Request.Address.ConnectionId, Result);
   }
}

static void WEBrequestMapData(const WEBrequestData& Input, COLwebRequest* pOut){
   COL_FUNCTION(WEBrequestData);
   pOut->Address.ConnectionId = Input.ConnectionId;
   pOut->IsSecure             = Input.IsSecure;
   pOut->Data                 = WEBrequestInputs(Input);
   pOut->SessionId            = WEBcookieGet(Input.Headers, WEBdefaultCookieName(Input.IsSecure));
}

bool WEBserverDispatchWorker(const WEBrequestData& RequestData, COLhashmap<COLstring, COLwebHandlerFunc>& Handlers, COLstring User, bool AdminUser, SCKloop* pLoop){
   COL_FUNCTION(WEBserverDispatchWorker);
   COLhashmapPlace i = Handlers.find(RequestData.Path);
   if (!i) {
      COL_TRC("Not handled.");
      return false;
   }
   COLwebRequest WebRequest;
   WebRequest.User = User;
   WebRequest.AdminUser = AdminUser;
   WEBrequestMapData(RequestData, &WebRequest);
   COLwebHandlerFunc pFunc = Handlers[i];
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&WEBserverDispatchWorkerThread, pFunc, WebRequest));
   return true;
}

bool WEBserverDispatchMain(const WEBrequestData& RequestData, COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& Handlers, COLstring User, bool AdminUser){
   COL_FUNCTION(WEBserverDispatchMain);
   COLhashmapPlace i = Handlers.find(RequestData.Path);
   if (!i) {
      COL_TRC("Not handled.");
      return false;
   }
   COLwebRequest WebRequest;
   WebRequest.User = User;
   WebRequest.AdminUser = AdminUser;
   WEBrequestMapData(RequestData, &WebRequest);
   Handlers[i]->run(WebRequest);
   return true;
}