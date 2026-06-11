#ifndef __TUN_SEND_PEER_DISCONNECTED_H__
#define __TUN_SEND_PEER_DISCONNECTED_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNsendPeerDisconnected
//
// Description:
//
// Tells the client that the client with the given public key has disconnected.
//
// Author: Eliot Muir 
// Date:   Tuesday 27 June 2023 - 04:24PM
// ---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class SCKloop;
class COLstring;

void TUNsendPeerDisconnected(SCKloop* pLoop, COLuint64 ConnectionId, const COLstring& PublicKeyInDer);

#endif // end of defensive include
