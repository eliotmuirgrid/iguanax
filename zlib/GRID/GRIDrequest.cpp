//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDrequest.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <GRID/GRIDnode.h>
#include <GRID/GRIDrequest.h>
#include <GRID/GRIDwebSocket.h>

#include <memory>
COL_LOG_MODULE;

struct GRIDremoteRequestTracker {
   explicit GRIDremoteRequestTracker(const int Count, GRIDnode* pNode, SCKloop* pLoop,
                                     COLauto<COLclosure1<const COLvar&>> pCallback)
       : m_Count(Count),
         m_pNode(pNode),
         m_pLoop(pLoop),
         m_TimerId(-1),
         m_TimedOut(false),
         m_pCallback(pCallback) {}
   int                                 m_Count;
   COLvar                              m_Results;
   GRIDnode*                           m_pNode;
   SCKloop*                            m_pLoop;
   int                                 m_TimerId;
   bool                                m_TimedOut;
   COLhashmap<COLuint64, bool>         m_PendingConnections;
   COLauto<COLclosure1<const COLvar&>> m_pCallback;
};

static void GRIDremoteRequestTimeout(std::shared_ptr<GRIDremoteRequestTracker> pTracker) {
   COL_FUNCTION(GRIDremoteRequestTimeout);
   COL_TRC("Peer stats request timed out, " << pTracker->m_Count << " peers did not respond");
   pTracker->m_TimedOut = true;
   auto& ApiConnections = pTracker->m_pNode->getApiConnections();
   for(const auto& it : pTracker->m_PendingConnections) {
      const COLuint64 ConnectionId = it.first;
      if(ApiConnections.count(ConnectionId)) {
         COL_TRC("Removing pending connection: " << ConnectionId);
         delete ApiConnections[ConnectionId].pCallback;
         if(ApiConnections[ConnectionId].TimerId != -1) {
            pTracker->m_pLoop->cancelTimer(ApiConnections[ConnectionId].TimerId);
         }
         ApiConnections.removeKey(ConnectionId);
      }
   }
   pTracker->m_pCallback->run(pTracker->m_Results); // partial results
}

static void GRIDpeerRemoteCallback(const COLvar& Result, const COLstring IguanaId,
                                  std::shared_ptr<GRIDremoteRequestTracker> pTracker, const COLuint64 ConnectionId) {
   COL_FUNCTION(GRIDpeerRemoteCallback);
   if(pTracker->m_TimedOut) {  // If we already timed out, don't do anything
      COL_TRC("Ignoring late response from " << IguanaId << " (already timed out)");
      return;
   }
   COL_VAR(Result);
   if(Result["success"].asBool()) { pTracker->m_Results[IguanaId] = Result["data"]; }
   else {
      pTracker->m_Results[IguanaId]["error_msg"]  = Result["error"];
      pTracker->m_Results[IguanaId]["error_code"] = 403;
   }

   pTracker->m_Count--;
   pTracker->m_PendingConnections.removeKey(ConnectionId);
   COL_TRC("Received stats from " << IguanaId << ", remaining: " << pTracker->m_Count);

   if(pTracker->m_Count == 0) {
      COL_TRC("All peer stats collected, responding");
      if(pTracker->m_TimerId != -1) {
         pTracker->m_pLoop->cancelTimer(pTracker->m_TimerId);
         pTracker->m_TimerId = -1;
      }
      pTracker->m_pCallback->run(pTracker->m_Results);
   }
}

static void GRIDpeerMakeRequest(const COLstring& Api, const COLvar& Data, GRIDnode* pNode, const COLstring IguanaId, std::shared_ptr<GRIDremoteRequestTracker> pTracker) {
   COL_FUNCTION(GRIDpeerMakeRequest);
   const COLuint64             ConnectionId = GRIDgenerateConnectionId(pNode);
   COLclosure1<const COLvar&>* pCallback    = COLnewClosure1(&GRIDpeerRemoteCallback, IguanaId, pTracker, ConnectionId);

   GRIDwebSocket* pConnection = GRIDfindConnectedPeer(pNode, IguanaId);
   if(!pConnection) {
      delete pCallback;
      pTracker->m_Count--;
      return;
   }

   COLvar RemoteRequest;
   GRIDbuildRemoteRequestJson(RemoteRequest, Api, Data, ConnectionId);

   pNode->getApiConnections()[ConnectionId] = GRIDnode::GRIDapiConnection(pCallback, -1);
   pConnection->sendJson(RemoteRequest.json(0));
   pTracker->m_PendingConnections[ConnectionId] = true;
}

