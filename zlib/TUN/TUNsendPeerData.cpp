// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNsendPeerData
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 28 June 2023 - 01:02PM
// ---------------------------------------------------------------------------

#include <TUN/TUNsendPeerData.h>
#include <TUN/TUNstateClient.h>
#include <TUN/TUNnumber.h>
#include <TUN/TUNrecord.h>
#include <TUN/TUNstring.h>
#include <TUN/TUNrecordType.h>
#include <TUN/TUNaes.h>

#include <SFI/SFIaesEncryptor.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNsendPeerData(TUNstateClient* pState, const COLstring& Data, const COLstring& PeerLabel){
   COL_FUNCTION(TUNsendPeerData);
   COL_VAR2(PeerLabel, Data);
   COLstring Record;
   TUNstringWrite(&Record, PeerLabel);
 
   // TODO we need ephemeral keys for more security.
   COLstring& SharedSecret = pState->m_Peers[PeerLabel].m_SharedSecret;
   SFIaesEncryptor Encryptor((COLuint8*)SharedSecret.c_str(), 16);
   // TODO need to use IV for better encryption.
   COLstring OutData(Data);
   TUNaesEncrypt(Encryptor, &OutData);
   TUNstringWrite(&Record, OutData);   
   TUNrecordConnectionWrite(pState->m_pLoop, pState->m_ConnectionId, TUN_PEER_DATA, Record);
}