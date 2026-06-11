#ifndef __TUN_MSG_PEER_DATA_CLIENT_H__
#define __TUN_MSG_PEER_DATA_CLIENT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgPeerDataClient
//
// Description:
//
// Receive peer data from another client on a client.
//
// Author: Eliot Muir 
// Date:   Wednesday 28 June 2023 - 01:48PM
// ---------------------------------------------------------------------------

class TUNstateClient;
class TUNrequestItem;

void TUNmsgPeerDataClient(TUNrequestItem* pRequest, TUNstateClient* pState);

#endif // end of defensive include
