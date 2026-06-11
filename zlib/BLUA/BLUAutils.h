#ifndef BLUAUTILS_H
#define BLUAUTILS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: BLUAutils.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   24/03/25 2:27 PM
//  ---------------------------------------------------------------------------
#include <COL/COLarray.h>

class COLstring;
class COLvar;
class BLUAinstance2;
struct CFGmap;
struct lua_State;

void BLUAsetLuaRequirePath(lua_State* L, const COLstring& ProjectPath);
COLstring BLUAfdError();

// windows specific
void BLUAaddLuacomLoader(lua_State* L);

#endif // end of defensive include
