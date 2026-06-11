//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDIupgrade.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <GRIDA/GRIDAadd.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <GRID/GRIDnode.h>
#include <GRID/GRIDwebSocket.h>
#include <GRIDI/GRIDIhttp.h>
#include <GRIDI/GRIDIupgrade.h>
#include <IWEB/IWEBconfig.h>
#include <SCK/SCKloop.h>
#include <SCKS/SCKSsslConnection.h>
#include <WEB/WEBrequestData.h>
COL_LOG_MODULE;

static COLstring GRIDIinternalServerError() { return "HTTP/1.1 500 Internal Server Error\r\n\r\n"; }
static COLstring GRIDIbadRequest()          { return "HTTP/1.1 400 Bad Request\r\n\r\n"; }

static void GRIDIsendUpgradeResponse(const int ConnectionId, const COLstring& WebSocketKey, const COLstring& IguanaId) {
   COL_FUNCTION(GRIDIsendUpgradeResponse);
   const COLstring UpgradeResp = GRIDIbuildHttpUpgradeResponse(WebSocketKey, IguanaId);
   COLsendWebResponse(ConnectionId, UpgradeResp);
}

static SCKconnection* GRIDIgetConnection(SCKloop* pLoop, int ConnectionId) {
   COL_FUNCTION(GRIDIgetConnection);
   SCKconnection* pConn = pLoop->m_ConnectionMap[ConnectionId];
   if(pConn) { return pConn; }
   COL_ERR("Connection not found in loop: " << ConnectionId);
   return nullptr;
}

static bool GRIDIcheckHeaders(const COLmap<COLstring, COLstring>& Headers, COLstring& WsKey, COLstring& PeerId) {
   COL_FUNCTION(GRIDIvalidateUpgradeHeader);
   const auto IdIt      = Headers.find("iguanaid");
   const auto UpgradeIt = Headers.find("upgrade");
   const auto KeyIt     = Headers.find("sec-websocket-key");
   if(KeyIt != Headers.end()) { WsKey = KeyIt->second; }
   if(IdIt != Headers.end())  { PeerId = IdIt->second; }
   COL_VAR3(UpgradeIt, WsKey, PeerId);
   return UpgradeIt != Headers.end() && UpgradeIt->second == "websocket" && !WsKey.is_null() && !PeerId.is_null();
}

void GRIDIprocessWsUpgradeRequest(const WEBrequestData& Request, GRIDnode* pNode, SCKloop* pLoop) {
   COL_FUNCTION(GRIDIprocessWsUpgradeRequest);
   COLstring PeerId, WebSocketKey;
   if(!pNode)                                                          { return COLsendWebResponse(Request.ConnectionId, GRIDIinternalServerError()); }
   if(!GRIDIcheckHeaders(Request.Headers, WebSocketKey, PeerId)) { return COLsendWebResponse(Request.ConnectionId, GRIDIbadRequest()); }

   auto pConn = GRIDIgetConnection(pLoop, Request.ConnectionId);
   if(!pConn) { return COLsendWebResponse(Request.ConnectionId, GRIDIinternalServerError()); }

   auto& NodeInfo = pNode->getNodeInfo();
   if(NodeInfo.m_pWebConfig->useHttps() != pConn->sslEnabled()) {
      COL_TRC("Mismatched http/https.");
      return COLsendWebResponse(Request.ConnectionId, GRIDIinternalServerError());
   }
   const auto&     Peers    = pNode->getPeers();
   const auto      PeerIt   = Peers.find(PeerId);
   if(PeerIt == Peers.end()) { return COLsendWebResponse(Request.ConnectionId, "Iguana with this id has been rejected."); }

   GRIDIsendUpgradeResponse(Request.ConnectionId, WebSocketKey, NodeInfo.m_IguanaId);
   auto pWebSocket = new GRIDwebSocket(NodeInfo, Peers[PeerIt].Fingerprint, *pLoop, true, pConn);
   pNode->acceptIncomingConnection(PeerId, pWebSocket);
   pWebSocket->sendChallenge();
}