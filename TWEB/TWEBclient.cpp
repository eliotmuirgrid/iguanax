// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TWEBclient
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday 30 June 2023 - 04:04PM
// ---------------------------------------------------------------------------

#include <TWEB/TWEBclient.h>
#include <TWEB/TWEBclientRequest.h>
#include <TUN/TUNstateClient.h>
#include <TUN/TUNcacheRequest.h>

#include <SCK/SCKconnectionWrite.h>
#include <SCK/SCKloop.h>

#include <COL/COLweb.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void TWEBclientFileResponse(const COLvar& Request, COLuint64 ConnectionId, COLuint64 RequestId, TUNstateClient* pState){
   COL_FUNCTION(TWEBclientFileResponse);
   // This is a file response which comes with the headers.
   if (pState->m_LuaConnections.count(ConnectionId)){
      COL_TRC("This was a request from Lua - a remote request from translator or BLUA. Will respond to the waiting function.");
      pState->m_LuaConnections[ConnectionId]->runAndDelete(Request["d"]);
      pState->m_LuaConnections.removeKey(ConnectionId);
   }
   // TODO - it would make more sense to just send the file content and generate the HTTP headers locally.
   COL_TRC("Got a response");
   SCKconnectionWrite(pState->m_pLoop, ConnectionId, Request["d"]);
   TUNunloadRequest(COLuint64ToString(ConnectionId), pState);
   COL_VAR(pState->m_RequestsInProgress.size());
}

static void TWEBclientJsonResponse(const COLvar& Request, COLuint64 ConnectionId, COLuint64 RequestId, TUNstateClient* pState){
   COL_FUNCTION(TWEBclientJsonResponse);
   COL_TRC("Got a JSON response");
   COLstring HttpResponse;
   if (pState->m_LuaConnections.count(ConnectionId)){
      COL_TRC("This was a request from Lua - a remote request from translator or BLUA. Will respond to the waiting function.");
      pState->m_LuaConnections[ConnectionId]->runAndDelete(Request["d"]);
      pState->m_LuaConnections.removeKey(ConnectionId);
   }
   TUNunloadRequest(COLuint64ToString(ConnectionId), pState);
   COL_VAR(pState->m_RequestsInProgress.size());
   COLwebJsonResponse(Request["d"], &HttpResponse);
   SCKconnectionWrite(pState->m_pLoop,ConnectionId, HttpResponse);
}

static void TWEBclientAckResponse(const COLvar& Request, COLuint64 ConnectionId, COLuint64 RequestId, const COLstring& PeerLabel, TUNstateClient* pState){
   COL_FUNCTION(TWEBclientAckResponse);
   COL_TRC("Got a ACK response");
   // If we got an ack back, "upgrade" the connection so the next time we send a request, we can expect an ack
   if (pState->m_Peers[PeerLabel].m_PeerProtocolVer == 0) { pState->m_Peers[PeerLabel].m_PeerProtocolVer = 1; }

   if (pState->m_RequestsInProgress.count(COLuint64ToString(ConnectionId)) && pState->m_RequestsInProgress[COLuint64ToString(ConnectionId)]->m_AckTimer != 0){
      pState->m_pLoop->cancelTimer(pState->m_RequestsInProgress[COLuint64ToString(ConnectionId)]->m_AckTimer);
      COL_TRC("Canceled ACK timer, now wait for a response.");
      TUNupdateLastActionTime(&pState->m_LastActivity);
   }
}

void TWEBclient(const COLstring& PeerLabel, const COLstring& Data, TUNstateClient* pState, WEBserver* pServer){
   COL_FUNCTION(TWEBclient);
   COL_TRC(Data.size());
   int BytesConsumed;
   COLvar Request;
   Request.parseBinary(Data.c_str(), Data.size(), &BytesConsumed);
   COL_VAR(Request.json(1));
   COLstring Type = Request["t"].asString();
   COLuint64 ConnectionId = atoll(Request["c"].asString().c_str());
   COLuint64 RequestId    = atoll(Request["i"].asString().c_str());

   // TODO - what is the most efficient way to drive this?
   if      (Type == "r"){ TWEBclientRequest     (Request, ConnectionId, RequestId, PeerLabel, pState, pServer); }
   else if (Type == "f"){ TWEBclientFileResponse(Request, ConnectionId, RequestId, pState); }
   else if (Type == "j"){ TWEBclientJsonResponse(Request, ConnectionId, RequestId, pState); }
   else if (Type == "a"){ TWEBclientAckResponse (Request, ConnectionId, RequestId, PeerLabel, pState); }
}
