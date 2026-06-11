//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUALutils
//
// Description:
//
// Utility functions for creating Lua C libraries that are dynamically loaded
// when needed. Forked from TNDLutils.
//
// Author: Eliot Muir
// Date:   Thursday, October 14th, 2010 @ 12:48:53 PM
//---------------------------------------------------------------------------

#include <LUAL/LUALutils.h>
#include <LUA/LUAtimeout.h>

#include <LUA/LUAtable.h>
#include <LUA/LUAerror.h>

#include <LUA/LUAutils.h>
#include <LUA/LUAcheck.h>

#include <LJSON/LJSONapi.h>
#include <SCK/SCKloop.h>
#include <COL/COLthreadPool.h>
#include <BLUA/BLUAinstance.h>


#include <NOD/NODtype.h>

#include <COL/COLclosure6.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
COL_LOG_MODULE;

const int LUAL_LUA_PROTOCOL = 0;

const COLstring LUALnilDisplayType("nil");
const COLstring LUALbooleanDisplayType("boolean");
const COLstring LUALnumberDisplayType("number");
const COLstring LUALstringDisplayType("string");
const COLstring LUALtableDisplayType("table");
const COLstring LUALfunctionDisplayType("function");
const COLstring LUALuserdataDisplayType("userdata");
const COLstring LUALthreadDisplayType("thread");

const COLstring LUALluaProtocolName("lua");
const COLstring LUALnullDisplayType("null");
const COLstring LUALunknownDisplayType("unknown");
const COLstring LUALerrorDisplayType("error");
const COLstring LUALwarningDisplayType("warning");

void LUALregisterLuaTypes(){
   COL_FUNCTION(LUALregisterLuaTypes);
   // Original Lua types
   NODtypeMapAdd(LUALnilDisplayType,      LUA_TNIL,      LUALluaProtocolName, LUAL_LUA_PROTOCOL);
   NODtypeMapAdd(LUALbooleanDisplayType,  LUA_TBOOLEAN,  LUALluaProtocolName, LUAL_LUA_PROTOCOL);
   NODtypeMapAdd(LUALnumberDisplayType,   LUA_TNUMBER,   LUALluaProtocolName, LUAL_LUA_PROTOCOL);
   NODtypeMapAdd(LUALstringDisplayType,   LUA_TSTRING,   LUALluaProtocolName, LUAL_LUA_PROTOCOL);
   NODtypeMapAdd(LUALtableDisplayType,    LUA_TTABLE,    LUALluaProtocolName, LUAL_LUA_PROTOCOL);
   NODtypeMapAdd(LUALfunctionDisplayType, LUA_TFUNCTION, LUALluaProtocolName, LUAL_LUA_PROTOCOL);
   NODtypeMapAdd(LUALuserdataDisplayType, LUA_TUSERDATA, LUALluaProtocolName, LUAL_LUA_PROTOCOL);
   NODtypeMapAdd(LUALthreadDisplayType,   LUA_TTHREAD,   LUALluaProtocolName, LUAL_LUA_PROTOCOL);

   // Our additional Lua types (for display (icon) purposes only)
   NODtypeMapAdd(LUALnullDisplayType,    LUAL_JSON_NULL_DISPLAY_TYPE, LUALluaProtocolName, LUAL_LUA_PROTOCOL);
   NODtypeMapAdd(LUALunknownDisplayType, LUAL_LUA_UNKNOWN_TYPE,       LUALluaProtocolName, LUAL_LUA_PROTOCOL);
   NODtypeMapAdd(LUALerrorDisplayType,   LUAL_ERROR_DISPLAY_TYPE,     LUALluaProtocolName, LUAL_LUA_PROTOCOL);
   NODtypeMapAdd(LUALwarningDisplayType, LUAL_WARNING_DISPLAY_TYPE,   LUALluaProtocolName, LUAL_LUA_PROTOCOL);
}

void LUALaddSubMetatable(lua_State* L, const char* MetatableName, const char* Name) {
   COL_FUNCTION(LUALaddSubMetatable);

   if (!lua_getmetatable(L, -1)) {
      lua_newtable(L);
   } //A metatable for the object on top of the stack, old or new, is pushed to the stack.

   lua_getfield(L, -1, "_submeta");
   if (lua_isnil(L,-1)) {
      lua_pop(L, 1);
      lua_newtable(L);
   } //_submeta table, old or new, is pushed to the stack.

   luaL_getmetatable(L, MetatableName);
   lua_setfield(L, -2, Name);
   //_submeta updated with MetatableName.

   lua_setfield(L, -2, "_submeta"); //Adds or updates "_submeta" field of metatable.

   lua_setmetatable(L, -2); //Adds or updates the metatable for the object on the stack;
}

void LUAcheckParamsEx(lua_State* L, int i, const char *const aOptions[]){
   COL_FUNCTION(LUAcheckParamsEx);
   // If we are being called with a single string-like param, coerce
   // it to a table using the named param in the options.
   if (lua_gettop(L) == 1 && !lua_istable(L, 1)) {
      const char *opt = aOptions[0];
      if (*opt) {
         lua_newtable(L);
         lua_pushvalue(L, 1);    //push parameter to top
         lua_setfield(L, 2, opt);
         lua_remove(L, 1);       //remove original parameter
      }
   }
   // Now, do the real work of validating the (new or existing)
   // param table.
   LUAcheckParams(L, i, aOptions);
}