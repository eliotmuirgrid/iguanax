// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBSapi
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 08 November 2022 - 08:48AM
// ---------------------------------------------------------------------------

#include <DBS/DBSapi.h>
#include <DBS/DBScomponentLoad.h>  
#include <DBS/DBScomponentSet.h>  

#include <COL/COLlog.h>
COL_LOG_MODULE;

void DBSapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(DBSapi);
   ApiMap.add("/component/db/set",  COLnewClosure1(&DBScomponentSet, pMap, pLoop));
   ApiMap.add("/component/db/load", COLnewClosure1(&DBScomponentLoad, pMap, pLoop));
}
