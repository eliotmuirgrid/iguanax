//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// DICOM module back-end: dicom_raw.dll
//
// Author: Eric Mulvaney
// Date:   July 2011
//
//---------------------------------------------------------------------------

#include <math.h>
#include <LUAL/LUALutils.h>
#include <COL/COLstring.h>


static int l_sine(lua_State* L) {
   double d = luaL_checknumber(L, 1);
   lua_pushnumber(L, sin(d));
   return 1;  /* number of results */
}

static int l_sqrt(lua_State* L) {
   double d = luaL_checknumber(L, 1);
   lua_pushnumber(L, sqrt(d));
   return 1;  /* number of results */
}

static int l_hello(lua_State* L) {
   COLstring r = "World";
   lua_pushstring(L, r.c_str());
   return 1;  /* number of results */
}


extern "C" LUAL_DLL_EXPORT int luaopen_hello(lua_State* L) {
   lua_newtable(L);

   lua_newtable(L);  // math
   LUAL_REGISTER_FUNCTION("sine", l_sine);
   LUAL_REGISTER_FUNCTION("sqrt", l_sqrt);
   lua_setfield(L, -2, "maths");

   LUAL_REGISTER_FUNCTION("hello", l_hello);

   lua_pushvalue(L, -1);
   lua_setfield(L, LUA_GLOBALSINDEX, "hello");
   return 1;
}
