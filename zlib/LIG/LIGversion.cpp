// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LIGversion
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Thursday 29 December 2022 - 02:51PM
// ---------------------------------------------------------------------------
#include <LIG/LIGversion.h>

#include <COL/COLlog.h>
#include <LUA/LUAutils.h>

COL_LOG_MODULE;

LIGiguanaVersion IguanaVer;
LIGiguanaVersion* pIguanaVer = 0;

void LIGspecifyIguanaVersion(int Major, int Minor, int Build){
   COL_FUNCTION(LIGspecifyIguanaVersion);
   IguanaVer.Major = Major;
   IguanaVer.Minor = Minor;
   IguanaVer.Build = Build;
   pIguanaVer = &IguanaVer;
};

// tostring method for the Iguana version info table.
static int LIGversionInfoToString(lua_State* L) {
   COL_FUNCTION(LIGversionInfoToString);
   // The version info table should be on the top of the stack when the function is called.
   lua_getfield(L, -1, "major");
   lua_getfield(L, -2, "minor");
   lua_getfield(L, -3, "build");
   // lua_tointeger will return 0 if the specified value is not a number or a string convertible to a number.
   lua_pushfstring(L, "%d.%d.%d", lua_tointeger(L, -3), lua_tointeger(L, -2), lua_tointeger(L, -1));
   // Move the output string below the version info numbers and then pop them from the stack.
   lua_insert(L, -4);
   lua_pop(L, 3);
   return 1;
}

// Returns a table containing the version info for Iguana. This table has an
// associated metatable which improves the display of the version info.
int LIGversion(lua_State* L){
   COL_FUNCTION(LIGversion);
   COL_VAR3(IguanaVer.Major, IguanaVer.Minor, IguanaVer.Build);
   lua_newtable(L);
   // Add version info to the table.
   lua_pushinteger(L, pIguanaVer->Major);
   lua_setfield(L, -2, "major");
   lua_pushinteger(L, pIguanaVer->Minor);
   lua_setfield(L, -2, "minor");
   lua_pushinteger(L, pIguanaVer->Build);
   lua_setfield(L, -2, "build");
   // Define the metatable for the version info.
   lua_newtable(L);

   lua_newtable(L); // _displayKeys table
   lua_pushliteral(L, "major");
   LUAappendToTable(L, -2);
   lua_pushliteral(L, "minor");
   LUAappendToTable(L, -2);
   lua_pushliteral(L, "build");
   LUAappendToTable(L, -2);
   lua_setfield(L, -2, "_displayKeys");

   LUAaddMethod(L, "__tostring", &LIGversionInfoToString);
   lua_setmetatable(L, -2); // pops metatable from stack
   return 1;
}