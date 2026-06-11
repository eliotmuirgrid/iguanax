// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBrollback
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 05 December 2022 - 11:34AM
// ---------------------------------------------------------------------------
#include <LDB/LDBrollback.h>

#include <COL/COLlog.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAtimeout.h>
#include <NODL/NODLconstants.h>
#include <NODL/NODLdbConnection.h>
COL_LOG_MODULE;

int LDBrollbackTransaction(lua_State* L){
   COL_FUNCTION(&LDBrollbackTransaction);
   try{      
      COLref<NODLdbConnection> pConn = NODLdbGetConnection(L, NODL_DB_TRANSACTION_OPTIONS);
      if (pConn->isOperationLive(L, true)){
         pConn->m_pDb->rollbackTransaction();
      }
      LUAcheckTimeout(L);
      return pConn->nonLiveConnectionWarning(L);
   } catch (const COLerror& Error) {
      LUAraiseLuaError(L, Error.description());
   }
   return 0;
}
