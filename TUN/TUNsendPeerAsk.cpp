// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNsendPeerAsk
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 27 June 2023 - 09:17PM
// ---------------------------------------------------------------------------

#include <TUN/TUNsendPeerAsk.h>
#include <TUN/TUNrecord.h>
#include <TUN/TUNstateClient.h>
#include <TUN/TUNkeyConvert.h>
#include <TUN/TUNrecordType.h>
#include <TUN/TUNchallenge.h>
#include <TUN/TUNstring.h>
#include <TUN/TUNnumber.h>
#include <TUN/TUNrequest.h>

#include <SCK/SCKconnectionWrite.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// TODO - we'll need to factor this out into it's own routine.
void TUNsendPeerAsk(TUNstateClient* pState, const COLstring& PeerLabel, COLstring* pOut){
   COL_FUNCTION(TUNsendPeerAsk);
   TUNpeer* pPeer = &pState->m_Peers[PeerLabel];
   if (pPeer->m_Authenticated){
      COL_TRC("This peer is already proven.");
   }

   COLstring EncryptedString;
   TUNchallenge(pState->m_MyKey, pPeer->m_PublicKey, &pPeer->m_SharedSecret, &pPeer->m_Challenge, &EncryptedString);
   COLstring Message;
   TUNstringWrite(&Message, TUNkeyPublicAsDer(pPeer->m_PublicKey));
   TUNstringWrite(&Message, EncryptedString);
   TUNstringWrite(&Message, PeerLabel);
   TUNrecordWriteString(pOut, TUN_PEER_ASK, Message);
   TUNrecordClientPeerRequest(pPeer->m_Label, PeerLabel);
   COL_DUMP("Challenge", pPeer->m_Challenge.c_str(), pPeer->m_Challenge.size());
}

void TUNsendAllPeerAsk(TUNstateClient* pState, TUNrequestItem* pRequest){
   COL_FUNCTION(TUNsendAllPeerAsk);
   for (auto i = pState->m_Peers.begin(); i != pState->m_Peers.end(); i++){
      TUNsendPeerAsk(pState, i->first, pRequest->pOut);
   }
}

void TUNsendPeerAddAsk(TUNstateClient* pState, const COLstring& PeerLabel){
   COL_FUNCTION(TUNsendPeerAddAsk);
   COLstring Message;
   TUNsendPeerAsk(pState, PeerLabel, &Message);
   SCKconnectionWrite(pState->m_pLoop, pState->m_ConnectionId, Message);
}
