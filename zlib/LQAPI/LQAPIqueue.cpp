//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LQAPIqueue.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   01/12/23 3:53 PM
//  ---------------------------------------------------------------------------
#include <LQAPI/LQAPIqueue.h>
#include <LQAPI/LQAPIcount.h>
#include <LQAPI/LQAPIpush.h>
#include <LQAPI/LQAPIsend.h>

#include <LUA/LUAutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void LQAPIaddQueueNamespace(lua_State* L, LUAmode Mode, CFGmap* pMap, BLUAinstanceManager* pBluaManager, SCKloop* pLoop) {
   COL_FUNCTION(LQAPIaddQueueNamespace);
   lua_newtable(L);
   LUAaddMethod(L, "count",         &LQAPIcount);
   LUAaddMethod(L, "push",          &LQAPIpush);
   lua_setglobal(L, "queue");

   LQAPIsetUpMessageSend(L, Mode, pMap, pBluaManager, pLoop);
   lua_newtable(L);
   LUAaddMethod(L, "send",          &LQAPIpushTransient);
   lua_setglobal(L, "message");
}
