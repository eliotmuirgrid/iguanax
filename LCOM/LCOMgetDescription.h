#ifndef __LCOM_GET_DESCRIPTION_H__
#define __LCOM_GET_DESCRIPTION_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMgetDescription
//
// Description:
// Api to get the component description from Lua.
// Lua binding for component.description.
//
// Author: Vismay Shah
// Date:   Wednesday 25 October 2023 - 02:51PM
// ---------------------------------------------------------------------------
struct lua_State;

int LCOMgetDescription(lua_State* L);

#endif // end of defensive include
