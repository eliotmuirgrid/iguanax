#ifndef __LOGG_API_H__
#define __LOGG_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LOGGapi
//
// Description:
//
// APIs for the log generator
//
// Author: Vismay Shah
// Date:   Wednesday 20 September 2023 - 03:24PM
// ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>
class SCKloop;
class SDBsystem;
void LOGGeneratorApi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SDBsystem* pLogQueue, SCKloop* pLoop);

#endif // end of defensive include
