#ifndef __LUA_STARTUP_H__
#define __LUA_STARTUP_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAstartup
//
// Description:
//
// Mark/determine whether we are in "startup" mode where iguana.register's
// startup functions are called.
//
// Author: Vismay Shah 
// Date:   Thursday 27 March 2025 - 11:42AM
// ---------------------------------------------------------------------------

struct lua_State;

bool LUALisEnvironmentStartup(lua_State* L);
void LUALmarkEnvironmentAsStartup(lua_State* L);

#endif // end of defensive include
