#ifndef __LDB_COMMIT_H__
#define __LDB_COMMIT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBcommit
//
// Description:
// Commit a database transaction
// Lua binding for database:commitTransaction (formerly db:commit()) 
//
// Author: Vismay Shah 
// Date:   Monday 05 December 2022 - 11:34AM
// ---------------------------------------------------------------------------
struct lua_State;

int LDBcommitTransaction(lua_State* L);

#endif // end of defensive include
