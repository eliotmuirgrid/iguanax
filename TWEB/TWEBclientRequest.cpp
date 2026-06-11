// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TWEBclientRequest
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Tuesday 08 August 2023 - 12:58PM
// ---------------------------------------------------------------------------

#include <TWEB/TWEBclientRequest.h>
#include <TUN/TUNstring.h>
#include <TUN/TUNstateClient.h>
#include <TUN/TUNsendPeerData.h>
#include <TUN/TUNcacheRequest.h>

#include <SCK/SCKloop.h>

#include <WEB/WEBrequestData.h>
#include <WEB/WEBserverDispatch.h>
#include <WEB/WEBserver.h>
#include <WEB/WEBfileHandler.h>

#include <COL/COLclosure6.h> // TODO - remove when ConnectionId is gone
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void TWEBsendBinaryResponse(const COLvar& Response, TUNstateClient* pState, const COLstring& PeerLabel){
   COL_FUNCTION(TWEBsendBinaryResponse);
   COLstring Message;
   Response.toBinary(&Message);
   TUNsendPeerData(pState, Message, PeerLabel);
}

static void TWEBprepareResponse(COLvar& Response, const COLstring& Type, const COLstring& ConnectionId, COLuint64 RequestId){
   COL_FUNCTION(TWEBprepareResponse);
   Response["t"] = Type;
   Response["c"] = ConnectionId;
   Response["i"] = COLuint64ToString(RequestId);
}

static void TWEBackResponse(TUNstateClient* pState, COLstring PeerLabel, COLstring ConnectionId, COLuint64 RequestId){
   COL_FUNCTION(TWEBackResponse);
   COLvar Response;
   TWEBprepareResponse(Response, "a", ConnectionId, RequestId); // a is for ACK response
   TWEBsendBinaryResponse(Response, pState, PeerLabel);
}

static void TWEBfileResponse(const COLstring& Data, TUNstateClient* pState, COLstring PeerLabel, COLstring ConnectionId, COLuint64 RequestId){
   COL_FUNCTION(TWEBfileResponse);
   COL_DUMP("Data", Data.c_str(), Data.size());
   COLvar Response;
   TWEBprepareResponse(Response, "f", ConnectionId, RequestId); // f is for FILE response
   Response["d"] = Data;
   TWEBsendBinaryResponse(Response, pState, PeerLabel);
}

static void TWEBmainCallback(const COLvar& Result, COLuint64 ConnectionId, COLuint64 RequestId, COLstring PeerLabel, TUNstateClient* pState){
   COL_FUNCTION(TWEBmainCallback);
   COL_VAR(Result.json());
   COLvar Response;
   TWEBprepareResponse(Response, "j", COLuint64ToString(ConnectionId), RequestId); // j is for JSON response
   Response["d"] = Result;
   TWEBsendBinaryResponse(Response, pState, PeerLabel);
}

static void TWEBthreadMainCallback(COLvar Result, COLuint64 ConnectionId, COLuint64 RequestId, COLstring PeerLabel, TUNstateClient* pState){
   COL_FUNCTION(TWEBthreadMainCallback);
   COL_VAR(Result.json());
   COLvar Response;
   TWEBprepareResponse(Response, "j", COLuint64ToString(ConnectionId), RequestId); // j is for JSON response
   Response["d"] = Result;
   TWEBsendBinaryResponse(Response, pState, PeerLabel);
}

static void TWEBthreadCallback(const COLvar& Result, COLuint64 ConnectionId, COLuint64 RequestId, COLstring PeerLabel, TUNstateClient* pState){
   COL_FUNCTION(TWEBthreadCallback);
   COL_VAR(Result);
   pState->m_pLoop->post(COLnewClosure0(&TWEBthreadMainCallback, Result, ConnectionId, RequestId, PeerLabel, pState));
}

