#ifndef __LUA_CHECK_H__
#define __LUA_CHECK_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAcheck
//
// Description:
//
// LUAcheckParams came from LUAL
//
// Author: Eliot Muir 
// Date:   Tuesday 16 May 2023 - 04:37PM
// ---------------------------------------------------------------------------

struct lua_State;
class COLvar;
class COLstring;

void LUAcheckParams(lua_State* L, int i, const char *const aOptions[]);

// Various parameter utility functions
int LUAcheckArgCount(lua_State* L, int Low, int High=-1);

// Checks the provided map to see if it has keys that are strictly contained within the supplied KnownParameters
// This is useful for checking that the table a user supplied has no extraneous (unknown) parameters
// If the OffendingKey field is supplied, it is set to the parameter that caused the issue
bool LUAparamsAreKnown(const COLvar& VarMap, const char* const KnownParameters[], COLstring* OffendingKey);

#endif // end of defensive include
