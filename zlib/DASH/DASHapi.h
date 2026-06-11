#ifndef DASHAPI_H
#define DASHAPI_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DASHapi.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   19/06/25 2:18 PM
//  ---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

class SCKloop;

struct CFGmap;

void DASHapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, SCKloop* pLoop, CFGmap* pMap);

#endif // end of defensive include
