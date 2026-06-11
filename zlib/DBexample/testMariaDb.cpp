//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testMariaDb
//
// Description:
//
// Implementation
//
// Author: Julian Muir
// Date:   03 May 2021
//---------------------------------------------------------------------------

#include "testMariaDb.h"
#include "testCredentials.h"

#include <COL/COLstring.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <DB/DBdriverFind.h>
#include <DB/DBdriverValidate.h>
#include <DB/DBdriverMariaDb.h>
#include <DB/DBmariaDb.h>

#include <UNIT/UNITapp.h>

static COLstring FindMariaDbLibPath() {
   COL_FUNCTION(FindMariaDbLibPath);
   COLstring Log, Path;
   COLostream LogStream(Log);
   bool Success = DBfindMariaDb(&Path, LogStream);

   COL_VAR3(Log, Path, Success);
   if (Success) {
      return Path;
   }
   return "";
}

static void testLoadDll() {
   COL_FUNCTION(testLoadDll);

   COLstring LibraryPath = FindMariaDbLibPath(); 
   DBdriverMariaDb Driver;
   Driver.loadLibrary(LibraryPath);
   COL_VAR(Driver.isLoaded());
   if (!Driver.isLoaded()){
      COL_TRC("Could not load MariaDB drivers");
      return;
   }else{COL_TRC("Loaded MariaDB drivers");}

   MARIADB* MariaDb; 
   MariaDb = Driver.mariadb_init(NULL);  
   if (!MariaDb) {
      COL_TRC("Could not initialize MARIADB object");
   } else { COL_TRC("Initialized MARIADB object"); }
}

static void testConnect() {
   COL_FUNCTION(testLoadDll);
   COLstring LibraryPath = FindMariaDbLibPath();  

   DBmariaDb Database;
   if (!Database.setDriver(LibraryPath)) {
      COL_TRC("Setting driver failed");
      return;
   }

   COLvar ConnectionInfo;
   GetCredentials("MariaDB", &ConnectionInfo, true);
   Database.connect(ConnectionInfo);
   COL_VAR(Database.isConnected());
   Database.disconnect();
}

void testMariaDb(UNITapp* pApp) {
   // pApp->add("mariadb/loadDll", &testLoadDll);
   // pApp->add("mariadb/connect", &testConnect);
}