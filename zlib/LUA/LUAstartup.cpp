// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAstartup
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 27 March 2025 - 11:42AM
// ---------------------------------------------------------------------------

#include <LUA/LUAstartup.h>
#include <LUA/LUAheader.h>
#include <LUA/LUAutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

const COLstring LUAL_STARTUP_ENVIRONMENT_REGISTRY_NAME = "StartupEnvironment";

bool LUALisEnvironmentStartup(lua_State* L){
   COL_FUNCTION(LUALisEnvironmentStartup);
   LUApushString(L, LUAL_STARTUP_ENVIRONMENT_REGISTRY_NAME);
   lua_gettable(L, LUA_REGISTRYINDEX);
   bool IsStartup = lua_isboolean(L, -1) && lua_toboolean(L, -1);
   lua_pop (L, 1);
   return IsStartup;
}

void LUALmarkEnvironmentAsStartup(lua_State* L){
   COL_FUNCTION(LUALmarkEnvironmentAsStartup);
   LUApushString(L, LUAL_STARTUP_ENVIRONMENT_REGISTRY_NAME);
   lua_pushboolean(L, true);
   lua_settable(L, LUA_REGISTRYINDEX);
}
