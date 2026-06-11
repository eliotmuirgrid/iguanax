#ifndef __SDB_INIT_H__
#define __SDB_INIT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBinit
//
// Description:
//
// Initialization functions for SDBsystem
//
// Author: John Qi
// Date:   Tuesday 04 April 2023 - 02:27PM
// ---------------------------------------------------------------------------

#include <COL/COLclosure6.h>
#include <COL/COLhashmap.h>
#include <SDB/SDBid.h>

class SDBsystem;

// NOTE: SDBstartSystem() must be call after component configs are fully loaded in to the CFGmap object that pConfigMap points to!
void SDBstartSystem(SDBsystem* pSystem, COLclosure0* pDone);
void SDBstopSystem (SDBsystem* pSystem, COLclosure0* pDone);


// unit test
void SDBlogRecovered(int Code, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap, SDBsystem* pSystem, COLclosure0* pDone);

#endif // end of defensive include
