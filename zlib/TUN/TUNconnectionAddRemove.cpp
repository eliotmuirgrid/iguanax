// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNconnectionAddRemove
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Friday 23 June 2023 - 04:39PM
// ---------------------------------------------------------------------------

#include <TUN/TUNconnectionAddRemove.h>
#include <TUN/TUNstateServer.h>
#include <TUN/TUNkeyFingerPrint.h>
#include <TUN/TUNhash.h>
#include <TUN/TUNrecordType.h>
#include <TUN/TUNsendPeerDisconnected.h>

#include <SCK/SCKconnectionWrite.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNconnectionRegister(COLuint64 ConnectionId, TUNstateServer* pState){
   COL_FUNCTION(TUNconnectionRegister);
   COL_TRC(*pState);
   COLstring Der = pState->m_ConnectionMap[ConnectionId].m_ConnectionPublicKeyInDer;
   COLstring HashDer = TUNhashKey(Der);
   COL_VAR4(ConnectionId, TUNkeyFingerPrintFromDer(Der), pState->m_PublicKeyToIdMap.count(HashDer), HashDer);
   
   if (pState->m_PublicKeyToIdMap.count(HashDer)){
      COL_TRC("There is an existing connection for this client.  Kick them off.");
      SCKconnectionHardClose(pState->m_pLoop, pState->m_PublicKeyToIdMap[HashDer]);
   }
   pState->m_PublicKeyToIdMap[HashDer] = ConnectionId;
   COL_TRC("Registered!");
   COL_TRC(*pState);
}


void TUNconnectionRemove(COLuint64 ConnectionId, TUNstateServer* pState){
   COL_FUNCTION(TUNconnectionRemove);
   TUNrecordDisconnect(ConnectionId);
   auto j = pState->m_ConnectionMap[ConnectionId].m_PeerMap;
   for (auto i = j.begin(); i != j.end(); i++){
      TUNsendPeerDisconnected(pState->m_pLoop, i->second.m_ConnectionId, pState->m_ConnectionMap[ConnectionId].m_ConnectionPublicKeyInDer);
   }

   COL_VAR(*pState);
   COL_TRC("Now remove connection " << ConnectionId);
   COLstring Der = pState->m_ConnectionMap[ConnectionId].m_ConnectionPublicKeyInDer;
   pState->m_PublicKeyToIdMap.removeKey(TUNhashKey(Der));
   pState->m_ConnectionMap.removeKey(ConnectionId);
   COL_VAR(*pState);
}