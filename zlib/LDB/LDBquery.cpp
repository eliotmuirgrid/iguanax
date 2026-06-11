// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBquery
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 01 December 2022 - 03:56PM
// ---------------------------------------------------------------------------
#include <LDB/LDBquery.h>

#include <COL/COLlog.h>
#include <NODL/NODLdbExecute.h>
COL_LOG_MODULE;

int LDBquery(lua_State* L){
   COL_FUNCTION(LDBquery);
   return NODLdoDbExecute(L, true);
}

int LDBexecute(lua_State* L){
   COL_FUNCTION(LDBexecute);
   return NODLdoDbExecute(L, false);
}
