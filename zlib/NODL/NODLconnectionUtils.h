#ifndef __NODL_CONNECTION_UTILS_H__
#define __NODL_CONNECTION_UTILS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODLdbConnection
//
// Description:
//
// Utility functions used by db.connect and NODLdbConnection
//
// Author: Vismay Shah 
// Date:   Thursday 1 December 2022
// ---------------------------------------------------------------------------
struct lua_State;

//Check whether any parameters in current method call aren't in either Params or standard connection parameters.
//NewConnection should be false when using an existing connection, and false otherwise.
void NODLcheckCombinedParams(lua_State* L, int i, bool NewConnection, const char* const Params[]);


#endif // end of defensive include
