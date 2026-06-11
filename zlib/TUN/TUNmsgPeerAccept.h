#ifndef __TUN_MSG_PEER_ACCEPT_H__
#define __TUN_MSG_PEER_ACCEPT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgPeerAccept
//
// Description:
//
// Message to say the peer has been accepted.
//
// Author: Eliot Muir 
// Date:   Monday 26 June 2023 - 01:19PM
// ---------------------------------------------------------------------------

class TUNstateServer;
class TUNrequestItem;

void TUNmsgPeerAccept(TUNrequestItem* pRequest, TUNstateServer* pState);

#endif // end of defensive include
