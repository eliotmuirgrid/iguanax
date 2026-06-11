// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PRMapi
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Monday 29 January 2024 - 07:51AM
// ---------------------------------------------------------------------------
#include <PRM/PRMapi.h>
#include <PRM/PRMcomponent.h>
#include <PRM/PRMcomponentAll.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


 void PRMapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, SCKloop* pLoop, CFGmap* pMap) {
    COL_FUNCTION(PRMapi);
    ApiMap.add("/component/permission",     COLnewClosure1(&PRMcomponent));
    ApiMap.add("/component/permission/all", COLnewClosure1(&PRMcomponentAll, pMap));
 }
