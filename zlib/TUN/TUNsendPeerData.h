#ifndef __TUN_SEND_PEER_DATA_H__
#define __TUN_SEND_PEER_DATA_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNsendPeerData
//
// Description:
//
// Send data to peer - the message is encrpyted.
//
// Author: Eliot Muir 
// Date:   Wednesday 28 June 2023 - 01:02PM
// ---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class TUNstateClient;
class COLstring;

void TUNsendPeerData(TUNstateClient* pState, const COLstring& Data, const COLstring& PeerLabel);

#endif // end of defensive include
