#ifndef __LCOM_SET_STOP_ON_ERROR_H__
#define __LCOM_SET_STOP_ON_ERROR_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMsetStopOnError
//
// Description:
// Lua binding for component.setStopOnError (formerly iguana.StopOnError). 
// Toggle whether the component should stop when it encounters an error
//
// Author: Vismay Shah 
// Date:   Monday 21 April 2025 - 10:30AM
// ---------------------------------------------------------------------------
struct lua_State;

int LCOMsetStopOnError(lua_State* L);

#endif // end of defensive include
