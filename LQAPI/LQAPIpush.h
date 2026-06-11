#ifndef __LQAPI_PUSH_H__
#define __LQAPI_PUSH_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LQAPIpush
//
// Description: The "queue" api for Lua.
//
//
// Author: Vismay Shah
// Date:   Tues, June 7th, 2021
//
//---------------------------------------------------------------------------
struct lua_State;

#define LUA_TARGETED_PUSH "LUA_TARGETED_PUSH"

enum LQAPItargetePushResult {
   LQAPI_TARGET_SUCCESS = 0,
   LQAPI_TARGET_UNKNOWN,
   LQAPI_TARGET_STOPPED,
};

int LQAPIpush(lua_State* L);

#endif