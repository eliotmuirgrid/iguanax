// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LOGGapi
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Wednesday 20 September 2023 - 03:24PM
// ---------------------------------------------------------------------------

#include <LOGG/LOGGapi.h>
#include <LOGG/LOGGgenerator.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void LOGGeneratorApi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SDBsystem* pLogQueue, SCKloop* pLoop){
   COL_FUNCTION(LOGGeneratorApi);
   ApiMap.add("/log_generator/start",  COLnewClosure1(&LOGGgeneratorStart, pLogQueue, pLoop));
   ApiMap.add("/log_generator/status", COLnewClosure1(&LOGGgeneratorStatus));
}
