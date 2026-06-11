// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODLconstants
//
// Description:
//
// A series of strings and arrays to be used throughout NODL
//
// Author: Vismay Shah 
// Date:   Thursday 1 December 2022
// ---------------------------------------------------------------------------
#include "NODLconstants.h"
#include <COL/COLerror.h>
#include <COL/COLlog.h>
#include <LUA/LUAerror.h>
#include <LUAL/LUALutils.h>
#include <LUA/LUAutils.h>
COL_LOG_MODULE;

static int NODLdbRecordsetArrayToString(lua_State* L){
   COL_FUNCTION(NODLdbRecordsetArrayToString);
   try {
      int n_elements = lua_objlen (L, lua_gettop(L));
      char buffer [32];
      snprintf (buffer, sizeof(buffer), "%d Result Set%s", n_elements, (n_elements > 1)?"s":"");
      lua_pushstring(L, buffer);
      int top = lua_gettop(L);
   } catch(const COLerror& Error) {
      LUAraiseLuaError(L, Error.description());
   }
   return 1;
}

static int NODLdbErrorTableToPrettyString(lua_State* L){
   COL_FUNCTION(NODLdbErrorTableToPrettyString);
   try {
      lua_pushstring(L, "message");
      lua_gettable(L, -2);
      COLstring Error = lua_tostring (L, lua_gettop(L));
      lua_pop (L, 1);
      lua_pushstring(L, "code");
      lua_gettable(L, -2);
      long ErrorCode = lua_tointeger(L, -1);
      lua_pop (L, 1);
      char buffer[64];
      snprintf (buffer, sizeof(buffer), "Error (%ld)\n", ErrorCode);
      Error.prepend(buffer);
      lua_pushstring(L, Error.c_str());
   } catch(const COLerror& Error) {
      LUAraiseLuaError(L, Error.description());
   }
   return 1;
}



void NODLaddDbApiConstants(lua_State* L){
   COL_FUNCTION(NODLaddDbApiConstants);
   LUAaddConstant(L, "MY_SQL",      1001);
   LUAaddConstant(L, "POSTGRES",    1002);
   LUAaddConstant(L, "SYBASE_ASA",  1003);
   LUAaddConstant(L, "SYBASE_ASE",  1004);
   LUAaddConstant(L, "ACCESS",      1005);
   LUAaddConstant(L, "SQL_SERVER",  1006);
   LUAaddConstant(L, "DB2",         1007);
   LUAaddConstant(L, "INFORMIX",    1008);
   LUAaddConstant(L, "INTERBASE",   1009);
   LUAaddConstant(L, "FILEMAKER",   1010);
   LUAaddConstant(L, "ORACLE_OCI",  1011);
   LUAaddConstant(L, "ORACLE_ODBC", 1012);
   LUAaddConstant(L, "SQLITE",      1013);
   LUAaddConstant(L, "MARIA_DB",    1014);
   LUAaddConstant(L, "MY_SQL_ODBC", 1015);

   if (luaL_newmetatable(L, NODL_DB_RECORDSET_ARRAY_METATABLE)){
      lua_pushcfunction(L, NODLdbRecordsetArrayToString);
      lua_setfield(L, -2, "__tostring");
   }
   lua_setfield(L, LUA_REGISTRYINDEX, NODL_DB_RECORDSET_ARRAY_METATABLE);

   if (luaL_newmetatable(L, NODL_DB_ERROR_TABLE_TO_STRING)){
      lua_pushcfunction(L, NODLdbErrorTableToPrettyString);
      lua_setfield(L, -2, "__tostring");
   }
   lua_setfield(L, LUA_REGISTRYINDEX, NODL_DB_ERROR_TABLE_TO_STRING);
}
