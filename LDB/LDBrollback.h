#ifndef __LDB_ROLLBACK_H__
#define __LDB_ROLLBACK_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBrollback
//
// Description:
// Rollback a database transaction
// Lua binding for database:rollbackTransaction (formerly db:rollback()) 
//
// Author: Vismay Shah 
// Date:   Monday 05 December 2022 - 11:34AM
// ---------------------------------------------------------------------------
struct lua_State;

int LDBrollbackTransaction(lua_State* L);

#endif // end of defensive include
