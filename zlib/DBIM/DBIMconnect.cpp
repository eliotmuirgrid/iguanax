// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMconnect
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 16 January 2023 - 04:12PM
// ---------------------------------------------------------------------------
#include <DBIM/DBIMconnect.h>

#include <COL/COLlog.h>
#include <COL/COLstring.h>
#include <DBSQL/DBSQLdatabaseSqlite.h>
#include <FIL/FILutils.h>
COL_LOG_MODULE;

static bool DBIMcheckSqlitePath(const COLstring& Datasource, COLstring* pErr) {
   // Have to index FIL_DIR_SEPARATOR to allow this comparison to work
   if(Datasource.size() > 0 && Datasource[Datasource.size() - 1] == FIL_DIR_SEPARATOR[0]) { 
      COL_TRC("Datasource is not a file.");
      *pErr = Datasource + " must be a path to a file.";
      return false; 
   }
   return true;
}

bool DBIMconnect(const COLstring& Datasource, const COLstring& Username, const COLstring& Password, const COLstring& Location,
                DBdatabasePtr pDatabase, COLstring* pErr){
   COL_FUNCTION(DBIMconnect);
   COL_VAR3(Datasource,Username,Password);
   try {
      if(pDatabase->databaseType() == DB_SQLITE){
         if(!DBIMcheckSqlitePath(Datasource, pErr)) { return false; }
         COLstring DbFile = DBSQLresolve(Datasource, Location);
         COL_TRC("Connect to: " + DbFile);
         pDatabase->connect(DbFile.c_str(), Username.c_str(), Password.c_str());
      } else {
         COL_TRC("Connect to: " + Datasource);
         pDatabase->connect(Datasource.c_str(), Username.c_str(), Password.c_str());
      }
   } catch (const COLerror& Error){
      *pErr = Error.description();
      return false;
   }
   if (!pDatabase) {
      COL_TRC("Couldn't establish a connection.");
      *pErr = "Error: Could not connect to database. Verify that the connection parameters, database name and API are correct.";
      return false;
   }
   return true;
}

void DBIMdisconnect(DBdatabasePtr pDatabase){
   COL_FUNCTION(DBIMdisconnect);
   pDatabase->disconnect();
}
