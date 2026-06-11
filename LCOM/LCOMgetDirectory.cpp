//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LCOMgetDirectory.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   25/03/25 11:00 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <DIR/DIRpath.h>
#include <LCOM/LCOMgetDirectory.h>
#include <LUA/LUAopen.h>
#include <LUA/LUAutils.h>
COL_LOG_MODULE;

int LCOMgetDirectory(lua_State* L) {
   COL_FUNCTION(LCOMgetDirectory);
   const COLstring Component = LUAcomponentId(L);
   LUApushString(L, DIRcomponentDir(Component));
   return 1;
}
