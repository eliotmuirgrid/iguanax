// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgPeerAskClient
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 26 June 2023 - 11:43AM
// ---------------------------------------------------------------------------

#include <TUN/TUNmsgPeerAskClient.h>
#include <TUN/TUNstring.h>
#include <TUN/TUNrequest.h>
#include <TUN/TUNkeyFingerPrint.h>
#include <TUN/TUNkeyConvert.h>
#include <TUN/TUNkey.h>
#include <TUN/TUNchallenge.h>
#include <TUN/TUNstateClient.h>
#include <TUN/TUNrecord.h>
#include <TUN/TUNrecordType.h>
#include <TUN/TUNhash.h>
#include <TUN/TUNsendPeerAsk.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// TUN_PEER_ASK - for the destination client
void TUNmsgPeerAskClient(TUNrequestItem* pRequest, TUNstateClient* pState){
   COL_FUNCTION(TUNmsgPeerAskClient);
   COLstring PeerInDerKey, ChallengeString;
   TUNcursor Cursor(pRequest->RecordData);
   // TODO error handling!
   TUNstringRead(&Cursor, &PeerInDerKey);
   TUNstringRead(&Cursor, &ChallengeString); 
      
   TUNkey PeerPublicKey;
   TUNkeyPublicFromDer(PeerInDerKey, &PeerPublicKey);

   COLstring Response;
   TUNstringWrite(&Response, PeerInDerKey);

   if (pState->m_KeyToPeerMap.count(TUNhashKey(PeerInDerKey))  == 0){
      TUNstringWrite(&Response, "F");
      TUNrecordWrite(pRequest, TUN_PEER_ACCEPT, Response);
      COL_ERR("   Nope - ask request refused");
   } else {
      COL_DBG("Received peer handshake from client " << TUNkeyFingerPrintFromDer(PeerInDerKey));
      COLstring ChallengeResponse = TUNchallengeResponse(pState->m_MyKey, PeerPublicKey, ChallengeString);
      COL_DUMP("ChallengeResponse", ChallengeResponse.c_str(), ChallengeResponse.size());
      TUNstringWrite(&Response, ChallengeResponse);
      COLstring PeerIndex = pState->m_KeyToPeerMap[TUNhashKey(PeerInDerKey)];
      TUNpeer& Peer = pState->m_Peers[PeerIndex];
      TUNrecordClientPeerRequest(Peer.m_Label, PeerIndex);
      TUNrecordWrite(pRequest, TUN_PEER_ACCEPT, Response);
      TUNrecordClientPeerRequest(Peer.m_Label, PeerIndex);
      if (!Peer.m_Authenticated){
         TUNsendPeerAsk(pState, PeerIndex, pRequest->pOut);
      }
   }
   TUNupdateLastActionTime(&pState->m_LastActivity);
}
