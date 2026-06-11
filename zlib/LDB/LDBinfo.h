#ifndef __LDB_INFO_H__
#define __LDB_INFO_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBinfo
//
// Description:
//
// Lua api to see database connection parameters
//
// Author: Vismay Shah 
// Date:   Mon 12 December 2022
// ---------------------------------------------------------------------------
struct lua_State;

int LDBconnectionInfo(lua_State* L);

#endif // end of defensive include
