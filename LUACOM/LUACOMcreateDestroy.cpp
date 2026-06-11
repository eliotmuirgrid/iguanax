//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUACOMcreateDestroy
//
// Description:
// Implementation
//
// Author: Vismay Shah
// Date:   Friday, October 20th, 2023 @ 2:54:34 PM
//---------------------------------------------------------------------------

#include <LUACOM/LUACOMcreateDestroy.h>
#include <LUAC/lua.h>

#ifdef _WIN32
#include <LUACOM/luacom.h>
#endif

#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifdef _WIN32
static void LUACOMdestroyLuacom(lua_State* L){
   COL_FUNCTION(LUACOMdestroyLuacom);
   if (L){
      lua_getfield(L, LUA_REGISTRYINDEX, "luacom");
      if (lua_istable(L, -1)){
         COL_TRC("Free luacom resources.");
         luacom_close(L);
      }
   }
}

int LUACOMloadLuacom(lua_State* L){
   COL_FUNCTION(LUACOMloadLuacom);
   COL_TRC("About to call luacom_open");
   COL_VAR(lua_gettop(L));
   luacom_open(L);
   COL_VAR(lua_gettop(L));
   COL_TRC("Now getting the luacom table to the top of the stack");
   lua_getfield(L, LUA_REGISTRYINDEX, "luacom");
   COL_VAR(lua_istable(L, -1));
   return 1;
}
#endif

void LUACOMdestroyLuacomWrapper(lua_State* L, bool DestroyRequired){
   COL_FUNCTION(LUACOMdestroyLuacomWrapper);
   COL_VAR(DestroyRequired);
   if (DestroyRequired){
#ifdef _WIN32
      COL_TRC("On Windows and destroy required, calling destroy code.");
      LUACOMdestroyLuacom(L);
      return;
#endif
      COL_TRC("Destroy required but not on Windows - do nothing.");
      COL_TRC("If you're looking at translator, look for an accidental call to TRNannotationStateSetLuacom");
      COL_TRC("If you're looking at headless mode, don't worry :)");
   }
   return;
}
