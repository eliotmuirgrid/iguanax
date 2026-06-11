#ifndef __LCOM_GET_NAME_H__
#define __LCOM_GET_NAME_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMgetName
//
// Description:
// Api to get the component name from Lua.
// Lua binding for component.name 
//
// Author: Vismay Shah 
// Date:   Wednesday 25 October 2023 - 02:48PM
// ---------------------------------------------------------------------------
struct lua_State;

int LCOMgetName(lua_State* L);

#endif // end of defensive include
