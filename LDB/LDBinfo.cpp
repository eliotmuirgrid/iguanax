// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBinfo
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Mon 12 December 2022
// ---------------------------------------------------------------------------
#include <LDB/LDBinfo.h>
#include <COL/COLlog.h>
#include <NODL/NODLdbConnection.h>
COL_LOG_MODULE;

int LDBconnectionInfo(lua_State* L) {
   COL_FUNCTION(LDBconnectionInfo);
   COLref<NODLdbConnection> pConn = NODLdbLoadConnectionHandle(L, false);
   return pConn->pushInfoTable(L);
}
