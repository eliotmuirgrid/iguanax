#ifndef __LUA_OPEN_H__
#define __LUA_OPEN_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAopen
//
// Description:
//
// Calls lua_open and loads the libraries.
//
// Author: Eliot Muir
// Date:   Friday 19 May 2023 - 04:10PM
// ---------------------------------------------------------------------------
#include <LUA/LUAutils.h>

class COLstring;
struct lua_State;

lua_State* LUAopen(LUAmode Mode);

void LUAsetIdAndRoot(lua_State* L, const COLstring& ComponentId, const COLstring& ProjectRoot);
COLstring LUAcomponentId(lua_State* L);
COLstring LUAprojectRoot(lua_State* L);

#endif // end of defensive include
