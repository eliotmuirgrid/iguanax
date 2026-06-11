#ifndef __DBD_LUA_REGISTER_H__
#define __DBD_LUA_REGISTER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDluaRegister
//
// Description:
//
// This function push additional data to lua_state after its construction.
//
// Author: John Q
// Date:   Wed 28 September 2022 - 02:20PM
// ---------------------------------------------------------------------------
#include <LUA/LUAutils.h>

class DBDworld;
struct lua_State;

void DBDluaRegister(lua_State* L, LUAmode Mode, DBDworld* pWorld);
void DBDluaIdle(lua_State* L);

#endif // end of defensive include
