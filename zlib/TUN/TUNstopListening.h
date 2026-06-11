#ifndef __TUN_STOP_LISTENING_H__
#define __TUN_STOP_LISTENING_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNstopListening
//
// Description:
//
// API for testing - shouldn't be used in production. Sets a boolean in the 
// TUNstateClient preventing responses to web requests.
//
// Author: Vismay Shah 
// Date:   Wednesday 02 August 2023 - 10:21AM
// ---------------------------------------------------------------------------
class TUNstateClient;
class COLwebRequest;

void TUNstopListening(const COLwebRequest& Request, TUNstateClient* pClient);

#endif // end of defensive include
