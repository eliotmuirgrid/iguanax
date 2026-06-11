#ifndef __TUN_API_PEER_DELETE_H__
#define __TUN_API_PEER_DELETE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNapiPeerDelete
//
// Description:
//
// Remove a peer
//
// Author: Eliot Muir 
// Date:   Thursday 29 June 2023 - 02:39PM
// ---------------------------------------------------------------------------

class COLwebRequest;
class TUNstateClient;

void TUNapiPeerDelete(const COLwebRequest& Request, TUNstateClient* pState);

#endif // end of defensive include
