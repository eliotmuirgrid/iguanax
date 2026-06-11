// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMapi
//
// Description:
//
// Implementation
//
// Author: Vismay
// Date:   Tuesday 10 January 2023 - 09:40AM
// ---------------------------------------------------------------------------
#include <DBIM/DBIMapi.h>
#include <DBIM/DBIMcheckSqlite.h>
#include <DBIM/DBIMexport.h>
#include <DBIM/DBIMgenerateSql.h>
#include <DBIM/DBIMlastExport.h>
#include <DBIM/DBIMlist.h>
#include <DBIM/DBIMread.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void DBIMapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, DBdatabaseFactory* pDatabaseFactory){
   COL_FUNCTION(DBIMapi);
   ApiMap.add("/database/list",                COLnewClosure1(&DBIMlist,                   pDatabaseFactory));
   ApiMap.add("/database/read",                COLnewClosure1(&DBIMread,            pLoop, pDatabaseFactory));
   ApiMap.add("/database/read_last_export",    COLnewClosure1(&DBIMreadLastExport,  pLoop                  ));
   ApiMap.add("/database/generate_sql",        COLnewClosure1(&DBIMgenerateSql,     pLoop, pDatabaseFactory));
   ApiMap.add("/database/export",              COLnewClosure1(&DBIMexport,          pLoop, pDatabaseFactory));
   ApiMap.add("/database/check_sqlite",        COLnewClosure1(&DBIMcheckSqlite,     pLoop                  ));
}