static void TWEBdispatchWorkerThread(COLwebHandlerFunc pFunc, COLwebRequest Request){
   COL_FUNCTION(TWEBdispatchWorkerThread);
   try{
      (*pFunc)(Request);
   } catch (const COLerror& Error){
      COLvar Result;
      Result["success"] = false;
      Result["error"] = Error.description();
      Request.Address.pInternalResponseCallback->runAndDelete(Result);
   }
}

static void TWEBworkerThreadRequest(const COLstring& Path, const COLvar& Request, WEBserver* pServer, COLuint64 ConnectionId, COLuint64 RequestId, const COLstring& PeerLabel, TUNstateClient* pState){
   COL_FUNCTION(TWEBworkerThreadRequest);
   COLwebRequest WebRequest;
   WebRequest.Path = Path;
   if (Request.exists("d")){
      WebRequest.Data = Request["d"];
   }
   WebRequest.Address.pInternalResponseCallback = COLnewClosure1(&TWEBthreadCallback, ConnectionId, RequestId, PeerLabel, pState);
   COLwebHandlerFunc pFunc = pServer->m_WorkerThreadHandlers[Path];
   pState->m_pLoop->threadPool()->scheduleTask(COLnewClosure0(&TWEBdispatchWorkerThread, pFunc, WebRequest));
}

static void TWEBmainThreadRequest(const COLstring& Path, const COLvar& Request, WEBserver* pServer, COLuint64 ConnectionId, COLuint64 RequestId, const COLstring& PeerLabel, TUNstateClient* pState){
   COL_TRC("We have a main thread event.");
   COLwebRequest WebRequest;
   WebRequest.Path = Path;
   if (Request.exists("d")){
      WebRequest.Data = Request["d"];
   }
   WebRequest.Address.pInternalResponseCallback = COLnewClosure1(&TWEBmainCallback, ConnectionId, RequestId, PeerLabel, pState);
   try{
      pServer->m_MainThreadHandlers[Path]->run(WebRequest);
   } catch(const COLerror& Error){
      COLvar Result;
      Result["success"] = false;
      Result["error"] = Error.description();
      WebRequest.Address.pInternalResponseCallback->runAndDelete(Result);
   }
}

// TODO - this probably doesn't even get called.
static void TWEBheartbeat(COLuint64 ConnectionId, COLuint64 RequestId, const COLstring& PeerLabel, TUNstateClient* pState){
   COL_FUNCTION(TWEBheartbeat);
   // TODO this is jury rigged for now!
   COLvar Result;
   Result["success"] = true;
   Result["data"]["is_admin"] = false;
   Result["data"]["valid"] = true;
   Result["data"]["permissions"].setArrayType();
   Result["data"]["username"] = "admin";
   TWEBmainCallback(Result, ConnectionId, RequestId, PeerLabel, pState);
}

void TWEBclientRequest(const COLvar& Request, COLuint64 ConnectionId, COLuint64 RequestId, const COLstring& PeerLabel, TUNstateClient* pState, WEBserver* pServer){
   COL_FUNCTION(TWEBclientRequest);
   // TODO - retire ConnectionId once shift to websockets is complete.
   TWEBackResponse(pState, PeerLabel, Request["c"].asString(), RequestId);
   COLstring Path = Request["p"];
   if (pServer->m_MainThreadHandlers.count(Path)){
      TWEBmainThreadRequest(Path, Request, pServer, ConnectionId, RequestId, PeerLabel, pState);
   } else if (pServer->m_WorkerThreadHandlers.count(Path)){
      TWEBworkerThreadRequest(Path, Request, pServer, ConnectionId, RequestId, PeerLabel, pState);
   } else if (Path == "/session/heartbeat"){
      TWEBheartbeat(ConnectionId, RequestId, PeerLabel, pState);
   } else {
      WEBfileResponseReal(Path, COLnewClosure1(&TWEBfileResponse, pState, PeerLabel, Request["c"].asString(), RequestId), pServer->m_pLoop);
   }
}