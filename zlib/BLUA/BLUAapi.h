#ifndef __BLUA_API_H__
#define __BLUA_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAapi
//
// Description:
//    APIs for start/stop components
//
// Author: Vismay Shah
// Date:   Friday May 14 2021
//---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

class  BLUAinstanceManager;
class  SCKloop;
struct CFGmap;

void BLUAapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, CFGmap* pMap, BLUAinstanceManager* pBluaManager);

#endif // end of defensive include
