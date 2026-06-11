#ifndef __LDB_CONNECTION_CHECK_H__
#define __LDB_CONNECTION_CHECK_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBconnectionCheck
//
// Description:
//
// Lua api to check whether a database connection is still connected
//
// Author: Vismay Shah 
// Date:   Mon 12 December 2022
// ---------------------------------------------------------------------------
struct lua_State;

int LDBconnectionCheck(lua_State* L);

#endif // end of defensive include
