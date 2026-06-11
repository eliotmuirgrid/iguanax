// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgPeerDataClient
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 28 June 2023 - 01:48PM
// ---------------------------------------------------------------------------

#include <TUN/TUNmsgPeerDataClient.h>
#include <TUN/TUNstring.h>
#include <TUN/TUNrequest.h>
#include <TUN/TUNstateClient.h>
#include <TUN/TUNaes.h>

#include <SFI/SFIaesDecryptor.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNmsgPeerDataClient(TUNrequestItem* pRequest, TUNstateClient* pState){
   COL_FUNCTION(TUNmsgPeerDataClient);
   TUNcursor Cursor(pRequest->RecordData);
   COLstring PeerIndex;
   TUNstringRead(&Cursor, &PeerIndex);

   if (pState->m_Peers.count(PeerIndex)== 0){
      COL_ERR(PeerIndex << " is invalid!");
      return;
   }

   COLstring Data;
   TUNstringRead(&Cursor, &Data);

   COLstring& SharedSecret = pState->m_Peers[PeerIndex].m_SharedSecret;
   SFIaesDecryptor Descryptor((COLuint8*)SharedSecret.c_str(), 16);
   TUNaesDecrypt(Descryptor, &Data);

   COL_VAR2(PeerIndex, Data);
   if (pState->m_pOnData.get()){
      pState->m_pOnData->run(PeerIndex, Data);
   }
   TUNupdateLastActionTime(&pState->m_LastActivity);
}
