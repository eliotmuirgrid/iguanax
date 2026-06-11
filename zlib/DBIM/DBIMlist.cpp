// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMlist
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Wednesday 08 March 2023 - 09:00AM
// ---------------------------------------------------------------------------

#include <DBIM/DBIMlist.h>
#include <DB/DBdatabase.h>
#include <DB/DBdatabaseFactory.h>
#include <COL/COLhashmap.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

// Removes the older ODBC databases as we don't support importing/exporting from them
static void DBIMfilterList(COLarray<COLstring>& Databases, COLvar& Result){
   COL_FUNCTION(DBIMfilterList);
   COLhashmap<COLstring, bool> SupportedDbs;
   SupportedDbs[DBdatabase::DB_DATABASE_ODBC_POSTGRESQL_NAME] = true;
   SupportedDbs[DBdatabase::DB_DATABASE_ODBC_MYSQL_NAME] = true;
   SupportedDbs[DBdatabase::DB_DATABASE_ODBC_ORACLE_NAME] = true;
   SupportedDbs[DBdatabase::DB_DATABASE_SQL_SERVER_NAME] = true;
   SupportedDbs[DBdatabase::DB_DATABASE_OCI_ORACLE_NAME] = true;
   SupportedDbs[DBdatabase::DB_DATABASE_MY_SQL_NAME] = true;
   SupportedDbs[DBdatabase::DB_DATABASE_MARIA_DB_NAME] = true;
   SupportedDbs[DBdatabase::DB_DATABASE_SQLITE_NAME] = true;
   for(int i = 0; i < Databases.size(); i++){
      if (SupportedDbs.count(Databases[i])) { Result.push_back(Databases[i]); }
   }
}

// /database/list
void DBIMlist(const COLwebRequest& Request, DBdatabaseFactory* pDatabaseFactory){
   COL_FUNCTION(DBIMlist);
   COLvar Result;
   Result.setArrayType();
   COLarray<COLstring> Databases = pDatabaseFactory->listDatabases();
   DBIMfilterList(Databases, Result);
   return COLrespondSuccess(Request.Address, Result);
}