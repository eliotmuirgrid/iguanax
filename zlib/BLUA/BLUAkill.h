#ifndef __BLUA_KILL_H__
#define __BLUA_KILL_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAkill
//
// Description:
//    BLUAinstance lua kill task
//
// Author: John Q
// Date:   Wednesday Oct 5 2022
//---------------------------------------------------------------------------

class  COLvar;
struct BLUAtask;
struct lua_State;

BLUAtask BLUAkillTask();
void BLUAhandleKill(lua_State*& L, const COLvar& Payload, int* pResult);

#endif // end of defensive include
