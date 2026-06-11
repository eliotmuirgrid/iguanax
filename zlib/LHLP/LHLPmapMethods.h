#ifndef __HELP_MAP_METHODS_H__
#define __HELP_MAP_METHODS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LHLPmapMethods
//
// Description:
//
// A wrapper for setting a metatable for an object into the Lua registry.
//
// Author: Vismay Shah 
// Date:   Tuesday 31 January 2023 - 01:14PM
// ---------------------------------------------------------------------------
struct lua_State;
class COLstring;

// Expects a <directory> relative to the component directory and a <table> consisting of method names mapped to functions.
// Creates an entry in the lua registry at the key <directory> and sets a metatable there
int LHLPmapMethods(lua_State* L);

void LHLPaddHelpNamespace(lua_State* L);

#endif // end of defensive include
