// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMgetId
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Wednesday 25 October 2023 - 02:51PM
// ---------------------------------------------------------------------------
#include <LCOM/LCOMgetId.h>

#include <LUA/LUAopen.h>
#include <LUA/LUAutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int LCOMgetId(lua_State* L){
   COL_FUNCTION(LCOMgetId);
   COLstring Guid = LUAcomponentId(L);
   COL_VAR(Guid);
   LUApushString(L, Guid);
   return 1;
}