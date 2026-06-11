// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBsetup
//
// Description:
//
// Implementation
//
// Author: Julian Muir (ported by Eliot to here)
// Date:   Sunday 18 September 2022 - 01:12PM
// ---------------------------------------------------------------------------

#include <IDB/IDBsetup.h>
#include <IDB/IDBdatabaseConfig.h>

#include <DB/DBdatabaseFactory.h>
#include <DB/DBdatabaseMySql.h>
#include <DB/DBdatabaseOciOracle.h>
#include <DB/DBdatabaseOdbc.h>
#include <DB/DBdriverFind.h>

#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>  // TODO should logging happen here?
#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>
#include <DIR/DIRpath.h>

#include <COL/COLclosure.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void IDBsearchForMissingDrivers(IDBdatabaseConfig* pDatabaseConfig, COLostream& Logstream){
   COL_FUNCTION(IDBsearchForMissingDrivers);
   if(pDatabaseConfig->mySql().preferredMySqlDll().is_null()){
      COL_TRC("MySql compliant driver not set, will try to find a suitable file");
      COLstring MySql;
      if(DBfindMariaDb(&MySql, Logstream)){ // prioritize maria db as it replaces mysql
         COL_TRC("Found MariaDB driver, setting as preferred driver.");
         pDatabaseConfig->mySql().setPreferredMySqlDll(MySql);
      } else if (DBfindMySql(&MySql, Logstream)){
         COL_TRC("Found MySql driver, setting as preferred driver.");
         pDatabaseConfig->mySql().setPreferredMySqlDll(MySql);
      } else {
         COL_TRC("No MySql or MariaDB support found.");
      }
   }
   if(pDatabaseConfig->oracle().preferredOracleDll().is_null()){
      COL_TRC("Oracle driver not set, will try to find a suitable file");
      COLstring Oracle;
      if(DBfindOracleOci(&Oracle, Logstream)){
         COL_TRC("Found Oracle driver, setting as preferred driver.");
         pDatabaseConfig->oracle().setPreferredOracleDll(Oracle);
      } else {
         COL_TRC("No Oracle support found.");
      }
   }
   if(pDatabaseConfig->odbc().preferredOdbcDll().is_null()){
      COL_TRC("ODBC driver not set, will try to find a suitable file");
      COLstring Odbc;
      if(DBfindOdbc(&Odbc, Logstream)){
         COL_TRC("Found ODBC driver, setting as preferred driver.");
         pDatabaseConfig->odbc().setPreferredOdbcDll(Odbc);
      } else {
         COL_TRC("No ODBC support found.");
      }
   }
}

static void IDBinitializeDatabases(const COLstring& PreferredMySqlLibPath, const COLstring& PreferredOracleLibPath, const COLstring& PreferredOdbcLibPath, COLostream& LogStream){
   COL_FUNCTION(IDBinitializeDatabases);
   if (!PreferredMySqlLibPath.is_null()){
      COL_TRC("Loading mysql driver.");
      bool MySqlInit = DBdatabaseMySql::initialize(LogStream, PreferredMySqlLibPath.c_str());
      COL_VAR(MySqlInit);
   }
   if (!PreferredOracleLibPath.is_null()){
      COL_TRC("Loading Oracle Driver");
      bool OracleInit = DBdatabaseOciOracle::initialize(LogStream, PreferredOracleLibPath.c_str());
      COL_VAR(OracleInit);
   }
   if (!PreferredOdbcLibPath.is_null()){
      COL_TRC("Loading Odbc Driver");
      bool OdbcInit = DBdatabaseOdbc::initialize(LogStream, PreferredOdbcLibPath.c_str());
      COL_VAR(OdbcInit);
   }
}

static void IDBlogOnMain(COLstring Log) {
   COL_FUNCTION(IDBlogOnMain);
   if ( !Log.is_null() ) {
     SDBIlogService("Loading database interfaces specified in configuration file.\n" + Log, "#startup");
   }
   SDBIlogService("The Database system started successfully.", "#startup");
}

void IDBsetup(IDBdatabaseConfig* pDatabaseConfig, DBdatabaseFactory* pDatabaseFactory, SCKloop* pLoop) {
   COL_FUNCTION(IDBsetup);
   COL_TRC("Discovering drivers...");
   bool ReadSuccess = IDBreadConfigFile(pDatabaseConfig);
   COL_VAR(ReadSuccess);
   COLstring Log; // TODO: do we want to make this visible to the user somehow?
   COLostream LogStream(Log);
   if (!ReadSuccess && !FILfileExists(DIRfileConfigDatabase())){
      COL_TRC("Could not find configuration file. Will search for drivers and setting up default configuration.");
   } else {
      COL_TRC("Found configuration file. Will search for any missing drivers.");
   }
   IDBsearchForMissingDrivers(pDatabaseConfig, LogStream);
   IDBsaveConfigFile(pDatabaseConfig);
   COL_TRC("Using drivers:");
   COL_VAR3(pDatabaseConfig->mySql().preferredMySqlDll(), pDatabaseConfig->oracle().preferredOracleDll(), pDatabaseConfig->odbc().preferredOdbcDll());
   const COLstring PreferredMySqlLibPath   = FILpathResolveEnvironment(pDatabaseConfig->mySql().preferredMySqlDll());
   const COLstring PreferredOracleLibPath  = FILpathResolveEnvironment(pDatabaseConfig->oracle().preferredOracleDll());
   const COLstring PreferredOdbcLibPath    = FILpathResolveEnvironment(pDatabaseConfig->odbc().preferredOdbcDll());
   COL_TRC("Initializing drivers");
   IDBinitializeDatabases(PreferredMySqlLibPath, PreferredOracleLibPath, PreferredOdbcLibPath, LogStream);
   COL_TRC("Database drivers loaded if found - now initializing database factory");
   pDatabaseFactory->initialize(LogStream);
   pLoop->post(COLnewClosure0(&IDBlogOnMain, Log));   // TODO should logging happen here?
}
