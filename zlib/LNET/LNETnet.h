#ifndef __LNET_NET_H__
#define __LNET_NET_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNETnet
//
// Description:
//
// Lua wrapper for C functions which make up the "net.http, net.smtp, net.tcp, etc"
// keywords in Lua.
//
// Author: Eric Mulvaney
// Date:   Monday, June 13th, 2011 @ 05:53:28 PM
//
//---------------------------------------------------------------------------
struct lua_State;

#define LNET_NAMESPACE "net"

void LNETaddNetNamespace(lua_State* L);

#endif