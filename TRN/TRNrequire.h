#ifndef __TRN_REQUIRE_H__
#define __TRN_REQUIRE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNrequire
//
// Description:
//
// Our replacement for the Lua require function.  This allows us to compile
// the code with hooks for annotations and auto completion.
//
// Author: Eliot Muir
// Date:   Wednesday, November 17th, 2010 @ 12:55:34 PM
//---------------------------------------------------------------------------

#include <LUA/LUAheader.h>
#include <COL/COLhashmap.h>
class TRNfile;

void TRNrequireOverride(lua_State* L);
void TRNdestroyLuacom(lua_State* L);
#endif // end of defensive include
