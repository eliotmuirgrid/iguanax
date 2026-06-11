#ifndef __LIG_COMPONENTS_H__
#define __LIG_COMPONENTS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LIGcomponents
//
// Description:
// LIGlistComponents can be used to get the list of component GUIDs.
// Lua binding for iguana.components
//
// Author: Vismay Shah
//---------------------------------------------------------------------------

struct lua_State;

int LIGlistComponents(lua_State* L);

#endif