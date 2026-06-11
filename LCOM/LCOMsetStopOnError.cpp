// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMsetStopOnError
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 21 April 2025 - 10:30AM
// ---------------------------------------------------------------------------
#include <LCOM/LCOMsetStopOnError.h>

#include <BLUA/BLUAinstance.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAstartup.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int LCOMsetStopOnError(lua_State* L){
   COL_FUNCTION(LCOMsetStopOnError);
   luaL_argcheck(L, lua_type(L,1) == LUA_TBOOLEAN, 1, "boolean expected");
   bool Stop = lua_toboolean(L, 1);
   if (LUALisEnvironmentStartup(L)) return 0;
   BLUAinstance2* pInstance = BLUAinstanceGet(L);
   if (pInstance) BLUAinstanceStopOnError(pInstance, Stop); // do nothing in IDE mode
   return 0;
}