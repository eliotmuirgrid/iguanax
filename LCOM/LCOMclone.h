#ifndef __LCOM_CLONE_H__
#define __LCOM_CLONE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMclone
//
// Description:
//
// API to create a set of Lua instances for a component. Lua binding for 
// component.spawnClone (formerly component.clone)
//
// Author: Vismay Shah
 
// Date:   Monday 25 November 2024 - 10:48AM
// ---------------------------------------------------------------------------
struct lua_State;
class BLUAinstanceManager;
class SCKloop;

int  LCOMspawnClone(lua_State* L);
void LCOMsetUpClone(lua_State* L, BLUAinstanceManager* pBluaManager, SCKloop* pLoop);

#endif // end of defensive include
