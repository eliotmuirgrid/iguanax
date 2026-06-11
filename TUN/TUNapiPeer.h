#ifndef __TUN_API_PEER_H__
#define __TUN_API_PEER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNapiPeer
//
// Description:
//
// Get information on Peer by name
//
// Author: Eliot Muir 
// Date:   Thursday 29 June 2023 - 02:49PM
// ---------------------------------------------------------------------------

class COLwebRequest;
class TUNstateClient;

void TUNapiPeer(const COLwebRequest& Request, TUNstateClient* pState);

#endif // end of defensive include
