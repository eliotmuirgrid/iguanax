#ifndef __LTMR_TIMER_H__
#define __LTMR_TIMER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LTMRtimer
//
// Description:
//
// Lua bindings for component.setTimer and component.cancelTimer
//
// Author: Vismay Shah
// Date:   Tuesday 23 May 2023 - 12:59PM
// ---------------------------------------------------------------------------
class  BLUAinstance2;
struct lua_State;

int LTMRsetTimer(lua_State* L);
int LTMRcancelTimer(lua_State* L);
void LTMRcancelAllTimers(BLUAinstance2* pInstance);

#endif // end of defensive include
