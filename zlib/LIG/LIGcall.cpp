//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LIGcall
//
// Description:
//
// Implementation
//
// Author: Shuo Hou
//---------------------------------------------------------------------------
#include "LIGcall.h"

#include <COL/COLclosure6.h>
#include <COL/COLevent.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <GRID/GRIDnode.h>
#include <GRID/GRIDwebSocket.h>
#include <GRIDI/GRIDIinit.h>
#include <LCOM/LCOMrunAs.h>
#include <LUA/LUAcheck.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAopen.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAutils.h>
#include <LUAL/LUALutils.h>
#include <SCK/SCKcall.h>
#include <SCK/SCKloop.h>
#include <TUN/TUNsendPeerData.h>
#include <TUN/TUNstateClient.h>
COL_LOG_MODULE;

int LIGcallList(lua_State* L) {
   COL_FUNCTION(LIGcallList);

   COLmap<COLstring, bool> AllApis;
   SCKlistApis(&AllApis);
   COL_VAR(AllApis.size());

   COLvar CallList;
   for (auto i = AllApis.begin(); i != AllApis.end(); i++){
      if(i->first.find("/ide/") == 0) { continue; } // skip /ide/* apis
      CallList.push_back(i->first);
   }
   COL_VAR(CallList);

   CallList.toLua(L);
   return 1;
}

static void LIGreceivedRemoteResponse(const COLvar& DataIn, COLevent* pEvent, COLvar* pResult){
   COL_FUNCTION(LIGreceivedRemoteResponse);
   COL_TRC("Got the data from the remote Iguana, signal the event");
   *pResult = DataIn;
   pEvent->signal();
}


static void LIGsendRemoteRequestMain(TUNstateClient* pState, COLuint64 ConnectionId, COLstring Message,
                                     COLstring Peer, COLevent* pOnReponse, COLvar* pResult){
   COL_FUNCTION(LIGsendRemoteRequestMain);
   pState->m_LuaConnections.add(ConnectionId, COLnewClosure1(&LIGreceivedRemoteResponse, pOnReponse, pResult));
   TUNsendPeerData(pState, Message, Peer);
}

static void LIGsendGridRequestMain(GRIDnode* pNode, COLuint64 ConnectionId, COLstring JsonRequest, COLstring Peer,
                                   COLevent* pOnReponse, COLvar* pResult) {
   COL_FUNCTION(LIGsendGridRequestMain);
   const auto& Connections = pNode->getPeers();
   const auto  it          = Connections.find(Peer);
   if(it == Connections.end()) {
      COL_ERR("GRID peer not found: " << Peer);
      pResult->reset();
      pOnReponse->signal();
      return;
   }
   GRIDwebSocket* pConnection = it->second.pConnection;
   if(!pConnection || !pConnection->connected()) {
      COL_ERR("GRID peer not connected: " << Peer);
      pResult->reset();
      pOnReponse->signal();
      return;
   }
   pNode->getApiConnections().add(ConnectionId, GRIDnode::GRIDapiConnection(COLnewClosure1(&LIGreceivedRemoteResponse, pOnReponse, pResult), -1));
   COL_TRC("Sending GRID request to " << Peer << ": " << JsonRequest);
   pConnection->sendJson(JsonRequest);
}

static bool LIGcheckPeerName(TUNstateClient* pState, const COLstring& Peer){
   COL_FUNCTION(LIGcheckPeerName);
   COL_VAR(pState->m_Peers.count(Peer));
   return pState->m_Peers.count(Peer);
}

static bool LIGcheckPeerOnline(TUNstateClient* pState, const COLstring& Peer){
   COL_FUNCTION(LIGcheckPeerOnline);
   COL_VAR(pState->m_Peers[Peer].m_Authenticated);
   return pState->m_Peers[Peer].m_Authenticated;
}

static bool LIGcheckGridPeerName(GRIDnode* pNode, const COLstring& Peer) {
   COL_FUNCTION(LIGcheckGridPeerName);
   const auto& Connections = pNode->getPeers();
   const bool  Exists      = Connections.count(Peer) > 0;
   COL_VAR(Exists);
   return Exists;
}

static bool LIGcheckGridPeerOnline(GRIDnode* pNode, const COLstring& Peer) {
   COL_FUNCTION(LIGcheckGridPeerOnline);
   const auto& Connections = pNode->getPeers();
   const auto  It          = Connections.find(Peer);
   if(It == Connections.end()) return false;
   bool Connected = It->second.pConnection && It->second.pConnection->connected();
   COL_VAR(Connected);
   return Connected;
}

static COLvar LIGsetupRemoteRequest(const COLstring& Api, COLuint64 ConnectionId, const COLvar& Params){
   COL_FUNCTION(LIGsetupRemoteRequest);
   COLvar Out;
   Out["t"] = "r";
   Out["p"] = Api;
   Out["c"] = COLuint64ToString(ConnectionId);
   Out["d"] = Params;
   return Out;
}

void LIGpopulateAgent(lua_State* L, COLstring *pAgent, bool& ComponentAgent){
   COL_FUNCTION(LIGpopulateAgent);
   if (LCOMgetRunAs(L, pAgent)) {
      // TODO: we need to make sure whoever is calling iguana.call{} as a user is allowed to do so.
      // e.g. standard user uses iguana.call{} to obtain admin privileges - bad
      ComponentAgent = false;
      COL_TRC("Run iguana.call{} as user " << *pAgent);
   } else {
      *pAgent = LUAcomponentId(L);
      COL_TRC("Run iguana.call{} as component " << *pAgent);
   }
}

