// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBget
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Wednesday 28 December 2022 - 12:09PM
// ---------------------------------------------------------------------------
#include <IDB/IDBget.h>
#include <IDB/IDBdatabaseConfig.h>

#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

// /settings/db/get
void IDBgetDriverPaths(const COLwebRequest& Request, IDBdatabaseConfig* pDatabaseConfig){
   COL_FUNCTION(IDBgetDriverPaths);
   COLvar Result;
   Result["mysql"]  = pDatabaseConfig->mySql().preferredMySqlDll();
   Result["oracle"] = pDatabaseConfig->oracle().preferredOracleDll();
   Result["odbc"]   = pDatabaseConfig->odbc().preferredOdbcDll();
   return COLrespondSuccess(Request.Address, Result);
}
