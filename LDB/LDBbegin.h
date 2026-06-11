#ifndef __LDB_BEGIN_H__
#define __LDB_BEGIN_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBbegin
//
// Description:
// Begin a database transaction
// Lua binding for database:beginTransaction (formerly db:begin()) 
//
// Author: Vismay Shah 
// Date:   Monday 05 December 2022 - 11:34AM
// ---------------------------------------------------------------------------
struct lua_State;

int LDBbeginTransaction(lua_State* L);

#endif // end of defensive include
