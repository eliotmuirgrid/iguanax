//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDnode.h
//
//  Description:
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <GRID/GRIDhandleJsonReqRes.h>
#include <GRID/GRIDnode.h>
#include <GRID/GRIDwebSocket.h>
#include <SCK/SCKcall.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void GRIDsendJsonResponseMain(const COLvar& Result, GRIDnode* pNode, const COLstring IguanaId, COLstring ConnectionId) {
   COL_FUNCTION(GRIDsendJsonResponseMain);
   COLvar Response;
   Response["t"]                = "j";
   Response["c"]                = ConnectionId;
   Response["d"]                = Result;
   const COLstring JsonResponse = Response.json(0);
   COL_TRC("Sending JSON response: " << JsonResponse);
   const auto& Peers  = pNode->getPeers();
   const auto  PeerIt = Peers.find(IguanaId);
   if(PeerIt != Peers.end()) {
      if(Peers[PeerIt].pConnection && Peers[PeerIt].pConnection->connected()) {
         Peers[PeerIt].pConnection->sendJson(JsonResponse);
      } else {
         COL_TRC(IguanaId << " is not connected, ignoring response");
      }
   }
}

static void GRIDworkerThreadCallback(const COLvar Result, COLauto<COLclosure1<const COLvar&>> pCallback) {
   COL_FUNCTION(GRIDworkerThreadCallback);
   pCallback->run(Result);
}

static void GRIDexecuteApiCall(COLstring Api, COLvar Params, SCKloop* pLoop, COLauto<COLclosure1<const COLvar&>> pCallback) {
   COL_FUNCTION(GRIDexecuteApiCall);
   COL_TRC("Executing API call on worker thread: " << Api);
   COLvar     Result;
   const bool ApiCallSuccess = SCKcall(Api, Params, "", &Result, false);
   if(!ApiCallSuccess) {
      Result["success"] = false;
      Result["error"]   = "API call failed.";
   }
   COL_TRC("API call completed, result: " << Result.json(0));
   pLoop->post(COLnewClosure0(&GRIDworkerThreadCallback, Result, pCallback));
}

static void GRIDhandleRequest(const COLvar& Request, GRIDnode* pNode, const COLstring& IguanaId, SCKloop* pLoop) {
   COL_FUNCTION(GRIDhandleRequest);
   const COLstring Api          = Request["p"].asString();
   const COLstring ConnectionId = Request["c"].asString();
   COLvar          Params;
   if(Request.exists("d")) { Params = Request["d"]; }
   COL_TRC("Dispatching API request to worker thread: " << Api);
   COLauto<COLclosure1<const COLvar&>> pCallback = COLnewClosure1(&GRIDsendJsonResponseMain, pNode, IguanaId, ConnectionId);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&GRIDexecuteApiCall, Api, Params, pLoop, pCallback));
}

static COLvar GRIDerrorVar(const COLstring& Error) {
   COL_FUNCTION(GRIDerrorVar);
   COLvar ErrorVar;
   ErrorVar["success"] = false;
   ErrorVar["error"]   = Error;
   return ErrorVar;
}

static void GRIDhandleIncomingRequest(const COLvar& Message, const COLstring& IguanaId, GRIDnode* pNode, SCKloop* pLoop) {
   COL_FUNCTION(GRIDhandleIncomingRequest);
   if(pNode->getReadAccess(IguanaId) != GRIDreadAccessLevel::Full) {
      COL_TRC("Access denied for incoming request");
      return GRIDsendJsonResponseMain(GRIDerrorVar("You do not have read access to this server."), pNode, IguanaId, Message["c"].asString());
   }
   COL_TRC("Handling incoming request");
   GRIDhandleRequest(Message, pNode, IguanaId, pLoop);
}

static void GRIDhandleJsonResponse(const COLvar& Message, GRIDnode* pNode, SCKloop* pLoop, const COLuint64 ConnectionId) {
   COL_FUNCTION(GRIDhandleJsonResponse);
   auto& ApiConnections = pNode->getApiConnections();
   
   if(!ApiConnections.count(ConnectionId)) {
      COL_TRC("No connection registered for ConnectionId " << ConnectionId);
      return;
   }
   
   COL_TRC("Found callback for ConnectionId " << ConnectionId);
   const int TimerId = ApiConnections[ConnectionId].TimerId;
   if(TimerId != -1) {
      pLoop->cancelTimer(TimerId);
      COL_TRC("Cancelled timer " << TimerId);
   }
   ApiConnections[ConnectionId].pCallback->runAndDelete(Message["d"]);
   ApiConnections.removeKey(ConnectionId);
   COL_TRC("Callback executed and removed");
}

void GRIDonJson(const COLstring* pJsonPayload, GRIDnode* pNode, const COLstring IguanaId, SCKloop* pLoop) {
   COL_FUNCTION(GRIDhandleJsonRequest);
   COL_TRC("Got JSON message: " << *pJsonPayload);
   COLvar Message;
   Message.parse(*pJsonPayload);
   COL_VAR(Message.json(1));

   const COLstring Type         = Message["t"].asString();
   const COLuint64 ConnectionId = atoll(Message["c"].asString().c_str());
   COL_TRC("Message type: " << Type << ", ConnectionId: " << ConnectionId);

   if(Type == "r") {
      GRIDhandleIncomingRequest(Message, IguanaId, pNode, pLoop);
   } else if(Type == "j") {
      GRIDhandleJsonResponse(Message, pNode, pLoop, ConnectionId);
   } else {
      COL_TRC("Unknown message type: " << Type);
   }
}
