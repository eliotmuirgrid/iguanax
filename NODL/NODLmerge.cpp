// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODLconnectionUtils
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 05 December 2022
// ---------------------------------------------------------------------------
#include "NODLmerge.h"

#include <COL/COLlog.h>
#include <DB/DBdatabase.h>
#include <DB/DBdatabasePtr.h>
#include <DB/DBresultSetPtr.h>
#include <DB/DBsqlSelect.h>
#include <DB/DBsqlUpdate.h>
#include <NODL/NODLaction.h>
#include <NTAB/NTABcollection.h>
#include <NTAB/NTABtable.h>
COL_LOG_MODULE;

int NODLdoMergeTablesNotBulk(lua_State* L, DBdatabasePtr pDatabase, const NTABcollection* pRootNode){
   COL_FUNCTION(NODLdoMergeTablesNotBulk);
   const int NumberOfTables = pRootNode->size();
   COL_VAR(NumberOfTables);
   for(int TableIndex = 0; TableIndex < NumberOfTables; ++TableIndex){
      const NTABtable& Table = dynamic_cast<const NTABtable&>(pRootNode->child(TableIndex));
      COLuint64 TotalSqlSize = 0;
      const int NumberOfRows = Table.size();
      COL_TRC("Table: " << Table.name() << " with " << NumberOfRows << " rows");
      for(int RowIndex = 0; RowIndex < NumberOfRows; ++RowIndex){
         DBsqlSelect SqlSelectExistingRecords;
         DBsqlUpdate SqlInsertOrUpdate;
         //always start with insert, it's safer. Last thing we want is to accidentally make an update without a where clause....
         SqlInsertOrUpdate.setMakeIntoInsert();
         COLarray<COLstring> KeyFields;
         NODLinsertTableRowPrepare(Table, RowIndex, &SqlSelectExistingRecords, &SqlInsertOrUpdate, &KeyFields);
         COL_VAR(KeyFields.size());
         NODLinsertTableRowExecute(pDatabase, KeyFields, SqlSelectExistingRecords, SqlInsertOrUpdate, TotalSqlSize);
      }
      // traceDbMerge(L, pDatabase, Table.name(), TotalSqlSize);
   }

   return 0;
}

int NODLdoMergeTablesBulk(lua_State* L, DBdatabasePtr pDatabase, const NTABcollection* pRootNode){
   COL_FUNCTION(NODLdoMergeTablesBulk);
   COLstring SqlQuery;                              
   const int NumberOfTables = pRootNode->size();
   for(int TableIndex = 0; TableIndex < NumberOfTables; ++TableIndex){
      const NTABtable& Table = dynamic_cast<const NTABtable&>(pRootNode->child(TableIndex));
      if (Table.size()){
         COLostream SqlQueryStream (SqlQuery);            
         NODLdbCreateBulkStreamStatement(pDatabase, SqlQueryStream, Table, false);  
         // NODLlogSQL(SqlQuery, pLog);
         pDatabase->executeSqlString(SqlQuery);
         // traceDbMerge(L, pDatabase, Table.name(), SqlQuery.size());
      }
      SqlQuery.clear();
   }
   return 0;
}

int NODLdoMergeTablesBulkSQLServer(lua_State* L, DBdatabasePtr pDatabase, const NTABcollection* pRootNode){
   COL_FUNCTION(NODLdoMergeTablesBulkSQLServer);
   const int NumberOfTables = pRootNode->size();
   for(int i = 0; i < NumberOfTables; i++){
      const NTABtable& Table = dynamic_cast<const NTABtable&>(pRootNode->child(i));
      if (Table.size()){
         DBsqlInsert SqlInsertCommand;
         NODLdbCreateBulkInsertStatement(SqlInsertCommand, Table, true);
         const COLstring& Sql = NODLactionMakeSql(pDatabase, SqlInsertCommand);
         // NODLlogSQL(Sql, pLog);
         pDatabase->executeSqlInsert(SqlInsertCommand);
         // traceDbMerge(L, pDatabase, Table.name(), Sql.size());
      }
   }
   return 0;
}
