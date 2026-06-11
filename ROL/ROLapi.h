#ifndef __ROLAPI_H__
#define __ROLAPI_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ROLapi.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   25/01/24 12:47 PM
//  ---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

class ROLmap;
class SCKloop;

void ROLapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, ROLmap* pRoleMap);

#endif // end of defensive include
