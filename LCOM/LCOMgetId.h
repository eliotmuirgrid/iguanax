#ifndef __LCOM_GET_ID_H__
#define __LCOM_GET_ID_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMgetId
//
// Description:
// Api to get the component guid from Lua.
// Lua binding for component.id
//
// Author: Vismay Shah 
// Date:   Wednesday 25 October 2023 - 02:51PM
// ---------------------------------------------------------------------------
struct lua_State;

int LCOMgetId(lua_State* L);

#endif // end of defensive include
