//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LTEST.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   22/04/24 1:02 PM
//  ---------------------------------------------------------------------------
#include <LTEST/LTEST.h>
#include <LUTIL/LUTILfindPattern.h>

#include <LUA/LUAutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void LTESTaddTestNameSpace(lua_State* L){
	COL_FUNCTION(LTESTaddTestNameSpace);
	lua_newtable(L);
	LUAaddMethod(L, "search", &LUTILfindPattern);
	lua_setglobal(L, "test");
}
