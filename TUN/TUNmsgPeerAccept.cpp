// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgPeerAccept
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 26 June 2023 - 01:19PM
// ---------------------------------------------------------------------------

#include <TUN/TUNmsgPeerAccept.h>
#include <TUN/TUNrequest.h>
#include <TUN/TUNchallenge.h>
#include <TUN/TUNstring.h>
#include <TUN/TUNstateServer.h>
#include <TUN/TUNrecord.h>
#include <TUN/TUNrecordType.h>
#include <TUN/TUNhash.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// TUN_PEER_ACCEPT - via server
void TUNmsgPeerAccept(TUNrequestItem* pRequest, TUNstateServer* pState){
   COL_FUNCTION(TUNmsgPeerAccept);
   COLstring PeerKeyInDer, ChallengeResponse;
   TUNcursor Cursor(pRequest->RecordData);
   TUNstringRead(&Cursor, &PeerKeyInDer);
   TUNstringRead(&Cursor, &ChallengeResponse);

   COL_DUMP("ChallengeResponse", ChallengeResponse.c_str(), ChallengeResponse.size());
    
   COLstring KeyHash = TUNhashKey(PeerKeyInDer);


   if (pState->m_PublicKeyToIdMap.count(KeyHash) == 0){
      COL_ERR("This peer doesn't exist");
   } else {
      COLuint64 ConnectionId = pState->m_PublicKeyToIdMap[KeyHash];
      COL_TRC("Send message to connection " << ConnectionId);
      COLstring ResponseB;
      TUNstringWrite(&ResponseB, ChallengeResponse);
      TUNstringWrite(&ResponseB, pState->m_ConnectionMap[pRequest->ConnectionId].m_ConnectionPublicKeyInDer);
      TUNrecordConnectionWrite(pState->m_pLoop, ConnectionId, TUN_PEER_ACCEPT, ResponseB);
   }
}
