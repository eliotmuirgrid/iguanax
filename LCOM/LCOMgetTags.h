#ifndef __LCOM_GET_TAGS_H__
#define __LCOM_GET_TAGS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMgetTags
//
// Description:
// Api to get the component tags from Lua.
// Lua binding for component.tags 
//
// Author: Vismay Shah 
// Date:   Monday 22 April 2024 - 01:09PM
// ---------------------------------------------------------------------------
struct lua_State;

int LCOMgetTags(lua_State* L);

#endif // end of defensive include
