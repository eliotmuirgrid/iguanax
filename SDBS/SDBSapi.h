#ifndef _SDBSAPI_H
#define _SDBSAPI_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBSapi.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-20, 12:55p.m.
//  ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <SDB/SDBpurge.h>
#include <COL/COLweb.h>

class SDBSretrieveState;
class SDBSdetailSearchItem;
class SCKloop;

void SDBSlogSearch(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap,
                   COLhashmap<COLstring, SDBSretrieveState>* pLogSearchStateMap, SCKloop* pLoop);

void SDBSlogDetailSearch(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap,
                         COLhashmap<COLstring, SDBSdetailSearchItem>* pLogDetailSearchMap, SCKloop* pLoop);

#endif  // end of defensive include
