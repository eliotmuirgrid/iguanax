// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgPeerAsk
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Friday 16 June 2023 - 04:38PM
// ---------------------------------------------------------------------------

#include <TUN/TUNmsgPeerAsk.h>
#include <TUN/TUNrequest.h>
#include <TUN/TUNkey.h>
#include <TUN/TUNkeyConvert.h>
#include <TUN/TUNstateServer.h>
#include <TUN/TUNstring.h>
#include <TUN/TUNhash.h>
#include <TUN/TUNrecord.h>
#include <TUN/TUNrecordType.h>
#include <TUN/TUNiv.h>
#include <TUN/TUNsendPeerDisconnected.h>

#include <SFI/SFIbase64EncodeFilter.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring TUNgeneratePeerId(){
    COL_FUNCTION(TUNgeneratePeerId);
    COLstring Id;
    TUNiv(&Id, 8);
    return SFIbase64Encode(Id);
}

// TUN_PEER_ASK - by server
void TUNmsgPeerAsk(TUNrequestItem* pRequest, TUNstateServer* pState){
   COL_FUNCTION(TUNmsgPeerAsk);
   COL_VAR(pRequest->ConnectionId);
   COLstring PeerKey, ChallengeString;
   TUNcursor Cursor(pRequest->RecordData);
   // TODO - we really need to check the return of each of these read operations
   TUNstringRead(&Cursor, &PeerKey);
   TUNstringRead(&Cursor, &ChallengeString);
   COLstring ClientId;
   TUNstringRead(&Cursor, &ClientId);

   COLstring PeerHash = TUNhashKey(PeerKey);
   COL_VAR(PeerHash);
   auto i = pState->m_PublicKeyToIdMap.find(PeerHash);
   if (i){
      COL_TRC("That peer is connected on " << i->second);
      // TODO - make sure this state gets cleared.
      pState->m_ConnectionMap[pRequest->ConnectionId].m_ConnectionIdToPeerMap[i->second] = ClientId;
      pState->m_ConnectionMap[pRequest->ConnectionId].m_PeerMap[ClientId].m_ConnectionId = i->second;
      COLstring Record;
      TUNstringWrite(&Record, pState->m_ConnectionMap[pRequest->ConnectionId].m_ConnectionPublicKeyInDer);
      TUNstringWrite(&Record, ChallengeString);
      TUNrecordClientPeerRequest(pState->m_ConnectionMap[i->second].m_Name, ClientId);
      TUNrecordConnectionWrite(pState->m_pLoop, i->second, TUN_PEER_ASK, Record);
   } else {
      COL_TRC("That peer is not currently connected.");
      TUNrecordClientNotConnected();
      TUNsendPeerDisconnected(pState->m_pLoop, pRequest->ConnectionId, PeerKey);
   }
}
