#ifndef __TUN_API_PEER_ADD_H__
#define __TUN_API_PEER_ADD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNapiPeerAdd
//
// Description:
//
// Add a peer.
//
// Author: Eliot Muir 
// Date:   Thursday 29 June 2023 - 02:22PM
// ---------------------------------------------------------------------------

class COLwebRequest;
class TUNstateClient;

void TUNapiPeerAdd(const COLwebRequest& Request, TUNstateClient* pState);

#endif // end of defensive include
