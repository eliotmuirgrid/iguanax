#ifndef __LQAPI_QUEUE_H__
#define __LQAPI_QUEUE_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LQAPIqueue.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   01/12/23 3:53 PM
//  ---------------------------------------------------------------------------
#include <LUA/LUAutils.h>

struct lua_State;
struct CFGmap;
class  BLUAinstanceManager;
class  SCKloop;

void LQAPIaddQueueNamespace(lua_State* L, LUAmode Mode, CFGmap* pMap, BLUAinstanceManager* pBluaManager, SCKloop* pLoop);

#endif // end of defensive include
