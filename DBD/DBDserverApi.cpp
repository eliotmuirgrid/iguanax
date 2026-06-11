// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDserverApi
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday 21 September 2022 - 06:03PM
// ---------------------------------------------------------------------------
#include <DBD/DBDserverApi.h>

#include <DBD/DBDserverHeader.h>
#include <DBD/DBDserverHealth.h>
#include <DBD/DBDserverOs.h>
#include <DBD/DBDsourceHash.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void DBDserverApi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, IWEBconfig* pWebConfig){
   COL_FUNCTION(DBDserverApi);
   ApiMap.add("/header", COLnewClosure1(&DBDserverHeader, pWebConfig));
   ApiMap.add("/os",     COLnewClosure1(&DBDserverOs));
   ApiMap.add("/hash",   COLnewClosure1(&DBDsourceHash));
   ApiMap.add("/health", COLnewClosure1(&DBDserverHealth));
}
