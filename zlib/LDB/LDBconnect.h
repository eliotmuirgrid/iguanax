#ifndef __LDB_CONNECT_H__
#define __LDB_CONNECT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBconnect
//
// Description:
//
// Lua api to connect to a database
//
// Author: Vismay Shah 
// Date:   Thur 1 December 2022
// ---------------------------------------------------------------------------
struct lua_State;

int LDBconnect(lua_State* L);

#endif // end of defensive include
