#ifndef __BLUA_TIMER_H__
#define __BLUA_TIMER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAtimer
//
// Description:
//    BLUAinstance timer functions
//
// Author: Vismay Shah
// Date:   Friday May 14 2021
//---------------------------------------------------------------------------
#include <COL/COLmap.h>

class  COLstring;
class  COLvar;
struct BLUAtask;
struct lua_State;

BLUAtask BLUAtimerTask(const COLstring& Data, int CallbackRef);
void BLUAhandleTimer(lua_State*& L, const COLvar& Payload, int* pResult);

#endif // end of defensive include
