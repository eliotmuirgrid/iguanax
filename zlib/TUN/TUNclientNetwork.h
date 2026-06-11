#ifndef __TUN_CLIENT_NETWORK_H__
#define __TUN_CLIENT_NETWORK_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNclientNetwork
//
// Description:
//
// TUNclientNetwork - set up tunnel client
//
// Author: Eliot Muir 
// Date:   Friday 16 June 2023 - 01:28PM
// ---------------------------------------------------------------------------

class TUNstateClient;

void TUNclientNetworkInit(TUNstateClient* pState);
void TUNclientShutdown(TUNstateClient* pState);

#endif // end of defensive include
