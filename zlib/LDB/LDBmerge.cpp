// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBmerge
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 05 December 2022 - 11:34AM
// ---------------------------------------------------------------------------
#include <LDB/LDBmerge.h>

#include <DB/DBtransaction.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAtimeout.h>
#include <LUA/LUAtable.h>

#include <NOD/NODplace.h>
#include <NODL/NODLconstants.h>
#include <NODL/NODLdbConnection.h>
#include <NODL/NODLmerge.h>
#include <NODL/NODLpreviewListener.h>
#include <NODL/NODLutils.h>
#include <NTAB/NTABcollection.h>

#include <COL/COLauto.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

int LDBmerge(lua_State* L){
   COL_FUNCTION(LDBmerge);
   const int CountOfParam = lua_gettop(L);   
   try{      
      NODLdbConnectionParamToTable(L, "data");
      COLref<NODLdbConnection> pConn = NODLdbGetConnection(L, NODL_MERGE_OPTIONS);   
      const NODplace* pNodeData = NODLexpectedNodeParam(L, 1, "data");
      if (pNodeData){
         if (pNodeData->protocolType() != NTAB_TABLE_PROTOCOL || pNodeData->nodeType() != NTAB_ROOT){
            LUAraiseLuaError(L, COL_T("The 'data' parameter must be set to a table collection node produced from a call to db.tables()."));
         }
      }
      COL_VAR(*pNodeData);
      // Optional parameters   
      bool BulkInsert = LUAoptionalBoolParam(L, 1, "bulk_insert", false); 
      bool UseTransaction = LUAoptionalBoolParam(L, 1, "transaction", true);
      COL_VAR(BulkInsert);
      if (!pConn->isOperationLive(L, false)) {
         COL_TRC("Test env: skipping the actual insert");
         NODLpreview(L, *pNodeData, NODLpreviewListener::DB_TABLES);
         return pConn->nonLiveConnectionWarning(L);
      }
      if(BulkInsert 
         && DBdatabase::DB_DATABASE_SQL_SERVER_NAME != pConn->m_DatabaseApi 
         && DBdatabase::DB_DATABASE_MY_SQL_NAME     != pConn->m_DatabaseApi
         && DBdatabase::DB_DATABASE_MARIA_DB_NAME   != pConn->m_DatabaseApi) {
            LUAraiseLuaError(L, COL_T("Bulk inserts are only supported for MySQL and SQL Server."));
      }
      const NTABcollection* pRootNode = (const NTABcollection*)pNodeData;
      {         
         COLauto<DBtransaction> pTransaction = UseTransaction ? new DBtransaction(pConn->m_pDb.get()) : NULL;
         // NODLlog* pLog = NODLgetDebugSink(L);
         int Result = 0;
         if (!BulkInsert) {  
            COL_TRC ("Not using bulk insert");
            Result = NODLdoMergeTablesNotBulk(L, pConn->m_pDb, pRootNode);
         } else {
            if(DBdatabase::DB_DATABASE_SQL_SERVER_NAME == pConn->m_DatabaseApi) { 
               COL_TRC("Using SQL Server insert.");
               Result = NODLdoMergeTablesBulkSQLServer(L, pConn->m_pDb, pRootNode);
            }
            else if (DBdatabase::DB_DATABASE_MY_SQL_NAME == pConn->m_DatabaseApi) {
               COL_TRC("Using bind insert.");
               Result = NODLdoMergeTablesBulk(L, pConn->m_pDb, pRootNode);
            }
         }
         if (pTransaction != NULL) pTransaction->commit();
         return Result;
      }
   } catch(const COLerror& Error) {
      COL_VAR(Error);
      LUAraiseLuaError(L, Error.description());
   }
   COLASSERT (lua_gettop(L) == CountOfParam);  // Should not leave anything on stack.
   LUAcheckTimeout(L);
   return 0;
}
