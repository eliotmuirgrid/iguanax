#ifndef __TUN_SEND_PEER_ASK_H__
#define __TUN_SEND_PEER_ASK_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNsendPeerAsk
//
// Description:
//
// Send the peer channel message.
//
// Author: Eliot Muir 
// Date:   Tuesday 27 June 2023 - 09:17PM
// ---------------------------------------------------------------------------


class TUNstateClient;
class TUNrequestItem;
class COLstring;

void TUNsendPeerAsk(TUNstateClient* pState, const COLstring& PeerLabel, COLstring* pOut);

void TUNsendAllPeerAsk(TUNstateClient* pState, TUNrequestItem* pOut);

void TUNsendPeerAddAsk(TUNstateClient* pState, const COLstring& PeerLabel);

#endif // end of defensive include
