#ifndef __TWEB_API_WEB_H__
#define __TWEB_API_WEB_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TWEBapiWeb
//
// Description:
//
// First cut at API to tunnel web requests to another peer.
//
// Author: Eliot Muir 
// Date:   Friday 30 June 2023 - 02:42PM
// ---------------------------------------------------------------------------
class COLstring;
class WEBrequestData;
class TUNstateClient;
class WEBserver;

void TWEBapiWillHandle(const WEBrequestData& RequestData, bool* pHandled);
void TWEBapiHandle(const WEBrequestData& RequestData, TUNstateClient* pState, WEBserver* pServer);
void TWEBapiSendData(TUNstateClient* pState, const COLstring& Peer, COLstring* pMessage, const COLstring& ConnectionId);

#endif // end of defensive include
