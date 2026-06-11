#ifndef __TUN_MSG_PEER_ASK_H__
#define __TUN_MSG_PEER_ASK_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgPeerAsk
//
// Description:
//
// Receive a peer handshake
//
// Author: Eliot Muir 
// Date:   Friday 16 June 2023 - 04:38PM
// ---------------------------------------------------------------------------

class TUNstateServer;
class TUNrequestItem;

void TUNmsgPeerAsk(TUNrequestItem* pRequest, TUNstateServer* pState);

#endif // end of defensive include
