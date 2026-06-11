#ifndef __TUN_RECORD_TYPE_H__
#define __TUN_RECORD_TYPE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNrecordType
//
// Description:
//
// Record types in protocol
//
// Author: Eliot Muir 
// Date:   Monday 26 June 2023 - 10:42AM
// ---------------------------------------------------------------------------

#include <COL/COLclosure.h>
#include <COL/COLminimumInclude.h>

class COLstring;
class TUNstateClient;

enum TUNrecordType{
   TUN_HELLO=0,
   TUN_CHALLENGE_QUESTION=1,
   TUN_CHALLENGE_RESPONSE=2,
   TUN_CHALLENGE_ACCEPTED=3, 
   TUN_PEER_ASK=4,
   TUN_PEER_ACCEPT=5,
   TUN_PEER_DISCONNECTED=6,
   TUN_PEER_DATA=7
};

const char* TUNrecordTypeAsString(TUNrecordType T);

void TUNrecordOutTrace(TUNrecordType T, COLuint64 ConnectionId);
void TUNrecordInTrace (TUNrecordType T, COLuint64 ConnectionId);
void TUNrecordDisconnect(COLuint64 ConnectionId);
void TUNrecordClientNotConnected();
void TUNrecordClientPeerRequest(const COLstring& PeerLabel, const COLstring& ClientId);

void TUNrecordPeerDisconnected(const COLstring& Label);
void TUNrecordPeerAccepted(const COLstring& Label);


void TUNsetNameMap(COLclosure2<COLuint64, COLstring*>* pNameMap);
void TUNrecordPeerStatus(TUNstateClient* pState);

#endif // end of defensive include
