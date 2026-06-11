#ifndef __SDBA_API_H__
#define __SDBA_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBAapi
//
// Description:
//
// WEB API for logging system.
//
// Author: Eliot Muir
// Date:   Friday 05 May 2023 - 03:46PM
// ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <SDB/SDBpurge.h>
#include <COL/COLweb.h>

class BLUAinstanceManager;
class SDBAsearchState;
class SCKloop;
class SDBsystem;

struct CFGmap;

void SDBAworker(COLhashmap<COLstring, COLwebHandlerFunc>& WorkerThreadMap);

void SDBAblua(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, SCKloop* pLoop, CFGmap* pMap,
              BLUAinstanceManager* pManager, SDBsystem* pSystem);

void SDBAlog(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, SDBsystem* pSystem,
             SCKloop* pLoop);

#endif // end of defensive include
