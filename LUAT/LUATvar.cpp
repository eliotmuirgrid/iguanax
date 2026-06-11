
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUATvar
//
// Description:
//
// Implementation
//
// Author: Jonathan Marshall
// Date:   Feb 3 2016
//---------------------------------------------------------------------------
#include "LUATprecomp.h"
#pragma hdrstop

#include <LUA/LUAheader.h>
#include <LUA/LUAvar.h>
#include "LUATvar.h"

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static lua_State* NODLTgetL() {
   lua_State* L = lua_open();
   luaL_openlibs(L);
   return L;
}

static lua_State* buildSimpleLuaTable() {
   lua_State* L = NODLTgetL();
   lua_newtable(L);
   lua_pushstring(L, "value1");
   lua_setfield(L, -2, "key1");
   lua_pushstring(L, "value2");
   lua_setfield(L, -2, "key2");
   return L;
}

static void testLuaToVar() {
   COL_FUNCTION(LUATluaToVar);
   lua_State* L = buildSimpleLuaTable();
   lua_newtable(L);
   lua_setfield(L, -2, "nested_table");
   lua_getfield(L, -1, "nested_table");
   lua_pushstring(L, "nested_value1");
   lua_setfield(L, -2, "nested_key1");
   lua_pushstring(L, "nested_value2");
   lua_setfield(L, -2, "nested_key2");
   lua_pop(L, 1);
   COLvar Var;
   Var.fromLua(L);
   Var.fromLua(L);
   COLstring JsonStr("{\"key1\":\"value1\",\"key2\":\"value2\",\"nested_table\":{\"nested_key1\":\"nested_value1\",\"nested_key2\":\"nested_value2\"}}");
   COL_VAR2(Var, JsonStr);
   TST_ASSERT_EQUALS(JsonStr, Var.json());
   lua_close(L);
}

static void testLuaToVarCyclicFails() {
   COL_FUNCTION(testLuaToVarCyclicFails);
   lua_State* L = buildSimpleLuaTable();
   lua_newtable(L);
   lua_setfield(L, -2, "nested_table");
   lua_getfield(L, -1, "nested_table");
   lua_pushvalue(L, 1);
   lua_setfield(L, -2, "cyclic_key");
   lua_pop(L, 1);
   COLvar Var;
   LUA::LUAconversionResult Result = LUAtoVar(L, lua_gettop(L), &Var);
   TST_ASSERT_EQUALS(LUA::ERROR_LUA_CYCLIC_TABLE, Result);
   COL_VAR(Var);
   lua_close(L);
}

TSTtestable* LUATluaToVar() {
   TST_START_COLLECTION("LUATvar")
      TST_ADD_TEST_FUNCTION(testLuaToVar);
      TST_ADD_TEST_FUNCTION(testLuaToVarCyclicFails);
   TST_END_FIXTURE
};

