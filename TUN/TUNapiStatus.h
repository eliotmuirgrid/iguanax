#ifndef __TUN_API_STATUS_H__
#define __TUN_API_STATUS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNapiStatus
//
// Description:
//
// Get the status of the tunnel connection.
//
// Author: Eliot Muir 
// Date:   Thursday 29 June 2023 - 11:20AM
// ---------------------------------------------------------------------------

class COLwebRequest;
class TUNstateClient;

void TUNapiStatus(const COLwebRequest& Request, TUNstateClient* pState);

#endif // end of defensive include