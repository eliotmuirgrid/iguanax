#ifndef __TUN_MSG_PEER_DISCONNECTED_H__
#define __TUN_MSG_PEER_DISCONNECTED_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgPeerDisconnected
//
// Description:
//
// Message that peer with given public key is not connected
//
// Author: Eliot Muir 
// Date:   Tuesday 27 June 2023 - 04:40PM
// ---------------------------------------------------------------------------


class TUNstateClient;
class TUNrequestItem;

void TUNmsgPeerDisconnected(TUNrequestItem* pRequest, TUNstateClient* pState);

#endif // end of defensive include
