//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DBDmodeApi.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   5/2/23 9:55 AM
//  ---------------------------------------------------------------------------
#include <DBD/DBDmodeApi.h>
#include <DBD/DBDmode.h>
#include <DBD/DBDmodeNormalStart.h>
#include <DBD/DBDworld.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void DBDmodeApi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, DBDworld* pWorld) {
   COL_FUNCTION(DBDmodeApi);
   ApiMap.add("/mode",              COLnewClosure1(&DBDmodeWeb,         pWorld));
   ApiMap.add("/mode/normal/start", COLnewClosure1(&DBDmodeNormalStart, pWorld));
}
