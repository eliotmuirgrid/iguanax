#ifndef __LCOM_STOPPING_H__
#define __LCOM_STOPPING_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMstopping
//
// Description:
//
// Api to see if a lua state is in the process of stopping 
//
// Author: Vismay Shah 
// Date:   Monday 24 July 2023 - 11:59AM
// ---------------------------------------------------------------------------
struct lua_State;

int LCOMisStopping(lua_State* L);

#endif // end of defensive include
