// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNrecordType
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 26 June 2023 - 10:42AM
// ---------------------------------------------------------------------------

#include <TUN/TUNrecordType.h>
#include <TUN/TUNstateClient.h>

#include <COL/COLauto.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLauto< COLclosure2<COLuint64, COLstring*> > s_pNameMap;

void TUNsetNameMap(COLclosure2<COLuint64, COLstring*>* pNameMap){
   s_pNameMap = pNameMap;
}

COLstring TUNmapName(COLuint64 ConnectionId){
   COLstring Result;
   if (s_pNameMap.get()){
      s_pNameMap->run(ConnectionId, &Result);
   }
   return Result;
}

const char* TUNrecordTypeAsString(TUNrecordType T) {
    switch (T) {
        case TUN_HELLO:              return "TUN_HELLO";
        case TUN_CHALLENGE_QUESTION: return "TUN_CHALLENGE_QUESTION";
        case TUN_CHALLENGE_RESPONSE: return "TUN_CHALLENGE_RESPONSE";
        case TUN_CHALLENGE_ACCEPTED: return "TUN_CHALLENGE_ACCEPTED";
        case TUN_PEER_ASK:           return "TUN_PEER_ASK";
        case TUN_PEER_ACCEPT:        return "TUN_PEER_ACCEPT";
        case TUN_PEER_DISCONNECTED:  return "TUN_PEER_DISCONNECTED";
        case TUN_PEER_DATA:          return "TUN_PEER_DATA";
        default:                     return "Unknown TUNrecordType";
    }
}

void TUNrecordOutTrace(TUNrecordType T, COLuint64 ConnectionId){
   COL_TRC(TUNmapName(ConnectionId) << " --> " << TUNrecordTypeAsString(T));
}

void TUNrecordInTrace(TUNrecordType T, COLuint64 ConnectionId){
   COL_TRC(TUNmapName(ConnectionId) << " <-- " << TUNrecordTypeAsString(T));
}

void TUNrecordDisconnect(COLuint64 ConnectionId){
   COL_TRC(TUNmapName(ConnectionId) << " DISCONNECTED");
}

void TUNrecordClientNotConnected(){
   COL_TRC("   Peer not connected");
}

void TUNrecordClientPeerRequest(const COLstring& PeerLabel, const COLstring& ClientId){
   COL_TRC("     For " << PeerLabel << " (" << ClientId << ")");
}

void TUNrecordPeerDisconnected(const COLstring& Label){
   COL_TRC("   " << Label << " not connected");
}

void TUNrecordPeerAccepted(const COLstring& Label){
   COL_TRC("   " << Label << " verified to own key.");
}

void TUNrecordPeerStatus(TUNstateClient* pState){
   for (auto i = pState->m_Peers.begin(); i != pState->m_Peers.end(); i++){
      COL_TRC("   " << i->first << " - connected = " << i->second.m_Authenticated);
   }
}
