//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testMySql
//
// Description:
//
// testMySql - tests basic functionality with MySQL
//             Credentials can be found in testCredentials.cpp/h
//
// Author: Nicolas Lehman
// Date:   Thu 22 Apr 2021
//---------------------------------------------------------------------------

#include "testMySql.h"
#include "testCredentials.h"

#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLstring.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <DB/DBdriverFind.h>
#include <DB/DBdriverValidate.h>
#include <DB/DBdriverMySql.h>
#include <DB/DBmySql.h>

#include <UNIT/UNITapp.h>

static COLstring FindMySqlLibPath() {
   COL_FUNCTION(FindMySqlLibPath);
   COLstring Log, Path;
   COLostream LogStream(Log);
   bool Success = DBfindMySql(&Path, LogStream);

   COL_VAR3(Log, Path, Success);
   if (Success) {
      return Path;
   }
   return "";
}

static void testLoadDll() {
   COL_FUNCTION(testLoadDll);
   COLstring LibraryPath = FindMySqlLibPath();
   DBdriverMySql Driver;
   Driver.loadLibrary(LibraryPath);
   COL_VAR(Driver.isLoaded());
   if (!Driver.isLoaded()){
      COL_TRC("Could not load MYSQL drivers");
      return;
   }else{COL_TRC("Loaded MYSQL drivers");}

   MYSQL* MySql;   
   MySql = Driver.mysql_init(NULL);
   if (!MySql) {
      COL_TRC("Could not initialize MYSQL object");
   } else {
      COL_TRC("Initialized MYSQL object");
   }
}

static void testConnect() {
   COL_FUNCTION(testConnect);
   COLstring LibraryPath = FindMySqlLibPath();  

   DBmySql Database;
   if (!Database.setDriver(LibraryPath)) {
      COL_TRC("Setting driver failed");
      return;
   }

   COLvar ConnectionInfo;
   GetCredentials("MySQL", &ConnectionInfo, true);
   Database.connect(ConnectionInfo);
   COL_VAR(Database.isConnected());
   Database.disconnect();
}

static void testQuery() {
   COL_FUNCTION(testQuery);
   COLstring LibraryPath = FindMySqlLibPath();  

   DBmySql Database;
   if (!Database.setDriver(LibraryPath)) {
      COL_TRC("Setting driver failed");
      return;
   }

   COLvar ConnectionInfo;
   GetCredentials("MySQL", &ConnectionInfo, true);
   ConnectionInfo["database"] = "employees";
   ConnectionInfo["database"] = "test";          // JULIAN REMOVE
   Database.connect(ConnectionInfo);
   COLvar ResultSet;
   COLvar ResultString;
   bool Success = Database.executeSqlString("SELECT * FROM titles LIMIT 10;", &ResultSet, &ResultString);
   COL_VAR2(Success, ResultSet.json(1));
   Database.disconnect();
}

static void Logger(COLstring LogText) {
   COL_FUNCTION(Logger);
   COL_TRC(LogText);
}

static void testLogger() {
   COL_FUNCTION(testLogger);
   COLstring LibraryPath = FindMySqlLibPath();  

   DBmySql Database;



   if (!Database.setDriver(LibraryPath)) {
      COL_TRC("Setting driver failed");
      return;
   }

   COLclosure1<COLstring>* Callback = COLnewClosure1<COLstring>(&Logger);
   Database.setLogger(Callback);

   COLvar ConnectionInfo;
   GetCredentials("MySQL", &ConnectionInfo, true);
   ConnectionInfo["database"] = "employees";
   Database.connect(ConnectionInfo);
   Database.disconnect();
}

void testMySql(UNITapp* pApp) {
   // JULIAN these work - commented out for testing Oracle OCI
   // pApp->add("mysql/loadDll", &testLoadDll);
   // pApp->add("mysql/connect", &testConnect);
   // pApp->add("mysql/query", &testQuery);
}