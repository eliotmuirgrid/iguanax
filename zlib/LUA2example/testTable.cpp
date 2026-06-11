// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testTable.cpp
//
//  Description: Implementation
//
//  Author: Eliot Muir
//  Date:   Wed 31 Aug 2022 15:06:47 EDT
// ---------------------------------------------------------------------------
#include "testTable.h"

#include <UNIT/UNITapp.h>

#include <LUA/LUAtable.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


static void insertFunction(lua_State* L, lua_CFunction F, const char* pFuncName){
   COL_FUNCTION(insertFunction);
   lua_pushcfunction(L,F);
   lua_setglobal(L, pFuncName);
}

// TODO - No real error handling.
lua_State* testVariable(const char* pCodeFragment){
   COL_FUNCTION(testVariable);
   COL_VAR(pCodeFragment);
   lua_State* L = lua_open();
   COL_VAR(L);
   int Result = luaL_loadstring(L, pCodeFragment);
   COL_VAR(Result);
   Result = lua_pcall(L, 0, 0, 0);
   COL_VAR(Result);
   lua_getglobal(L, "V");
   COL_VAR(Result);
   COL_VAR(lua_gettop(L));
   return L;
}

int runCode(lua_State* L, const char* pCodeFragment){
   COL_FUNCTION(testCode);
   COL_VAR(pCodeFragment);
   int Result = luaL_loadstring(L, pCodeFragment);
   COL_VAR(Result);
   Result = lua_pcall(L, 0, 0, 0);
   return Result;
}

static bool getGlobalBool(lua_State* L){
   COL_FUNCTION(getGlobalBool);
   lua_getglobal(L, "Expected");
   int Expected = lua_toboolean(L, lua_gettop(L));
   COL_VAR2(Expected, (bool)Expected);
   return (bool)Expected;
}

static int boolTester(lua_State* L){
   COL_FUNCTION(boolTester);
   LUAparamIsTable(L, 1);
   bool Expected = getGlobalBool(L);
   COL_VAR(Expected);
   bool Result = LUAexpectedBoolParam(L, 1, "param");
   COL_VAR(Result);
   UNIT_ASSERT_EQUALS(Expected, Result);
   return 0;
}

static void testHelperBool(const char* Code, bool ExpectedValue){
   COL_FUNCTION(testHelperBool);
   lua_State* L = lua_open();
   lua_pushboolean(L, ExpectedValue ? 1 : 0);
   lua_setglobal(L, "Expected");
   insertFunction(L, &boolTester, "test");
   runCode(L, Code);
   int Result = runCode(L, "test(V)");
   UNIT_ASSERT_EQUALS(0, Result);
   lua_close(L);
}

// TODO how do we detect errors?
static void testExpectedBoolParam() {
   COL_FUNCTION(testExpectedIntParam);
   testHelperBool("V = {param=true}", true);
   testHelperBool("V = {param=false}", false);
   //testHelperBool("V = {param=0}", false);  lua_isboolean doesn't recognize 0, 1 etc.
}

static void testHelperInt(const char* Code, int ExpectedValue){
   COL_FUNCTION(testHelperInt);
   lua_State* L = testVariable(Code);
   LUAparamIsTable(L, 1);
   int Result = LUAexpectedIntParam(L, 1, "param");
   UNIT_ASSERT_EQUALS(Result, ExpectedValue);
   lua_close(L);
}

static void testExpectedIntParam() {
   COL_FUNCTION(testExpectedIntParam);
   testHelperInt("V = {param='111'}", 111);
   testHelperInt("V = {param=111}", 111);
}

static void testHelperString(const char* Code, const COLstring& ExpectedValue){
   COL_FUNCTION(testHelperString);
   lua_State* L = testVariable(Code);
   LUAparamIsTable(L, 1);
   COLstring Result = LUAexpectedStringParam(L, 1, "param");
   UNIT_ASSERT_EQUALS(Result, ExpectedValue);
   lua_close(L);
}

static void testExpectedStringParam() {
   COL_FUNCTION(testExpectedStringParam);
   testHelperString("V={param='life'}", "life");
   testHelperString("V={param=111}", "111");
}

static void testOptionalStringParamPushed() {
   COL_FUNCTION(testOptionalStringParamPushed);
   lua_State* L = testVariable("V={param='life'}");
   // if parameter is absent, return false and the stack is untouched
   UNIT_ASSERT_EQUALS(false, LUAoptionalStringParamPushed(L, 1, "absent"));
   // if parameter is absent, return true and "life" is on top of the stack
   UNIT_ASSERT_EQUALS(true, LUAoptionalStringParamPushed(L, 1, "param"));
   UNIT_ASSERT_EQUALS(true, lua_isstring(L, -1));
   UNIT_ASSERT_EQUALS(0, strcmp("life", lua_tostring(L, -1)));
   lua_pop(L, -1);   // pop "life" from stack
   lua_close(L);
}

void testTable(UNITapp* pApp) {
   pApp->add("table/LUATexpectedBoolParam", &testExpectedBoolParam);
   pApp->add("table/LUATexpectedIntParam", &testExpectedIntParam);
   pApp->add("table/LUATexpectedStringParam", &testExpectedStringParam);
   pApp->add("table/LUAToptionalStringParamPushed", &testOptionalStringParamPushed);
}
