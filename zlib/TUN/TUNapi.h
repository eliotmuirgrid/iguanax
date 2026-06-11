#ifndef __TUN_API_H__
#define __TUN_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNapi
//
// Description:
//
// API to access status of the tunnel client
//
// Author: Eliot Muir
// Date:   Thursday 29 June 2023 - 11:17AM
// ---------------------------------------------------------------------------

#include <COL/COLweb.h>
#include <COL/COLhashmap.h>

class TUNstateClient;

void TUNapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, TUNstateClient* pClient);

#endif // end of defensive include