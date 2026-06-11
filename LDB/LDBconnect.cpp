// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBconnect
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thur 1 December 2022
// ---------------------------------------------------------------------------
#include <LDB/LDBconnect.h>

#include <COL/COLlog.h>
#include <LUAC/lua.h>
#include <LUAL/LUALutils.h>
#include <NODL/NODLconnectionUtils.h>
#include <NODL/NODLconstants.h>
#include <NODL/NODLdbConnection.h>
COL_LOG_MODULE;

int LDBconnect(lua_State* L){
   COL_FUNCTION(LDBconnect);
   NODLcheckCombinedParams(L, 1, true, NODL_CONNECT_OPTIONS);

   void* v = lua_newuserdata(L, sizeof(COLref<NODLdbConnection>)); // pushes new userdata   
   COLref<NODLdbConnection>* ppConn = new (v) COLref<NODLdbConnection>(new NODLdbConnection());   
   COLref<NODLdbConnection> pConn = *ppConn;

   pConn->m_ConnectionCaching = false;
   pConn->getParamsFromCall(L);
   pConn->open(L);

   luaL_getmetatable(L, NODL_DB_CONNECTION_METATABLE); // pushes NODL_DB_CONNECTION_METATABLE
   lua_setmetatable(L, -2);                            // assigns metatable to new user data above, and pops NODL_DB_CONNECTION_METATABLE

   return 1; //new connection pushed to stack
}

