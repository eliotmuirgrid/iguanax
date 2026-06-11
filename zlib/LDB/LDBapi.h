#ifndef __LDB_API_H__
#define __LDB_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBapi
//
// Description:
//
// Lua db API implementation
//
// Author: Nicolas Lehman
// Date: Thu May 13 2021
//---------------------------------------------------------------------------

struct lua_State;

// Store counter of active database connections in Lua registry.
void LDBaddDatabaseConnectionCount(lua_State* L);

void LDBaddDatabaseNamespace(lua_State* L);

#endif // end of defensive include
