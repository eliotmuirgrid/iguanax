// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAopen
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday 19 May 2023 - 04:10PM
// ---------------------------------------------------------------------------

#include <LUA/LUAopen.h>
#include <LUA/LUAheader.h>
#include <LUA/LUAutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

lua_State* LUAopen(LUAmode Mode) {
   COL_FUNCTION(LUAopen);
   lua_State* L = lua_open();
   COL_TRC("About to open core libraries.");
   luaL_openlibs(L);   // opens all standard Lua libraries into the state
   // This call back registers all the libraries so as not to make TRN and BLUA depend on them.
   // PLEASE DON'T ADD MORE LIBRARIES HERE - look at DBDluaRegister instead.
   LUArunLuaCallBack(L, Mode);
   return L;
}

void LUAsetIdAndRoot(lua_State* L, const COLstring& ComponentId, const COLstring& ProjectRoot) {
   COL_FUNCTION(LUAsetIdAndRoot);
   COL_VAR2(ComponentId, ProjectRoot);
   LUAsetStringField(L, "LUAcomponentId", ComponentId);
   LUAsetStringField(L, "LUAprojectRoot", ProjectRoot);
}

COLstring LUAcomponentId(lua_State* L){
   COL_FUNCTION(LUAcomponentId);
   return LUAgetRegistryString(L, "LUAcomponentId");
}

COLstring LUAprojectRoot(lua_State* L){
   COL_FUNCTION(LUAcomponentId);
   return LUAgetRegistryString(L, "LUAprojectRoot");
}