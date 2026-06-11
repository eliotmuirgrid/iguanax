#ifndef __GIT_API_H__
#define __GIT_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITapi
//
// Description:
//
// GIT API functions - part of the new implementation
//
// Author: Eliot Muir 
// Date:   Saturday 25 March 2023 - 12:58PM
// ---------------------------------------------------------------------------
#include <COL/COLweb.h>
#include <COL/COLhashmap.h>

class SCKloop;
struct CFGmap;

void GITapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, SCKloop* pLoop, CFGmap* pMap);

void GITapiAsync(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);

#endif // end of defensive include
