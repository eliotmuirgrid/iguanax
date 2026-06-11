#ifndef LIBAPI_H
#define LIBAPI_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LIBapi.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   09/06/25 2:32 PM
//  ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

class SCKloop;
struct CFGmap;

void LIBapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, CFGmap* pMap, SCKloop* pLoop);


#endif // end of defensive include
