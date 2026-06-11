//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DASHapi.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   19/06/25 2:18 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <DASH/DASHapi.h>
#include <DASH/DASHstatus.h>
#include <DASH/DASHlistData.h>
#include <DASH/DASHmapData.h>
#include <DASH/DASHcomponentDetail.h>
COL_LOG_MODULE;

void DASHapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, SCKloop* pLoop, CFGmap* pMap) {
   COL_FUNCTION(DASHapi);
   ApiMap.add("/dashboard/status", COLnewClosure1(&DASHstatus, pMap, pLoop));
   ApiMap.add("/component/list",   COLnewClosure1(&DASHlistData, pMap));
   ApiMap.add("/component/map",    COLnewClosure1(&DASHmapData, pMap));

   ApiMap.add("/component/detail", COLnewClosure1(&DASHcomponentDetail, pMap));
}
