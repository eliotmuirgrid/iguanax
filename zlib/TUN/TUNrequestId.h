#ifndef __TUN_REQUEST_ID_H__
#define __TUN_REQUEST_ID_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNrequestId
//
// Description:
//
// Monotonically increasing request id for use in tunnel protocol. Better than 
// using the connection id from the browser as those are re-used.
//
// Author: Vismay Shah 
// Date:   Thursday 10 August 2023 - 02:15PM
// ---------------------------------------------------------------------------
#include <COL/COLminimumInclude.h>
class TUNstateClient;

COLuint64 TUNgetNextRequestId(TUNstateClient* pState);
COLuint64 TUNcurrentRequestId(TUNstateClient* pState);


#endif // end of defensive include
