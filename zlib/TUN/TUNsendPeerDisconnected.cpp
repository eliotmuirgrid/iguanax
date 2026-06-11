// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNsendPeerDisconnected
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 27 June 2023 - 04:24PM
// ---------------------------------------------------------------------------

#include <TUN/TUNsendPeerDisconnected.h>

#include <TUN/TUNrecord.h>
#include <TUN/TUNrecordType.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNsendPeerDisconnected(SCKloop* pLoop, COLuint64 ConnectionId, const COLstring& PublicKeyInDer){
   COL_FUNCTION(TUNsendPeerDisconnected);
   TUNrecordConnectionWrite(pLoop, ConnectionId, TUN_PEER_DISCONNECTED, PublicKeyInDer);
}