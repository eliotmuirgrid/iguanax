#ifndef __BLUA_RUN_H__
#define __BLUA_RUN_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUArun
//
// Description:
//    BLUAinstance lua run task
//
// Author: John Q
// Date:   Wednesday Oct 5 2022
//---------------------------------------------------------------------------

class  COLvar;
class  BLUAinstance2;
struct BLUAtask;
struct lua_State;



BLUAtask BLUArunTask();
void BLUAhandleRun(lua_State*& L, const COLvar& Payload, int* pResult, BLUAinstance2* pInstance);
void BLUAinstanceSetUpTimeoutHook(lua_State* L);
#endif // end of defensive include
