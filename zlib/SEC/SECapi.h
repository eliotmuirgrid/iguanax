#ifndef __SECAPI_H__
#define __SECAPI_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SECapi
//
// Description:
//
// Author: Matthew Sobkowski
// Date: 2024-11-28
// ---------------------------------------------------------------------------
class SCKloop;

#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

void SECapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop);

#endif // end of defensive include

