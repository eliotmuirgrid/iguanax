// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBconnectionCheck
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Mon 12 December 2022
// ---------------------------------------------------------------------------
#include <LDB/LDBconnectionCheck.h>

#include <COL/COLlog.h>
#include <LUA/LUAutils.h>
#include <NODL/NODLdbConnection.h>
COL_LOG_MODULE;

int LDBconnectionCheck(lua_State* L) {
   COL_FUNCTION(LDBconnectionCheck);
   COLref<NODLdbConnection> pConn = NODLdbLoadConnectionHandle(L, false);
   lua_pushboolean(L, !pConn->m_Closed && pConn->m_pDb->isConnected());
   return 1;
}

