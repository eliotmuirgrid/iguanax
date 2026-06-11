//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LAPIapi
//
// Description:
//    Implementation
//
// Author: Vismay Shah
// Date:   Tues, June 7th, 2021
//
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <LAPI/LAPIapi.h>
#include <LCR/LCRapi.h>
#include <LIV/LIVapi.h>
#include <LND/LNDutils.h>
#include <LUA/LUAtimeout.h>
#include <LUTIL/LUTILsafeCall.h>
#include <SEC/SECaccess.h>
COL_LOG_MODULE;

static void LAPIremoveCoroutine(lua_State* L){
   COL_FUNCTION(LAPIremoveCoroutine);
   // Remove it from the globals table
   lua_pushnil(L);
   lua_setglobal(L, "coroutine");
   // Also remove it from packages.loaded -- not sure how to do this or if its needed
   // int StackTop = lua_gettop(L);
   // COL_VAR(StackTop);
   // COL_TRC("Looking for package");
   // lua_getglobal(L, "packages"); //+1
   // COL_TRC("Looking for loaded");
   // lua_getfield(L, -1, "loaded");// +1
   // lua_pushnil(L); // +1
   // lua_setfield(L, -2, "coroutine"); //-1
   // lua_pop(L, 2);
   // StackTop = lua_gettop(L);
   // COL_VAR(StackTop);
}

static int LAPIdisabledTrace(lua_State* L) {
   return 0;
}

void LAPIapi(lua_State* L) {
   // Register functions at the global level.
   lua_pushcfunction(L, LUTILsafeCall);
   lua_setglobal    (L, "unwind_protect");
   lua_pushcfunction(L, LAPIdisabledTrace);
   lua_setglobal    (L, "trace");
}

static int LAPIload_aux(lua_State *L, int status) {
  if (status == 0)  /* OK? */
    return 1;
  else {
    lua_pushnil(L);
    lua_insert(L, -2);  /* put before error message */
    return 2;  /* return nil plus error message */
  }
}


int LAPIloadfile(lua_State* L){
   COL_FUNCTION(LAPIloadfile);
   const char *fname = luaL_optstring(L, 1, NULL);
	COLstring	Err;
	if(!SECvalidate(fname, &Err)) { return luaL_error(L, "%s", Err.c_str()); }
   if (fname == NULL){
      luaL_error(L, "Load file from standard input is not supported.");
   } 
   return LAPIload_aux(L, luaL_loadfile(L, fname));
}

void LAPIremoveUnsupportedFunctions(lua_State* L){
   COL_FUNCTION(LAPIremoveUnsupportedFunctions);
   LAPIremoveCoroutine(L);
   lua_pushnil(L);
   lua_setglobal(L, "newproxy");
   lua_pushcfunction(L, &LAPIloadfile);
   lua_setglobal(L, "loadfile");
}
