#ifndef __DTLAPI_H__
#define __DTLAPI_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DTLapi.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   25/03/24 12:27 PM
//  ---------------------------------------------------------------------------
#include <COL/COLweb.h>
#include <COL/COLhashmap.h>

struct CFGmap;
class SCKloop;

void DTLapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, CFGmap* pMap, SCKloop* pLoop);
void DTLapiAsync(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);

#endif // end of defensive include
