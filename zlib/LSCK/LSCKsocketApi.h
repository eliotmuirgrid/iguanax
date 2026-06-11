#ifndef __LSCK_SOCKET_API_H__
#define __LSCK_SOCKET_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LSCKsocketApi
//
// Description: The "socket" api for Lua.
//
//
// Author: Shuo Hou
// Date:   Jan 10th, 2022
//
//---------------------------------------------------------------------------
struct lua_State;

void LSCKaddSocketNamespace(lua_State* L);

#endif