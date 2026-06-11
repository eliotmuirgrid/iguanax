#ifndef __LNET_TCP_ASYNC_H__
#define __LNET_TCP_ASYNC_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNETtcpAsync
//
// Description:
//
// The asynchronous socket api for Lua.
//
// Author: John Q
// Date:   Thu Apr 24 11:40:14 EDT 2025
//---------------------------------------------------------------------------
class  COLstring;
struct lua_State;

void LNETaddTcpAsyncModule(lua_State* L);
bool LNETusingTcpAsync(lua_State* L, int ConnId);
bool LNETusingTcpAsyncListen(lua_State* L);
int LNETonAccept(lua_State* L, int ConnId, const COLstring& ClientIp);
int LNETonData  (lua_State* L, int ConnId, const COLstring& Buffer);
int LNETonWrite (lua_State* L, int ConnId);
int LNETonClose (lua_State* L, int ConnId, const COLstring& Error, const COLstring& Buffer);
int LNETonConnect(lua_State* L, int ConnId);

#endif