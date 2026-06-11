// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMisTestMode
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 21 April 2025 - 10:12AM
// ---------------------------------------------------------------------------
#include <LCOM/LCOMisTestMode.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAtest.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int LCOMisTestMode(lua_State* L){
   COL_FUNCTION(LCOMisTestMode);
   if (lua_gettop(L) > 0) { LUA_ERROR_STREAM("No arguments needed for isTest()"); }
   bool IsTest = LUALisEnvironmentTest(L);
   lua_pushboolean(L, IsTest);
   COL_VAR(IsTest);
   return 1;
}