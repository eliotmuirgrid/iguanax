// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOClapi
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 25 August 2025 - 12:48PM
// ---------------------------------------------------------------------------

#include <DOC/DOClapi.h>
#include <DOC/DOCluaAdd.h>
#include <DOC/DOCluaSet.h>
#include <DOC/DOCluaGet.h>
#include <DOC/DOCsearch.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void DOClapi(lua_State* L){
   COL_FUNCTION(DOClapi);
   lua_newtable (L);
   LUAaddMethod(L, "add",    &DOCluaAdd);
   LUAaddMethod(L, "get",    &DOCluaGet);
   LUAaddMethod(L, "set",    &DOCluaSet);
   LUAaddMethod(L, "search", &DOCluaSearch);
   
   lua_setglobal(L, "doc");
}