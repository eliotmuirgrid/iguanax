// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBclose
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thur 1 December 2022
// ---------------------------------------------------------------------------
#include <LDB/LDBclose.h>

#include <COL/COLlog.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAtimeout.h>
#include <NODL/NODLconnectionUtils.h>
#include <NODL/NODLconstants.h>
#include <NODL/NODLdbConnection.h>
COL_LOG_MODULE;

int LDBclose(lua_State* L){
   COL_FUNCTION(LDBclose);
   try{      
      COLref<NODLdbConnection> pConn = NODLdbGetConnection(L, NODL_DB_CLOSE_OPTIONS); //If cached, it still opens connection before we close it      
      pConn->close(L);      
      LUAcheckTimeout(L);
   } catch (const COLerror& Error){
      LUAraiseLuaError(L, Error.description());
   }
   return 0;
}

