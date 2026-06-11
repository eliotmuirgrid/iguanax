#ifndef __LTUN_API_H__
#define __LTUN_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LTUNapi
//
// Description:
//
// Lua API for interacting with TUN - for peer iguana connections
//
// Author: Vismay Shah 
// Date:   Friday 07 July 2022 - 01:30PM
// ---------------------------------------------------------------------------

struct lua_State;

void LTUNapi(lua_State* L);

#endif // end of defensive include
