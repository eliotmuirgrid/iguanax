// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCluaGet
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 17 September 2025 - 10:59AM
// ---------------------------------------------------------------------------

#include <DOC/DOCluaGet.h>
#include <DOC/DOCdocumentGet.h>

#include <LUA/LUAerror.h>

#include <COL/COLvar.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int DOCluaGet(lua_State* L){
   COL_FUNCTION(DOCluaGet);
   COLvar Data;
   Data.fromLua(L);
   COLvar Result;
   try{
      DOCdocumentGet(Data, &Result);
   }
   LUA_CATCH()
   // TODO - we probably need the Created hex string and an indication if the document existed already?
   Result.toLua(L);
   return 1;
}
