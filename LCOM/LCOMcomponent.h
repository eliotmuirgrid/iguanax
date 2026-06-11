#ifndef __LCOM_COMPONENT_H__
#define __LCOM_COMPONENT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMcomponent
//
// component lua name space
//
// Author: Shuo Hou
// Date:   Feb 18th, 2022
//---------------------------------------------------------------------------

struct lua_State;
struct CFGmap;
class  BLUAinstanceManager;
class  SCKloop;

void LCOMaddComponentNamespace(lua_State* L, CFGmap* pMap, BLUAinstanceManager* pBluaManager, SCKloop* pLoop);

#endif