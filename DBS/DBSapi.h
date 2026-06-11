#ifndef __DBS_API_H__
#define __DBS_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBSapi
//
// Description:
//
// API module for DB schema support
//
// Author: Eliot Muir 
// Date:   Tuesday 08 November 2022 - 08:48AM
// ---------------------------------------------------------------------------

#include <COL/COLweb.h>
#include <COL/COLhashmap.h>
class SCKloop;

struct CFGmap;

void DBSapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, CFGmap* pMap, SCKloop* pLoop);

#endif // end of defensive include
