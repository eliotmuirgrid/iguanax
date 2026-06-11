// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUTILsafeCall
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 21 April 2025 - 01:06PM
// ---------------------------------------------------------------------------
#include <LUTIL/LUTILsafeCall.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAtimeout.h>
#include <time.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void popfield(lua_State* L, int index, const char *k){
   lua_getfield(L, index, k);
   lua_pushnil(L);
   lua_setfield(L, index, k);
}

int LUTILsafeCall(lua_State* L){
   COL_FUNCTION(LUTILsafeCall);
   int i, nValues, Top = lua_gettop(L);
   bool Failed = false;
   time_t NewTarget;

   luaL_checktype(L, 1, LUA_TFUNCTION);
   luaL_checktype(L, 2, LUA_TFUNCTION);
   // Call the main logic routine.
   lua_pushvalue(L, 1);
   Failed = (lua_pcall(L, 0, LUA_MULTRET, 0) != 0);
   nValues = lua_gettop(L) - Top;
   // The failure may have been a timeout.  We need the cleanup routine
   // to run, so we change the timeout settings temporarily.
   lua_getfield(L, LUA_REGISTRYINDEX, LUAL_TIMEOUT_STRING);
   lua_getfield(L, LUA_REGISTRYINDEX, LUAL_TIMEOUT_TARGET);
   if(lua_tointeger(L,-1) < (NewTarget = time(NULL) + 60)) {
      lua_pushinteger(L, NewTarget);
      lua_setfield(L, LUA_REGISTRYINDEX, LUAL_TIMEOUT_TARGET);
   }
   popfield(L, LUA_REGISTRYINDEX, LUAL_TIMEOUT_CHECKER);
   // Call the cleanup routine with the results of the previous pcall().
   lua_pushvalue(L, 2);
   lua_pushboolean(L, !Failed);
   for(i=1; i <= nValues; i++)  { lua_pushvalue(L, Top + i); }
   if(lua_pcall(L, nValues+1, 0, 0) != 0) {
      Failed = true;
      lua_insert(L, -4);  // Insert error before timeout fields.
   }
   // Undo our changes to the timeout settings.
   lua_setfield(L, LUA_REGISTRYINDEX, LUAL_TIMEOUT_CHECKER);
   lua_setfield(L, LUA_REGISTRYINDEX, LUAL_TIMEOUT_TARGET);
   lua_setfield(L, LUA_REGISTRYINDEX, LUAL_TIMEOUT_STRING);
   if(Failed) lua_error(L);
   return nValues;

}