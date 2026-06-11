#ifndef __LUTIL_SAFE_CALL_H__
#define __LUTIL_SAFE_CALL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUTILsafeCall
//
// Description:
//
// Lua binding for util.safeCall (formerly unwind_protect). 
//
// Author: Vismay Shah 
// Date:   Monday 21 April 2025 - 01:06PM
// ---------------------------------------------------------------------------
struct lua_State;

int LUTILsafeCall(lua_State* L);

#endif // end of defensive include
