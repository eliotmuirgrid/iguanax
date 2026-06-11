#ifndef __TUN_API_KEY_PUBLIC_H__
#define __TUN_API_KEY_PUBLIC_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNapiKeyPublic
//
// Description:
//
// API to get the public API for the server.
//
// Author: Eliot Muir 
// Date:   Thursday 29 June 2023 - 01:33PM
// ---------------------------------------------------------------------------

class COLwebRequest;
class TUNstateClient;

void TUNapiKeyPublic(const COLwebRequest& Request, TUNstateClient* pState);

#endif // end of defensive include
