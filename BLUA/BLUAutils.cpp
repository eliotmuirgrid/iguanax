
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: BLUAutils.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   24/03/25 2:27 PM
//  ---------------------------------------------------------------------------
#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAutils.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <INFO/INFOusage.h>
#include <INFO/INFOutils.h>
#include <LOG/LOGlog.h>
#include <LUA/LUAtimeout.h>
#include <LUA/LUAutils.h>

#ifdef _WIN32
#include <LUACOM/LUACOMcreateDestroy.h>
#endif

#include <COL/COLlog.h>
#include <LAPI/LAPIapi.h>
#include <LNET/LNETcurl.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

void BLUAsetLuaRequirePath(lua_State* L, const COLstring& ProjectPath) {
   COL_FUNCTION(BLUAsetLuaRequirePath);
   COL_VAR(ProjectPath);
   // local lua files in the translator folder
   COLstring LuaPath = FILpathResolve(ProjectPath);
   LuaPath = FILpathAppend(LuaPath, "?.lua");
   COL_VAR(LuaPath);
   lua_getglobal(L, "package");
   LUApushString(L, LuaPath);
   lua_setfield(L, -2, "path"); // set the field "path" in table at -2 with value at top of stack
   // disable lua c libraries by setting cpath to an empty string
   LUApushString(L, "");
   lua_setfield(L, -2, "cpath"); // set the field "cpath" in table at -2 with value at top of stack
   lua_pop(L, 1); // get rid of package table from top of stack
}

static int BLUAluacomRequire(lua_State* L){
   COL_FUNCTION(BLUAluacomRequire);
#ifdef _WIN32 // luacom only works on Windows as COM is a Windows protocol
   COL_VAR((void*)L);
   COLstring ModuleName = LUAtoString(L, -1);
   COL_VAR(ModuleName);
   if (ModuleName == "luacom"){
      COL_TRC("Handle require for luacom");
      lua_pushcfunction(L, &LUACOMloadLuacom);
      return 1;
   } else {
      lua_pushfstring(L, "\n\tModule not handled by luacom loader");
      return 1;
   }
#endif
   return 0;
}

COLstring BLUAfdError(){
   COL_FUNCTION(BLUAfdError);
   int Limit = INFOfileHandleLimit();
   COLstring Message = "Iguana is currently limited to " + COLintToString(Limit) +
                       " file descriptors. Set the environment variable " + IFW_FD_LIMIT + " to a higher value and restart Iguana to fix this issue.";
   return Message;
}

void BLUAaddLuacomLoader(lua_State* L){
   COL_FUNCTION(BLUAaddLuacomLoader);
#ifdef _WIN32
   lua_getfield(L, LUA_GLOBALSINDEX, "package"); // push package
   lua_getfield(L, -1, "loaders"); // push package.loaders
   lua_pushinteger(L, 5); // push 2
   lua_pushcfunction(L, &BLUAluacomRequire);
   lua_settable(L, -3); // package.loaders[5]
   lua_pop(L, 2); // pop package.loaders and package
#endif
}