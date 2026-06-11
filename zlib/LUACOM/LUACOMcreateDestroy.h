#ifndef __LUACOM_CREATE_DESTROY_H__
#define __LUACOM_CREATE_DESTROY_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUACOMcreateDestroy
//
// Description:
//
// Functions wrapping luacom_open and luacom_close. Windows only.
//
// Author: Vismay Shah
// Date:   Friday, October 20th, 2023 @ 2:54:34 PM
//---------------------------------------------------------------------------
struct lua_State;

// Can be called from any OS.
void LUACOMdestroyLuacomWrapper(lua_State* L, bool DestroyRequired);

#ifdef _WIN32
int LUACOMloadLuacom(lua_State* L);
#endif

#endif // end of defensive include
