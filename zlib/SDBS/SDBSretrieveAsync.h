#ifndef _SDBSRETRIEVEASYNC_H
#define _SDBSRETRIEVEASYNC_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBSretrieveAsync.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-20, 1:39p.m.
//  ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
class SDBUparams;
class SDBSretrieveState;
class SCKloop;

void SDBSretrieveAsync(const SDBUparams& Params, COLhashmap<COLstring, SDBSretrieveState>* pRetrieveStateMap,
                       SCKloop* pLoop);

#endif  // end of defensive include
