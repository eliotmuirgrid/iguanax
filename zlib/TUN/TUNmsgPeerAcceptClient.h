#ifndef __TUN_MSG_PEER_ACCEPT_CLIENT_H__
#define __TUN_MSG_PEER_ACCEPT_CLIENT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgPeerAcceptClient
//
// Description:
//
// TODO
//
// Author: Eliot Muir 
// Date:   Monday 26 June 2023 - 04:44PM
// ---------------------------------------------------------------------------

class TUNstateClient;
class TUNrequestItem;

void TUNmsgPeerAcceptClient(TUNrequestItem* pRequest, TUNstateClient* pState);

#endif // end of defensive include
