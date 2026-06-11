//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testApp
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tue 18 Nov 2014 10:23:15 EST
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <LUA/LUAheader.h>
#include <TRN/TRNparseErrorForMissingEnd.h>
COL_LOG_MODULE;

// This test compiles a lua script which has a syntax error, a missing 'end' on line 5
// to close an if statement. The lua compiler will recognize that the error is attributed
// to the function starting on line 1, but will say the error occurs near end of file, since 
// its waiting for an 'end'.
static void testEndMissingErrorLineNumberOverwrite() {
   COL_FUNCTION(testEndMissingErrorLineNumberOverwrite);
   COL_TRC("Creating lua state");
   lua_State *L = luaL_newstate();
   luaL_openlibs(L);
   int CompileResult = luaL_dofile(L, "scriptWithMissingEnd.lua");
   UNIT_ASSERT_EQUALS(1, CompileResult);  // The compile should have failed
   if (CompileResult) { 
      COLstring LuaCompileError = lua_tostring(L, -1);
      UNIT_ASSERT_EQUALS("scriptWithMissingEnd.lua:14: 'end' expected (to close 'function' at line 1) near '<eof>'", LuaCompileError);
      // This is a mockup of how TRN's error structure looks like
      COLvar ErrorDetails;
      ErrorDetails["FileName"] = "scriptWithMissingEnd.lua";
      ErrorDetails["Description"] = LuaCompileError;
      ErrorDetails["LineNumber"] = 14; // Since the error message says near '<eof>', TRN's logic would have resolved that to line 14
      // The override logic should parse the error message and set the line to 1 since that function is missing the end
      TRNparseErrorForMissingEnd(&ErrorDetails); 
      UNIT_ASSERT_EQUALS(1, ErrorDetails["LineNumber"].asInt());
   }
   lua_close(L);
}

static int myCustomCFunction(lua_State* L){
   COL_FUNCTION(myCustomCFunction);
   lua_pushstring(L, "executed myFunction");
   return 1;
}

static bool checkGlobalTable(lua_State* L){
   COL_FUNCTION(checkGlobalTable);
   // First check _G is of type table
   lua_getglobal(L, "_G");
   if (!lua_istable(L, -1)) {
      COL_TRC("_G not type table");
      return false;
   }
   // Second check _G._G is of type table
   lua_pushstring(L, "_G");
   lua_gettable(L, -2);
   if (!lua_istable(L, -1)) {
      COL_TRC("_G._G not type table");
      return false;
   }
   return true;
}

static void testGlobalTableModification() {
   COL_FUNCTION(testGlobalTableModification);
   COL_TRC("Creating lua state");
   lua_State *L = luaL_newstate();
   luaL_openlibs(L);
   lua_pushcfunction(L, myCustomCFunction);
   lua_setglobal(L, "myFunction");
   UNIT_ASSERT_EQUALS(true, checkGlobalTable(L));
   // setup done, now run
   int CompileResult = luaL_dofile(L, "scriptWithGlobalOverwrite.lua");
   COL_VAR(CompileResult);  // The compile should have failed???
   if (CompileResult) { 
      COLstring LuaCompileError = lua_tostring(L, -1);
      COL_VAR(LuaCompileError);
   } 
   UNIT_ASSERT_EQUALS(false, checkGlobalTable(L));
   COL_TRC("Global table has been modified and is no longer of type table");
   lua_close(L);
}


void testApp(UNITapp* pApp) {
   pApp->add("app/testEndErrorOverride",       &testEndMissingErrorLineNumberOverwrite);
   pApp->add("app/testGlobalTableModification", &testGlobalTableModification);
}

