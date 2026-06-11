#ifndef __TWEB_CLIENT_REQUEST_H__
#define __TWEB_CLIENT_REQUEST_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TWEBclientRequest
//
// Description:
//
// Handling of peer requests, figuring out what type of response to send back 
// over the tunnel.
//
// Author: Vismay Shah 
// Date:   Tuesday 08 August 2023 - 12:58PM
// ---------------------------------------------------------------------------
#include <COL/COLminimumInclude.h>
class COLstring;
class COLvar;
class TUNstateClient;
class WEBserver;

void TWEBclientRequest(const COLvar& Request, COLuint64 ConnectionId, COLuint64 RequestId, const COLstring& PeerLabel, TUNstateClient* pState, WEBserver* pServer);

#endif // end of defensive include
