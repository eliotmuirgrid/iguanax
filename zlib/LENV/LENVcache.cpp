// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LENVcache
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 30 January 2025 - 02:52PM
// ---------------------------------------------------------------------------

#include <LENV/LENVcache.h>
#include <ENV/ENVcatalogue.h>
#include <LUA/LUAvar.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLvar LENVgetEnvironment(){
   COL_FUNCTION(LENVgetEnvironment);
   COLvar EnvironmentVars;
   EnvironmentVars.setMapType();
   ENVretrieveAll(&EnvironmentVars);
   COL_VAR(EnvironmentVars);
   return EnvironmentVars;
}

void LENVcache(lua_State* L, const COLvar& EnvironmentVars){
   COL_FUNCTION(LENVcache);
   LUApushVar(L, EnvironmentVars);
   lua_setfield(L, LUA_REGISTRYINDEX, "_ENV_VAR_CACHE_");
}

void LENVgetCache(lua_State* L, COLvar* pCachedEnvironmentVars){
   COL_FUNCTION(LENVgetCache);
   lua_getfield(L, LUA_REGISTRYINDEX, "_ENV_VAR_CACHE_");
   if (!lua_istable(L, -1)) { 
      COL_TRC("Environment cache not found");
   }
   pCachedEnvironmentVars->fromLua(L, -1);
}