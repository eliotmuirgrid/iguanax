#ifndef __TWEB_CLIENT_H__
#define __TWEB_CLIENT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TWEBclient
//
// Description:
//
// Handle web tunnel request on target.
//
// Author: Eliot Muir 
// Date:   Friday 30 June 2023 - 04:04PM
// ---------------------------------------------------------------------------

class COLstring;
class TUNstateClient;
class WEBserver;

void TWEBclient(const COLstring& PeerLabel, const COLstring& Data, TUNstateClient* pState, WEBserver* pServer);
#endif // end of defensive include
