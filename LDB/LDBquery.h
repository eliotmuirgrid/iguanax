#ifndef __LDB_QUERY_H__
#define __LDB_QUERY_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBquery
//
// Description:
//
// LUA endpoints for db.query and db.execute - these call the same NODL function
//
// Author: Vismay Shah 
// Date:   Thursday 01 December 2022 - 03:56PM
// ---------------------------------------------------------------------------
struct lua_State;

int LDBquery(lua_State* L);
int LDBexecute(lua_State* L);

#endif // end of defensive include
