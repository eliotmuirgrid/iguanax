#ifndef __LDB_QUOTE_H__
#define __LDB_QUOTE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBquote
//
// Description:
//
// Lua api to return an escaped string surrounded by single quotes for use as part of
// a database query
//
// Author: Vismay Shah 
// Date:   Mon 12 December 2022
// ---------------------------------------------------------------------------
struct lua_State;

int LDBquote(lua_State* L);

#endif // end of defensive include
