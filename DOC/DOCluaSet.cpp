// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCluaSet
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 17 September 2025 - 10:59AM
// ---------------------------------------------------------------------------

#include <DOC/DOCluaSet.h>
#include <DOC/DOCdocumentSet.h>

#include <LUA/LUAerror.h>

#include <COL/COLvar.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int DOCluaSet(lua_State* L){
   COL_FUNCTION(DOCluaSet);
   COLvar Data;
   Data.fromLua(L);
   COLvar Result;
   try{
      DOCdocumentSet(Data, &Result);
   }
   LUA_CATCH()
   Result.toLua(L);
   return 1;
}