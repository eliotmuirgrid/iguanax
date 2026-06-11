#ifndef __TUN_MSG_PEER_DATA_H__
#define __TUN_MSG_PEER_DATA_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgPeerData
//
// Description:
//
// Receive data to send to peer on server from another client.
//
// Author: Eliot Muir 
// Date:   Wednesday 28 June 2023 - 01:27PM
// ---------------------------------------------------------------------------

class TUNstateServer;
class TUNrequestItem;

void TUNmsgPeerData(TUNrequestItem* pRequest, TUNstateServer* pState);

#endif // end of defensive include
