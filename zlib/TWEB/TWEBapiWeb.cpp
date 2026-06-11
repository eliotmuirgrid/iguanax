// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNapiWeb
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Friday 30 June 2023 - 02:42PM
// ---------------------------------------------------------------------------

#include <TWEB/TWEBapiWeb.h>
#include <TUN/TUNcacheRequest.h>
#include <TUN/TUNstateClient.h>
#include <TUN/TUNsendPeerData.h>
#include <TUN/TUNrecord.h>
#include <TUN/TUNrequestId.h>

#include <WEB/WEBrequestData.h>
#include <WEB/WEBserver.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void TWEBapiWillHandle(const WEBrequestData& RequestData, bool* pHandled){
   //COL_FUNCTION(TWEBapiWillHandle);
  // COL_VAR(RequestData.Path);
   if (RequestData.Path.size() < 6){
      return;
   }
   if (0 == strncmp("/peer/", RequestData.Path.c_str(), 6)){
      *pHandled = true;
   }
}

void TWEBpack(const WEBrequestData& RequestData, COLvar* pOut){
   COL_FUNCTION(TWEBpack);
   COLvar& Out = *pOut;
   if (!RequestData.Body.is_null()){
      Out["d"].parse(RequestData.Body);
   } else {
      if (RequestData.Args.size() > 0){
         COLvar& Vars = Out["d"];
         for (auto i=RequestData.Args.cbegin(); i != RequestData.Args.cend(); i++){
            Vars[i->first] =i->second;
         } 
      }
   }
   Out["c"] = COLint64ToString(RequestData.ConnectionId);  // TODO - optmize?
}

COLstring TWEBpeer(const COLstring& Path, COLstring* pRealPath){
   COL_FUNCTION(TWEBpeer);
   COLstring Remainder, Peer;
   Remainder = Path.substr(6);
   Remainder.split(Peer, *pRealPath, "/");
   COL_VAR(Peer);
   return Peer;
}

static void TWEBresendRequest(TUNstateClient* pState, COLstring Peer, COLstring ConnectionId){
   COL_FUNCTION(TWEBresendRequest);
   COL_VAR(ConnectionId);
   pState->m_RequestsInProgress[ConnectionId]->m_AckTimer = pState->m_pLoop->addTimer(COLnewClosure0(TWEBresendRequest, pState, Peer, ConnectionId), 1000); // try resending again in a second
   TUNsendPeerData(pState, *(pState->m_RequestsInProgress[ConnectionId]->m_pBinaryRequestData), Peer);
}

void TWEBapiSendData(TUNstateClient* pState, const COLstring& Peer, COLstring* pMessage, const COLstring& ConnectionId){
   COL_FUNCTION(TWEBapiSendData);
   int AckTimer = 0;
   if (pState->m_Peers[Peer].m_PeerProtocolVer){  // TODO - want a better way of setting protocol version than just relying on receiving an ack
      COL_TRC("Using protocol version 1 so set up an ack timer");
      AckTimer = pState->m_pLoop->addTimer(COLnewClosure0(TWEBresendRequest, pState, Peer, ConnectionId), 500); // expect to hear back ack within 500ms
   } else {
      COL_TRC("Using protocol version 0 or this is the first request and we haven't upgraded yet");
   }

   TUNcacheRequest(pMessage, AckTimer, ConnectionId, pState);
   COL_VAR(pState->m_RequestsInProgress.size());

   TUNsendPeerData(pState, *pMessage, Peer);
}

// TODO - we don't have any logic to tell the remote peer when a connection has closed yet
// to tell annotation jobs to early return etc.

void TWEBapiHandle(const WEBrequestData& RequestData, TUNstateClient* pState, WEBserver* pServer){
   COL_FUNCTION(TWEBapiHandle);
   COLstring RealPath;
   COLaddress Address;
   Address.ConnectionId = RequestData.ConnectionId;
   COLstring Peer = TWEBpeer(RequestData.Path, &RealPath);
   if (pState->m_Peers.count(Peer)==0){
      // TODO have a HTML error.
      COLrespondError(Address, "No peer called " + Peer);
      return;   
   }
   if (!pState->m_Peers[Peer].m_Authenticated){
      COLrespondError(Address, Peer + " is not reachable.");
      return;        
   }
   
   COLstring* pMessage = new COLstring(); // the request will be cached - we clean up this pointer when we are sure the request has been responded to.
   COLvar Data;
   COLuint64 RequestId = TUNgetNextRequestId(pState);
   Data["p"] = "/" + RealPath;
   Data["t"] = "r";
   Data["i"] = COLuint64ToString(RequestId);
   TWEBpack(RequestData, &Data);
   COL_VAR(Data.json(1));
   Data.toBinary(pMessage);

   TWEBapiSendData(pState, Peer, pMessage, Data["c"]);
}
