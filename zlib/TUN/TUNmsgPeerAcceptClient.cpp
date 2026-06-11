// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgPeerAcceptClient
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 26 June 2023 - 04:44PM
// ---------------------------------------------------------------------------

#include <TUN/TUNmsgPeerAcceptClient.h>
#include <TUN/TUNrequest.h>
#include <TUN/TUNstring.h>
#include <TUN/TUNstateClient.h>
#include <TUN/TUNhash.h>
#include <TUN/TUNrecordType.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNmsgPeerAcceptClient(TUNrequestItem* pRequest, TUNstateClient* pState){
   COL_FUNCTION(TUNmsgPeerAcceptClient);
   TUNcursor Cursor = TUNcursor(pRequest->RecordData);
   COLstring ChallengeResponse, PeerKeyInDer;
   TUNstringRead(&Cursor, &ChallengeResponse);
   TUNstringRead(&Cursor, &PeerKeyInDer);
   COLstring PeerKeyHash = TUNhashKey(PeerKeyInDer);
   COL_DUMP("ChallengeResponse", ChallengeResponse.c_str(), ChallengeResponse.size());
   COL_DBG(pState->m_KeyToPeerMap.size());
   COL_DBG(PeerKeyHash);
   if (pState->m_KeyToPeerMap.count(PeerKeyHash)){
      COLstring PeerLabel = pState->m_KeyToPeerMap[PeerKeyHash];
      TUNpeer& Peer = pState->m_Peers[PeerLabel];
      COL_DBG(Peer.m_Label << " sent a response...");
      if (Peer.m_Challenge == ChallengeResponse){
         COL_TRC("Success - challenge answered");
         Peer.m_Authenticated = true;
         TUNrecordPeerAccepted(Peer.m_Label);
         // TODO - send message to verify route?
         if (pState->m_pOnConnect.get()){
            pState->m_pOnConnect->run(PeerLabel);
         }
      } else {
         // COL_ERR("   " << Peer.m_Label << " failed challenge.");
         // TODO - send an error response.
      }
   } else {
      COL_ERR("Could not find this key.");
   }
   TUNrecordPeerStatus(pState);
   TUNupdateLastActionTime(&pState->m_LastActivity);
}
