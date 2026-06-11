// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBquote
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Mon 12 December 2022
// ---------------------------------------------------------------------------
#include <LDB/LDBquote.h>

#include <COL/COLlog.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAutils.h>
#include <NODL/NODLconnectionUtils.h>
#include <NODL/NODLdbConnection.h>
COL_LOG_MODULE;

int LDBquote(lua_State* L){
   COL_FUNCTION(LDBquote);
   // Expects one string parameter (after the connection), not a table;
   COLref<NODLdbConnection> pConn = NODLdbLoadConnectionHandle(L, false);
   if (!pConn->m_pDb.get()) { return LUAraiseLuaError(L, "No connection found to database."); }
   if (!lua_isstring(L, 2)) { return LUAraiseLuaError(L, "Second parameter must be a string."); }
   const COLstring& Data = LUAtoString(L, 2);
   COLstring Quoted;
   COLostream Stream(Quoted);
   pConn->m_pDb->addEscapedStringToStream(Stream, Data);
   lua_pushstring(L, Quoted.c_str());
   return 1;
}

