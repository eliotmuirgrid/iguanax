#ifndef __SDBT_POSITION_MAP_H__
#define __SDBT_POSITION_MAP_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBTpositionMap
//
// Description:
//
// Unit test helper functions for SDBpositionMap
//
// Author: John Qi
// Date:   Friday 31 March 2023 - 01:00PM
// ---------------------------------------------------------------------------

#include <SDB/SDBpositionMap.h>
#include <COL/COLhashmap.h>

#include <time.h>

void SDBTcreateTestPositionMapData(COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pOut, int Components = 10, int Index = 1, time_t Date = time(NULL));
SDBpositionMap SDBTcreateTestPositionMap(int Components = 10, int Index = 1, time_t Date = time(NULL));
COLstring SDBTcreateSerializedTestPositionMap(const SDBpositionMap& Map);
COLstring SDBTcreateSerializedTestPositionMap(int Components = 10, int Index = 1, time_t Date = time(NULL));

#endif // end of defensive include
