#ifndef __LCOM_IS_TEST_MODE_H__
#define __LCOM_IS_TEST_MODE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMisTestMode
//
// Description:
//
// Lua binding for iguana.isTest
// Returns whether we are in translator 
//
// Author: Vismay Shah 
// Date:   Monday 21 April 2025 - 10:12AM
// ---------------------------------------------------------------------------
struct lua_State;

int LCOMisTestMode(lua_State* L);

#endif // end of defensive include
