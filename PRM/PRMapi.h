#ifndef __PRM_API_H__
#define __PRM_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PRMapi
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Monday 29 January 2024 - 07:51AM
// ---------------------------------------------------------------------------
#include <COL/COLweb.h>
#include <COL/COLhashmap.h>
#include <COL/COLstring.h>

struct CFGmap;
class SCKloop;

void PRMapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, SCKloop* pLoop, CFGmap* pMap);

#endif // end of defensive include
