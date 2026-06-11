#ifndef __NODL_DB_EXECUTE_H__
#define __NODL_DB_EXECUTE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODLdbExecute
//
// Description:
//
// Functions for performing a query
//
// Author: Vismay Shah
// Date:   Thursday 24 November 2022
// ---------------------------------------------------------------------------
struct lua_State;

int NODLdoDbExecute(lua_State* L, bool IsQuery);

#endif // end of defensive include
