#ifndef __LAPI_API_H__
#define __LAPI_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LQAPIapi
//
// Description:
//    Register Lua C api functions to lua state and remove the ones we don't 
//    support (e.g. coroutine.*)
// Author: Vismay Shah
// Date:   Tues, June 7th, 2021
//
//---------------------------------------------------------------------------
struct lua_State;

void LAPIapi(lua_State* L);
void LAPIremoveUnsupportedFunctions(lua_State* L);

#endif  // end of defensive include