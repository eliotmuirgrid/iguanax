//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCredentials
//
// Description:
//
// testCredentials - helper functions for credentials used in unit tests
//
// Author: Nicolas Lehman
// Date:   Thu 22 Apr 2021
//---------------------------------------------------------------------------

#include "testCredentials.h"

#include <COL/COLstring.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

//MSSQL :1433 db = master user = sa pass = 49Psi7LL6#
//DB2 :50000 db = testdb user = db2inst1 pass = 49Psi7LL6#
//Mysql :3306 user = test pass 49Psi7LL6#                   // JULIAN db = test (???)
//Postgres :5432 user = postgres pass 49Psi7LL6#
//Oracle :1521 :55000  user = sys  pass  = Oradoc_db1

static void GetLocalMySql(COLvar* pCredentials) {
   COL_FUNCTION(GetLocalMySql);
   COLvar Credentials;
   Credentials["user"]     = "root";
   Credentials["password"] = "49Psi7LL6#";
   Credentials["host"]     = "localhost";
   Credentials["port"]     = 3306;

   *pCredentials = Credentials;
   COL_VAR(*pCredentials);
}

static void GetLocalMariaDb(COLvar* pCredentials) {
   COL_FUNCTION(GetLocalMySql);
   COLvar Credentials;
   Credentials["user"]     = "unit_test_mariadb";    // JULIAN TODO create on dev machine
   Credentials["password"] = "49Psi7LL6#";
   Credentials["host"]     = "localhost";
   Credentials["port"]     = 3306;

   *pCredentials = Credentials;
   COL_VAR(*pCredentials);
}

static void GetLocalOracleOci(COLvar* pCredentials) {
   COL_FUNCTION(GetLocalMySql);
   COLvar Credentials;
   // Credentials["user"]     = "unit_test_oracle"; // create on dev machine
   // Credentials["password"] = "49Psi7LL6#";
   // Credentials["host"]     = "oraclexe";         // set in hosts file on dev machine
   // Credentials["port"]     = 1521;

   // JULIAN local setup
   // Credentials["user"]     = "sys";
   Credentials["user"]        = "system";
   Credentials["password"]    = "test";
   // Credentials["password"] = "49Psi7LL6#";
   // Credentials["host"]     = "//10.211.55.28:1521/xe";       // FAIL invalid user/pwd
   // Credentials["host"]     = "orcl-xe";                  // FAIL invalid user/pwd
   Credentials["host"]        = "(DESCRIPTION=(ADDRESS=(PROTOCOL=TCP) (HOST=10.211.55.28) (PORT=1521))  (CONNECT_DATA=(SID=XE)))";
   // Credentials["host"]     = "(DESCRIPTION=(ADDRESS=(PROTOCOL=TCP) (HOST=10.211.55.28) (PORT=1521)) (CONNECT_DATA=(SERVICE_NAME=XE)))";
   //                            "(description=(address=(host=oracle64)(protocol=tcp)(port=1521))(connect_data=(sid=ORCL)))",
   //                            "(description=(address=(host=oraclexe)(protocol=tcp)(port=1521))(connect_data=(sid=XE)))",

   // Credentials["port"]     = 1521; // value never used with Oracle OCI

   *pCredentials = Credentials;
   COL_VAR(*pCredentials);
}

void GetCredentials(const COLstring& DbServerType, COLvar* pCredentials, bool UseLocal) {
   COL_FUNCTION(GetCredentials);
   
   if (UseLocal) {
      COL_TRC("Fetching credentials for a local database server...");
      if (DbServerType.equals("MySQL")) {
         GetLocalMySql(pCredentials);
      }else if (DbServerType.equals("MariaDB")) {
         GetLocalMariaDb(pCredentials);
      }else if (DbServerType.equals("OracleOci")) {
         GetLocalOracleOci(pCredentials);
      }   
   }
}
