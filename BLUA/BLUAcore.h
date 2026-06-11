#ifndef __BLUA_CORE_H__
#define __BLUA_CORE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAcore
//
// Description:
//    Core BLUAinstance functions
//
// Author: Vismay Shah
// Date:   Friday May 14 2021
//---------------------------------------------------------------------------
#include <COL/COLclosure.h>
class BLUAinstance2;
class SCKloop;
class BLUAinstanceManager;
struct CFGconfig;
struct lua_State;

BLUAinstance2* BLUAinstanceCreate(CFGconfig& Config, SCKloop* pLoop, BLUAinstanceManager* pManager, bool Resubmission = false);
void BLUAinstanceSpawn(SCKloop* pLoop, BLUAinstanceManager* pManager, BLUAinstance2* pInstance, int Count); // exposed for LCOMclone
int BLUAprintThreadId(lua_State* L);

#endif // end of defensive include
