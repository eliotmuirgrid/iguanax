//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMisLive
//
// Description: Implementation
//
// Author: Matthew Sobkowski
// Date:   Wednesday 26 April 2023 - 10:30AM
// ---------------------------------------------------------------------------
#include <LCOM/LCOMisLive.h>

#include <BLUA/BLUAtimer.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAheader.h>
#include <LUA/LUAtest.h>
#include <LUAL/LUALutils.h>
#include <COL/COLlog.h>
#include <COL/COLutils.h>
COL_LOG_MODULE;

int LCOMisProductionMode(lua_State* L) {
   COL_FUNCTION(LCOMisProductionMode);
   if(lua_gettop(L) > 0) {LUA_ERROR_STREAM("No arguments needed for live()"); }
   bool IsTest = LUALisEnvironmentTest(L);
   lua_pushboolean(L, !IsTest);
   COL_VAR(IsTest);
   return 1;
}
