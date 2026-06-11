#ifndef __LUA_TEST_H__
#define __LUA_TEST_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAtest
//
// Description:
//
// Stuff for .isTest
//
// Author: Eliot Muir 
// Date:   Tuesday 16 May 2023 - 04:59PM
// ---------------------------------------------------------------------------

struct lua_State;

bool LUALisEnvironmentTest(lua_State* L);
void LUALmarkEnvironmentAsTest(lua_State* L);

#endif // end of defensive include
