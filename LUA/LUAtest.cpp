// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAtest
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 16 May 2023 - 04:59PM
// ---------------------------------------------------------------------------

#include <LUA/LUAtest.h>
#include <LUA/LUAheader.h>
#include <LUA/LUAutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

const COLstring LUAL_TEST_ENVIRONMENT_REGISTRY_NAME = "TestEnvironment";

bool LUALisEnvironmentTest(lua_State* L){
   COL_FUNCTION(LUALisEnvironmentTest);
   LUApushString(L, LUAL_TEST_ENVIRONMENT_REGISTRY_NAME);
   lua_gettable(L, LUA_REGISTRYINDEX);
   bool IsTest = lua_isboolean(L, -1) && lua_toboolean(L, -1);
   lua_pop (L, 1);
   return IsTest;
}

void LUALmarkEnvironmentAsTest(lua_State* L){
   COL_FUNCTION(LUALmarkEnvironmentAsTest);
   LUApushString(L, LUAL_TEST_ENVIRONMENT_REGISTRY_NAME);
   lua_pushboolean(L, true);
   lua_settable(L, LUA_REGISTRYINDEX);
}
