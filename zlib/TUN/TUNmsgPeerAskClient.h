#ifndef __TUN_MSG_PEER_ASK_CLIENT_H__
#define __TUN_MSG_PEER_ASK_CLIENT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgPeerAskClient
//
// Description:
//
// Received peer handshake from another pper
//
// Author: Eliot Muir 
// Date:   Monday 26 June 2023 - 11:43AM
// ---------------------------------------------------------------------------

class TUNstateClient;
class TUNrequestItem;

void TUNmsgPeerAskClient(TUNrequestItem* pRequest, TUNstateClient* pState);

#endif // end of defensive include
