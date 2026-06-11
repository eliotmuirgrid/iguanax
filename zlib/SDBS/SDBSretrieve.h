#ifndef _SDBSRETRIEVE_H
#define _SDBSRETRIEVE_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBSretrieve.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-20, 12:55p.m.
//  ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
class COLwebRequest;
class SDBSretrieveState;
class SDBUparams;
class SCKloop;

void SDBSretrieve(const COLwebRequest& Request, COLhashmap<COLstring, SDBSretrieveState>* pStateMap, SCKloop* pLoop);
void SDBSretrieveResults(const COLwebRequest& Request, COLhashmap<COLstring, SDBSretrieveState>* pStateMap);

#endif  // end of defensive include
