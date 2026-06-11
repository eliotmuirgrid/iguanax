#ifndef __LDB_CLOSE_H__
#define __LDB_CLOSE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBclose
//
// Description:
//
// Lua api to close to a database connection
//
// Author: Vismay Shah 
// Date:   Thur 1 December 2022
// ---------------------------------------------------------------------------
struct lua_State;

int LDBclose(lua_State* L);

#endif // end of defensive include
