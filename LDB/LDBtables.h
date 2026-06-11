#ifndef __LDB_TABLES_H__
#define __LDB_TABLES_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBtables
//
// Description:
//
// Function which returns a record set created from a table schema
//
// Author: Eliot Muir 
// Date:   Wednesday 09 November 2022 - 08:24AM
// ---------------------------------------------------------------------------

struct lua_State;

int LDBloadSchemaTables(lua_State* L);

#endif // end of defensive include
