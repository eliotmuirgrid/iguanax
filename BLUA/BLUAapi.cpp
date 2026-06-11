//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAapi
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Friday May 14 2021
//---------------------------------------------------------------------------
#include "BLUAapi.h"

#include <BLUA/BLUAaddSource.h>
#include <BLUA/BLUAcomponentFunctionCall.h>
#include <BLUA/BLUAcomponentClearError.h>
#include <BLUA/BLUAdelete.h>
#include <BLUA/BLUAremoveSource.h>
#include <BLUA/BLUArunList.h>
#include <BLUA/BLUAstart.h>
#include <BLUA/BLUAstop.h>
#include <BLUA/BLUAstopAll.h>

#include <CFG/CFGmap.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void BLUAapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, CFGmap* pMap, BLUAinstanceManager* pBluaManager) {
   COL_FUNCTION(BLUAapi);
   // TODO - revisit component overloaded space.
   ApiMap.add("/component/run_list",      COLnewClosure1(&BLUArunList,               pBluaManager));
   ApiMap.add("/component/delete",        COLnewClosure1(&BLUAdeleteComponent,       pBluaManager, pMap));
   ApiMap.add("/component/start",         COLnewClosure1(&BLUAcomponentStart,        pBluaManager, pMap, pLoop));
   ApiMap.add("/component/start/check",   COLnewClosure1(&BLUAcomponentStartCheck));
   ApiMap.add("/component/stop",          COLnewClosure1(&BLUAcomponentStop,         pBluaManager, pMap, pLoop));
   ApiMap.add("/component/stop/check",    COLnewClosure1(&BLUAcomponentStopCheck));
   ApiMap.add("/component/stop_all",      COLnewClosure1(&BLUAcomponentStopAll,      pBluaManager));
   ApiMap.add("/component/clear_error",   COLnewClosure1(&BLUAcomponentClearError,   pBluaManager, pMap));
   ApiMap.add("/component/source/add",    COLnewClosure1(&BLUAaddComponentSource,    pBluaManager, pMap, pLoop));
   ApiMap.add("/component/source/delete", COLnewClosure1(&BLUAremoveComponentSource, pBluaManager, pMap, pLoop));
   ApiMap.add("/component/function/call", COLnewClosure1(&BLUAcomponentFunctionCall, pBluaManager, pMap, pLoop));
}