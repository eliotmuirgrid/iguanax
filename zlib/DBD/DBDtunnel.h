#ifndef __DBD_TUNNEL_H__
#define __DBD_TUNNEL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDtunnel
//
// Description:
//
// Code to start up and shut down the tunnel client in Iguana
//
// Author: Eliot Muir 
// Date:   Thursday 29 June 2023 - 09:58AM
// ---------------------------------------------------------------------------

class TUNstateClient;
class SCKloop;
class WEBserver;

void DBDtunnelInit(WEBserver* pServer, TUNstateClient* pState, SCKloop* pLoop);
void DBDtunnelShutdown(TUNstateClient* pState);

#endif // end of defensive include
