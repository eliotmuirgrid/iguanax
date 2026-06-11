//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseSqlite
//
// Description:
//
// Implementation
//
// Author: Julian Muir
// Date:   April 23 2021 
//
//---------------------------------------------------------------------------

#include "DBminimumInclude.h"
#include "DBdriverFind.h"
#include "DBdriverValidate.h"
#include "DBdatabaseSqlite.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

// JULIAN just a skeleton for now

// Load from config settings does not search for libraries - used for Iguana startup
// JULIAN TODO - this is a straight paste of MySQL code need to modify for Oracle
bool DBdatabaseSqlite::loadConfig(COLostream& LogStream, const COLstring& FileName){
   COL_FUNCTION(DBdatabaseSqlite::loadConfig);

//    COL_TRC("MySQL library not yet loaded.");
//    LogStream << COL_T("Trying to load MySQL compliant library... ") << newline;

//    if (!DBmysqlValidate(LogStream, FileName)){
//       COL_TRC("Library load failed - the file specified is not a valid MySQL library.");
//       LogStream << COL_T("Library load failed - the file specified is not a valid MySQL library.") << newline;
//       pLoadedMySqlDll = NULL;
//       return false;
//    }else{
//       COL_TRC("OK (mysql enabled)");
//       LogStream << COL_T(" OK (mysql compliant library enabled)") << newline;
//       COLdll Dll;
//       Dll.tryLoadLibrary(FileName);
//       pLoadedMySqlDll = new DBmySqlDll(Dll);
//    }   
// return (pLoadedMySqlDll != NULL);
return(false);          // JULIAN this is a dummy return so it compiles on Windows
}
