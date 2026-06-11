#ifndef __LQAPI_SEND_H__
#define __LQAPI_SEND_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LQAPIsend
//
// Description: Lua binding for message.send.
//
// Author: Vismay Shah
// Date:   Tues, June 7th, 2021
//---------------------------------------------------------------------------
#include <LUA/LUAutils.h>

struct lua_State;
struct CFGmap;
class  BLUAinstanceManager;
class  SCKloop;

// sets up message.send() translator API
void LQAPIsetUpMessageSend(lua_State* L, LUAmode Mode, CFGmap* pMap, BLUAinstanceManager* pBluaManager, SCKloop* pLoop);
// make sure LQAPIsetUpMessageSend() is called before registering the message.send API
int LQAPIpushTransient(lua_State* L);

#endif