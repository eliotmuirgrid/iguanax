#ifndef __TUN_CLIENT_LOAD_H__
#define __TUN_CLIENT_LOAD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNclientLoad
//
// Description:
//
// Load a client config file into TUNstateClient object.
//
// Author: Eliot Muir 
// Date:   Monday 26 June 2023 - 02:11PM
// ---------------------------------------------------------------------------

class COLstring;
class TUNstateClient;
class COLaddress;

// Used this in my command line client.
void TUNclientLoad(const COLstring& FileName, TUNstateClient* pState);

// Using this in Iguana.
bool TUNclientPeerLoad(TUNstateClient* pState, COLstring* pError);

void TUNclientPeerSave(const TUNstateClient& State, COLaddress Address);

#endif // end of defensive include
