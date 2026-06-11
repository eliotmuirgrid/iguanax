// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgPeerDisconnected
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 27 June 2023 - 04:40PM
// ---------------------------------------------------------------------------

#include <TUN/TUNmsgPeerDisconnected.h>
#include <TUN/TUNrequest.h>
#include <TUN/TUNhash.h>
#include <TUN/TUNstateClient.h>
#include <TUN/TUNrecordType.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNmsgPeerDisconnected(TUNrequestItem* pRequest, TUNstateClient* pState){
   COL_FUNCTION(TUNmsgPeerDisconnected);
   COLstring KeyHash = TUNhashKey(pRequest->RecordData);
   if (pState->m_KeyToPeerMap.count(KeyHash)){
      COLstring PeerIndex  = pState->m_KeyToPeerMap[KeyHash];
      pState->m_Peers[PeerIndex].m_Authenticated = false;
      TUNrecordPeerDisconnected(pState->m_Peers[PeerIndex].m_Label);
      if (pState->m_pOnDisconnect.get()){
         pState->m_pOnDisconnect->run(PeerIndex);
      }
   } else {
      COL_ERR("Unknown peer key.");
   }
   TUNrecordPeerStatus(pState);
   TUNupdateLastActionTime(&pState->m_LastActivity);
}
