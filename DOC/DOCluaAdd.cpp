// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCluaAdd
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 25 August 2025 - 12:54PM
// ---------------------------------------------------------------------------

#include <DOC/DOCluaAdd.h>
#include <DOC/DOCdocument.h>
#include <DOC/DOChex.h>
#include <DOC/DOCserialize.h>
#include <DOC/DOCgrammar.h>
#include <DOC/DOCdocumentGrammar.h>
#include <DOC/DOCdir.h>
#include <DOC/DOCdocumentAdd.h>

#include <LUA/LUAutils.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAopen.h>

#include <FIL/FILutils.h>

#include <DIR/DIRpath.h>

#include <COL/COLvarUtils.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

int DOCluaAdd(lua_State* L) {
   COL_FUNCTION(DOCluaAdd);
   COLvar Data;
   COLvar Result;
   Data.fromLua(L);
   try{
      DOCdocumentAdd(Data, &Result);
   }
   LUA_CATCH()
   // TODO - we probably need the Created hex string and an indication if the document existed already?
   Result.toLua(L);
   return 1;
}