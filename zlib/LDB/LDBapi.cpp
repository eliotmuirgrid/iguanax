//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBdatebase
//
// Description:
//
// Lua db API implementation
//
// Author: Eliot Muir
// Date: Thu May 13 2021
//---------------------------------------------------------------------------
#include <LDB/LDBapi.h>
#include <LDB/LDBbegin.h>
#include <LDB/LDBclose.h>
#include <LDB/LDBcommit.h>
#include <LDB/LDBconnect.h>
#include <LDB/LDBconnectionCheck.h>
#include <LDB/LDBinfo.h>
#include <LDB/LDBmerge.h>
#include <LDB/LDBquery.h>
#include <LDB/LDBquote.h>
#include <LDB/LDBrollback.h>
#include <LDB/LDBtables.h>
#include <LUA/LUAutils.h>
#include <LUAL/LUALutils.h>
#include <NODL/NODLdbConnection.h>
#include <NODL/NODLconstants.h>
#include <NODL/NODLenvironment.h>
#include <COL/COLstring.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void LDBaddDatabaseConnectionCount(lua_State* L){
   COL_FUNCTION(LDBaddDatabaseConnectionCount);
   LUApushString(L, NODL_DB_CONNECTION_COUNT);
   lua_pushinteger(L, 0);
   lua_settable(L, LUA_REGISTRYINDEX);
}

static void LDBregisterMetatable(lua_State* L){
   COL_FUNCTION(LDBregisterMetatable);
   luaL_newmetatable(L, NODL_DB_CONNECTION_METATABLE);
   LUAaddMethod(L, "__gc",       &NODLdbConnectionDestructor);
   LUAaddMethod(L, "__tostring", &NODLdbConnectionToString);
   lua_newtable(L); //setting up __index
   LUAaddMethod(L, "query",               &LDBquery);
   LUAaddMethod(L, "merge",               &LDBmerge);
   LUAaddMethod(L, "execute",             &LDBexecute);
   LUAaddMethod(L, "close",               &LDBclose);
   LUAaddMethod(L, "begin",               &LDBbeginTransaction);
   LUAaddMethod(L, "commit",              &LDBcommitTransaction);
   LUAaddMethod(L, "rollback",            &LDBrollbackTransaction);
   LUAaddMethod(L, "quote",               &LDBquote);
   LUAaddMethod(L, "info",                &LDBconnectionInfo);
   LUAaddMethod(L, "check",               &LDBconnectionCheck);
   lua_setfield(L,-2, "__index");
   lua_pop(L, 1);
   LUALaddSubMetatable(L, NODL_DB_CONNECTION_METATABLE, "db_connection");
}

void LDBaddDatabaseNamespace(lua_State* L) {
   COL_FUNCTION(LDBaddDatabaseNamespace);
   lua_newtable(L);  // "db" Namespace
   LUAaddMethod(L, "connect",  &LDBconnect);
   LUAaddMethod(L, "tables",   &LDBloadSchemaTables);
   NODLaddDbApiConstants(L);
   LDBregisterMetatable(L);
   lua_setglobal(L, "db");
}
