// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LTUNapi
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Friday 07 July 2022 - 01:30PM
// ---------------------------------------------------------------------------

#include <LTUN/LTUNapi.h>
#include <LTUN/LTUNpeerList.h>

#include <LUA/LUAutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void LTUNapi(lua_State* L){
   COL_FUNCTION(LTUNapi);
   lua_newtable (L);
   LUAaddMethod(L, "list", &LTUNpeerList);
   lua_setglobal(L, "peers");
}