static COLvar GRIDrequestError(const COLstring& Error) {
   COL_FUNCTION(GRIDrequestError);
   COLvar ErrorVar;
   ErrorVar["success"] = false;
   ErrorVar["error"]   = Error;
   return Error;
}

void GRIDrequestAll(const COLstring& Api, const COLvar& Data, GRIDnode* pNode, SCKloop* pLoop, COLauto<COLclosure1<const COLvar&>> pCallback) {
   COL_FUNCTION(GRIDrequestAll);
   COL_VAR2(Api, pNode);
   if(!pNode)                 { return pCallback->run(GRIDrequestError("Grid not initialized.")); }
   if(!pNode->validLicense()) { return pCallback->run(GRIDrequestError("Insufficient license.")); }
   if(pNode->IsStopping()) {
      COL_TRC("Node is stopping, skipping request");
      return pCallback->run(GRIDrequestError("Node is stopping."));
   }

   const auto Peers    = pNode->getPeers();
   auto       pTracker = std::make_shared<GRIDremoteRequestTracker>(Peers.size(), pNode, pLoop, pCallback);
   pTracker->m_Results.setMapType();
   if(Peers.size() == 0) {
      COL_TRC("No peers to query");
      return pCallback->run(pTracker->m_Results);
   }

   // Set a timeout of 10 seconds for all peer responses
   pTracker->m_TimerId = pLoop->addTimer(COLnewClosure0(&GRIDremoteRequestTimeout, pTracker), 10000);
   COL_TRC("Started timeout timer: " << pTracker->m_TimerId);

   for(const auto& it : Peers) {
      COL_VAR(it.first);
      GRIDpeerMakeRequest(Api, Data, pNode, it.first, pTracker);
   }

   // for the case when all connections are disconnected
   if(pTracker->m_Count == 0) {
      COL_TRC("No connected peers to query");
      if(pTracker->m_TimerId != -1) {
         pLoop->cancelTimer(pTracker->m_TimerId);
         pTracker->m_TimerId = -1;
      }
      return pCallback->run(pTracker->m_Results);
   }
}

COLuint64 GRIDgenerateConnectionId(GRIDnode* pNode) {
   COL_FUNCTION(GRIDgenerateConnectionId);
   static COLuint64 NextId         = 1;
   auto&            ApiConnections = pNode->getApiConnections();

   COLuint64 ConnectionId = NextId++;
   while(ApiConnections.count(ConnectionId)) {
      ConnectionId = NextId++;
      COL_TRC("ConnectionId collision, trying next: " << ConnectionId);
   }
   return ConnectionId;
}

GRIDwebSocket* GRIDfindConnectedPeer(GRIDnode* pNode, const COLstring& IguanaId) {
   COL_FUNCTION(GRIDfindConnectedPeer);
   const auto& Connections = pNode->getPeers();
   const auto  It          = Connections.find(IguanaId);
   if(It == Connections.end()) {
      COL_TRC("Peer not found: " << IguanaId);
      return nullptr;
   }
   GRIDwebSocket* pConnection = It->second.pConnection;
   if(!pConnection || !pConnection->connected()) {
      COL_TRC("Peer not connected: " << IguanaId);
      return nullptr;
   }
   return pConnection;
}

void GRIDbuildRemoteRequestJson(COLvar& OutRequest, const COLstring& Api, const COLvar& Data, const COLuint64 ConnectionId) {
   COL_FUNCTION(GRIDbuildRemoteRequestJson);
   OutRequest["t"] = "r";
   OutRequest["p"] = Api;
   OutRequest["d"] = Data;
   OutRequest["c"] = COLuint64ToString(ConnectionId);
}