static int LIGiguanaCall(lua_State* L, const COLstring& Api, const COLvar& Params){
   COL_FUNCTION(LIGiguanaCall);
   COLstring Agent;
   bool ComponentAgent = true;
   LIGpopulateAgent(L, &Agent, ComponentAgent);
   COLvar Result;
   COL_TRC("About to do SCKcall " << Api);
   // SCKcall as the specified user if username password are provided and pass validation
   bool ApiCallSuccess = SCKcall(Api, Params, Agent, &Result, ComponentAgent);
   COL_VAR(ApiCallSuccess);
   if (!ApiCallSuccess) {
      return LUAraiseLuaError(L, "Api call failed. Please verify that the api is correct.");
   }
   COL_TRC("Got data:");
   COL_VAR(Result);
   Result.toLua(L);
   return 1;
}

static int LIGremoteCallGrid(lua_State* L, const COLstring& Api, const COLstring& Peer, const COLvar& Params) {
   COL_FUNCTION(LIGremoteCallGrid);
   const COLuint64 ConnectionId = (COLuint64)LUAgetPointerField(L, "_pConnectionId");
   auto            pNode        = (GRIDnode*)LUAgetPointerField(L, "_pGridNode");
   auto            pLoop        = (SCKloop*)LUAgetPointerField(L, "_pEventLoop");

   if(!pNode) { return LUAraiseLuaError(L, "GRID not initialized."); }

   if(!LIGcheckGridPeerName(pNode, Peer))   { return LUAraiseLuaError(L, "GRID peer not found. Please verify that the peer name provided is correct."); }
   if(!LIGcheckGridPeerOnline(pNode, Peer)) { return LUAraiseLuaError(L, "Connection to GRID peer failed. Please verify that the peer is online."); }

   const COLvar RemoteRequest = LIGsetupRemoteRequest(Api, ConnectionId, Params);

   COLvar   Result;
   COLevent Event;
   pLoop->post(COLnewClosure0(&LIGsendGridRequestMain, pNode, ConnectionId,  RemoteRequest.json(), Peer, &Event, &Result));

   COL_TRC("Starting Wait for GRID response");
   Event.wait();
   COL_TRC("Got GRID data:");
   COL_VAR(Result);
   Result.toLua(L);
   return 1;
}

static int LIGremoteCall(lua_State* L, const COLstring& Api, const COLstring& Peer, const COLvar& Params) {
   COL_FUNCTION(LIGremoteCall);
   COLuint64 ConnectionId = (COLuint64)      LUAgetPointerField(L, "_pConnectionId");
   TUNstateClient* pState = (TUNstateClient*)LUAgetPointerField(L, "_pTunnelState");
   SCKloop* pLoop         = (SCKloop*)       LUAgetPointerField(L, "_pEventLoop");
   
   if (!LIGcheckPeerName  (pState, Peer)) return LUAraiseLuaError(L, "Peer not found. Please verify that the peer name provided is correct.");
   if (!LIGcheckPeerOnline(pState, Peer)) return LUAraiseLuaError(L, "Connection to peer failed. Please verify that the peer is online.");
   
   COLvar RemoteRequest = LIGsetupRemoteRequest(Api, ConnectionId, Params);
   COLstring Message;
   RemoteRequest.toBinary(&Message);
   
   COLvar Result;
   COLevent Event;
   pLoop->post(COLnewClosure0(&LIGsendRemoteRequestMain, pState, ConnectionId, Message, Peer, &Event, &Result));
   
   COL_TRC("Starting Wait");
   Event.wait();
   COL_TRC("Got data:");
   COL_VAR(Result);
   Result.toLua(L);
   return 1;
}

int LIGcall(lua_State* L) {
   COL_FUNCTION(LIGcall);
   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);
   const COLstring Api  = LUAexpectedStringParam(L, 1, "api");
   const COLstring Peer = LUAoptionalStringParam(L, 1, "peer");
   const bool      Live = LUAoptionalBoolParam(L, 1, "live", false);
   COLvar    Params;
   COL_TRC("Getting data from lua");
   if(LUAoptionalTableParam(L, 1, "data")) {
      try {
         Params.fromLua(L);
      } catch(const COLerror& Error) { return LUAraiseLuaError(L, Error.description()); }
   }
   COL_VAR4(Api, Params, Peer, Live);
   if(Api.find("/ide/") == 0) { LUA_ERROR_STREAM("Translator APIs (/ide/*) are not supported."); }
   bool NoAct = LUALisEnvironmentTest(L) ? !Live : false;
   if(LUALisEnvironmentStartup(L)) NoAct = true;
   if(!NoAct) {
      if(!Peer.is_null()) {
         COL_TRC("Calling api on peer Iguana");
         return LIGremoteCallGrid(L, Api, Peer, Params);
         // return LIGremoteCall(L, Api, Peer, Params);
      }
      COL_TRC("Calling api on local Iguana");
      return LIGiguanaCall(L, Api, Params);
   }
   LUApushString(L, "Not running, live is false.");
   return 1;
}