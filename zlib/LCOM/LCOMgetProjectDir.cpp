// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMgetProjectDir
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 21 April 2025 - 10:47AM
// ---------------------------------------------------------------------------
#include <LCOM/LCOMgetProjectDir.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAopen.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int LCOMgetProjectDir(lua_State* L){
   if (lua_gettop(L) > 0) { LUA_ERROR_STREAM("No arguments needed for getProjectDir()"); }
   COLstring FileLocation = LUAprojectRoot(L);
   LUApushString(L, FileLocation);
   return 1;
}