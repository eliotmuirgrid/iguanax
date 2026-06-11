// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODLaction
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 8 December 2022
// ---------------------------------------------------------------------------
#include "NODLaction.h"

#include <COL/COLlog.h>
#include <DB/DBdatabaseMySql.h>
#include <DB/DBdatabasePtr.h>
#include <DB/DBresultSetPtr.h>
#include <DB/DBescape.h>
#include <DB/DBsqlInsert.h>
#include <DB/DBsqlSelect.h>
#include <DB/DBsqlUpdate.h>
#include <DB/DBsqlWhere.h>
#include <DB/DBsqlWhereCondition.h>
#include <DB/DBsqlWhereItem.h>
#include <DB/DBvariant.h>
#include <NODL/NODLutils.h>
#include <NTAB/NTABtable.h>
#include <NTAB/NTABtableItem.h>
#include <NTAB/NTABtableRow.h>
COL_LOG_MODULE;

static void NODLselectTablePrepare(const COLstring& ColumnName, const DBvariant& VariantValue, DBsqlSelect* pSqlSelectExistingRecords){
   COL_FUNCTION(NODLselectTablePrepare);
   pSqlSelectExistingRecords->addColumn(ColumnName, "", VariantValue.dataType() );
   // Select statement where condition
   if ( !pSqlSelectExistingRecords->whereClauseExists() ){
      pSqlSelectExistingRecords->addWhereClause();
   } else {
      pSqlSelectExistingRecords->whereClause().addItem().setItemAsCombiner(DBsqlWhereItem::AND_COMBINER );
   }
   DBsqlWhereCondition& SelectWhereCondition = pSqlSelectExistingRecords->whereClause().addItem().setItemAsCondition();
   SelectWhereCondition.setLeftOperandColumnName(ColumnName);
   SelectWhereCondition.setConditionOperator( DBsqlWhereCondition::EQUAL );
   SelectWhereCondition.addRightOperandValue(VariantValue);
}

static void NODLaddWhereToInsertTableRow(const COLstring&ColumnName, const DBvariant& VariantValue, DBsqlUpdate* pSqlInsertOrUpdate){
   COL_FUNCTION(NODLaddWhereToInsertTableRow);
   if ( !pSqlInsertOrUpdate->whereClauseExists() ){
      pSqlInsertOrUpdate->addWhereClause();
   } else {
      pSqlInsertOrUpdate->whereClause().addItem().setItemAsCombiner( DBsqlWhereItem::AND_COMBINER );
   }
   DBsqlWhereCondition& UpdateWhereCondition = pSqlInsertOrUpdate->whereClause().addItem().setItemAsCondition();
   UpdateWhereCondition.setLeftOperandColumnName(ColumnName);
   UpdateWhereCondition.setConditionOperator( DBsqlWhereCondition::EQUAL );
   UpdateWhereCondition.addRightOperandValue(VariantValue);
} 


static void NODLdbBulkInsertAddHeader(COLostream& Stream, const NTABtable& Table){
   COL_FUNCTION(NODLdbBulkInsertStreamAddHeader);
   COLstring TableName = Table.name();
   Stream << "INSERT INTO " << TableName << "(";
}

static void NODLdbBulkInsertAddHeader(DBsqlInsert& SqlInsertCommand, const NTABtable& Table){
   COL_FUNCTION(NODLdbBulkInsertAddHeader);
   const COLstring& TableName =  Table.name();
   SqlInsertCommand.setTableName(TableName);
   COL_TRC("Updating table " + TableName);
}

static void NODLdbBulkInsertAddColumnNames (COLostream& Stream, const NTABtable& Table){
   COL_FUNCTION(NODLdbBulkInsertStreamAddColumnNames);
   const NTABtableRow& TableRow = (NTABtableRow&)Table.child(0);
   int NumberOfColumns = TableRow.size();
   for (int ColumnIndex = 0; ColumnIndex < NumberOfColumns; ++ColumnIndex){
      COLstring ColumnName = TableRow.child(ColumnIndex).name();
      COL_VAR(ColumnName);
      Stream << ColumnName;
      if (ColumnIndex < NumberOfColumns - 1){ Stream << ", "; }
   }
   Stream << ")\n VALUES ";
}

static void NODLdbBulkInsertAddColumnNames (DBsqlInsert& SqlInsertCommand, const NTABtable& Table){
   COL_FUNCTION(NODLdbBulkInsertAddColumnNames);
 
   const NTABtableRow& TableRow = (NTABtableRow&)Table.child(0);
   int NumberOfColumns = TableRow.size();
   for (int ColumnIndex = 0; ColumnIndex < NumberOfColumns; ++ColumnIndex){
      const NTABtableItem& Datum = (NTABtableItem&)TableRow.child(ColumnIndex);
      COL_VAR2(ColumnIndex, Datum.value());
      DBvariant Variant;
      switch(Datum.nodeType()){
      case NTAB_STRING:
         Variant = Datum.value();
         break;
      case NTAB_DATETIME:
         Variant = NODLconvertStringToDateTime(Datum.value());
         break;
      case NTAB_DOUBLE:
         Variant = atof(Datum.value().c_str());
         break;
      case NTAB_INTEGER:
         Variant = atoi(Datum.value().c_str());
         break;
      default:
         Variant = Datum.value();
      }       

      SqlInsertCommand.addColumn(TableRow.child(ColumnIndex).name(),  Variant, false, false, Table.size());
   }
}
static void NODLdbBulkInsertAddRowValues (DBdatabasePtr pDBdatabase, COLostream& Stream, const NTABtable& Table, int RowIndex, bool LastRow, bool SimpleCompound){
   COL_FUNCTION(NODLdbBulkInsertStreamAddRowValues);
   Stream << '(';
   const NTABtableRow& TableRow = (NTABtableRow&)Table.child(RowIndex);
   int NumberOfColumns = TableRow.size();
   for (int DatumIndex = 0; DatumIndex < NumberOfColumns; ++DatumIndex){
      const NTABtableItem& Datum = (NTABtableItem&)TableRow.child(DatumIndex);
      switch(Datum.nodeType()){
         case NTAB_STRING:
            DBescapeMySql (Stream, Datum.value());
            break;
         case NTAB_DATETIME:
            Stream << pDBdatabase->dateTimeToString(NODLconvertStringToDateTime(Datum.value()));
            break;
         case NTAB_DOUBLE:
            Stream << atof(Datum.value().c_str());
            break;
         case NTAB_INTEGER:
            Stream << atoi(Datum.value().c_str());
            break;
         default:
            Stream << "Unknown data type!";
      }       
      if (DatumIndex < NumberOfColumns - 1){ Stream << ", "; }
   }
   if(!LastRow){ Stream << ')' << (SimpleCompound ? COL_T(";\n") : COL_T(",\n")); } 
   else        { Stream << ')' << (SimpleCompound ? ";" : " "); }
}

static void NODLdbBulkInsertAddRowValues (DBsqlInsert& SqlInsertCommand, const NTABtable& Table, int RowIndex, bool LastRow, bool SimpleCompound){
   COL_FUNCTION(NODLdbBulkInsertAddRowValues);
   int NumberOfRows = Table.size();
   int NumberOfColumns = Table.child(0).size();
   const NTABtableRow& TableRow = (NTABtableRow&)Table.child(RowIndex);
   for (int ColumnIndex = 0; ColumnIndex < NumberOfColumns; ++ColumnIndex){
      const NTABtableItem& Datum = (NTABtableItem&)TableRow.child(ColumnIndex);
      COL_VAR2(ColumnIndex, Datum.value());
      DBvariant Variant;
      switch(Datum.nodeType()){
      case NTAB_STRING:
         Variant = Datum.value();
         break;
      case NTAB_DATETIME:
         Variant = NODLconvertStringToDateTime(Datum.value());
         break;
      case NTAB_DOUBLE:
         Variant = atof(Datum.value().c_str());
         break;
      case NTAB_INTEGER:
         Variant = atoi(Datum.value().c_str());
         break;
      default:
         Variant = Datum.value();
      }       
      SqlInsertCommand.setColumnValue (ColumnIndex, RowIndex, Variant);
      COL_TRC("Added value " + Variant.toString() + " to column index " + ColumnIndex);
   }
}

static int NODLretrieveSelectedRowCount(DBdatabasePtr pDatabase, const DBsqlSelect& SqlSelectExistingRecords){
   COL_FUNCTION(NODLretrieveSelectedRowCount);
   // NODLlogSQL(makeSql(pDatabase, SqlSelectExistingRecords), pLog);
   return pDatabase->executeSqlSelect(SqlSelectExistingRecords)->countOfRow();
}

// Simple Compound statement is of this form: 
// INSERT INTO A FROM (col_a, col_b...) VALUES (1, 2...);
// INSERT INTO A FROM (col_a, col_b...) VALUES (3, 4...);
// ...
// Regular statement is of this form: 
// INSERT INTO A FROM (col_a, col_b...) VALUES
// (1, 2...), 
// (3, 4...),...
//
// Use simple with SQL server, and regular with MySQL
void NODLdbCreateBulkStreamStatement(DBdatabasePtr pDBdatabase, COLostream& Stream, const NTABtable& Table, bool SimpleCompound){
   COL_FUNCTION(NODLdbCreateBulkStreamStatement);
   NODLdbBulkInsertAddHeader(Stream, Table);
   NODLdbBulkInsertAddColumnNames (Stream, Table);
   // The values for each row
   int NumberOfRows = Table.size();
   for (int RowIndex = 0; RowIndex < NumberOfRows; ++RowIndex){
      NODLdbBulkInsertAddRowValues (pDBdatabase, Stream, Table, RowIndex, RowIndex >= NumberOfRows - 1, SimpleCompound);
      if (SimpleCompound){  
         NODLdbBulkInsertAddHeader(Stream, Table);
         NODLdbBulkInsertAddColumnNames (Stream, Table);
      }
   }
}

void NODLinsertTableRowExecute(DBdatabasePtr pDatabase, const COLarray<COLstring>& KeyFields, const DBsqlSelect& SqlSelectExistingRecords, DBsqlUpdate& SqlInsertOrUpdate, COLuint64& TotalSqlSize){
   COL_FUNCTION(NODLinsertTableRowExecute);
   COL_VAR(KeyFields.size());
   if (KeyFields.size()){
      // NODL_DEBUG_LOG(pLog, "Selecting for existing records because key fields exist.");
      if (0 == NODLretrieveSelectedRowCount(pDatabase, SqlSelectExistingRecords)){
         SqlInsertOrUpdate.setMakeIntoInsert();
      } else {
         SqlInsertOrUpdate.setMakeIntoUpdate();
         int NumberOfKeys = KeyFields.size();
         for (int KeyIndex = 0; KeyIndex < NumberOfKeys; ++KeyIndex){
            SqlInsertOrUpdate.removeColumn(KeyFields[KeyIndex] );
         }
         if(0 == SqlInsertOrUpdate.countOfColumn()){
            // NODL_DEBUG_LOG(pLog, "No change to database since the table consists only of key columns."); 
            return;
         }
      }
   }
   const COLstring& Sql = NODLactionMakeSql(pDatabase, SqlInsertOrUpdate);
   TotalSqlSize += Sql.size();
   if(DB_INSERT_CMD == SqlInsertOrUpdate.commandType()){
      // NODL_DEBUG_LOG(pLog, (KeyFields.size() ? "Record does not exist so doing an insert." : "Inserting new record."));
      // NODLlogSQL(Sql, pLog);
      pDatabase->executeSqlInsert(SqlInsertOrUpdate);
   } else {
      //extremely important that we don't do an update across the whole table!
      COLPRECONDITION(SqlInsertOrUpdate.whereClause().countOfItem() > 0);
      // NODL_DEBUG_LOG(pLog, "Record exists so doing an update.");
      // NODLlogSQL(Sql, pLog);
      pDatabase->executeSqlUpdate(SqlInsertOrUpdate);
   }
}

void NODLinsertTableRowPrepare(const NTABtable& Table, int RowIndex, DBsqlSelect* pSqlSelectExistingRecords, DBsqlUpdate* pSqlInsertOrUpdate, COLarray<COLstring>* pKeyFields){
   COL_FUNCTION(NODLinsertTableRowPrepare);
   COLstring TableName = Table.name();
   pSqlInsertOrUpdate->setTableName(TableName);
   pSqlSelectExistingRecords->addTable(TableName);
   COL_VAR(TableName);
   const NTABtableRow& Row = (NTABtableRow&)Table.child(RowIndex);
   int NumberOfColumns = Row.size();
   for (int i = 0; i < NumberOfColumns; i++){
      const NTABtableItem& Datum = (NTABtableItem&)Row.child(i);
      DBvariant VariantValue;
      if(Datum.isNull()) {
         COL_TRC("Datum null");
         // Do nothing.  DBvariants are NULL by default.
      } else switch(Datum.nodeType()){
      case NTAB_STRING:
         VariantValue = Datum.value();
         break;
      case NTAB_DATETIME:
         VariantValue = NODLconvertStringToDateTime(Datum.value());
         break;
      case NTAB_DOUBLE:
         VariantValue = atof(Datum.value().c_str());
         break;
      case NTAB_INTEGER:
         VariantValue = atoi(Datum.value().c_str());
         break;
      default:
         VariantValue = Datum.value();
      }       

      const COLstring& ColumnName = Datum.name(); 
      pSqlInsertOrUpdate->addColumn(ColumnName, VariantValue);
      COL_VAR(ColumnName);
      if (Datum.isKey()){
         COL_TRC("Adding key field " << ColumnName);
         pKeyFields->push_back(ColumnName);
         NODLselectTablePrepare(ColumnName, VariantValue, pSqlSelectExistingRecords);
         NODLaddWhereToInsertTableRow(ColumnName, VariantValue, pSqlInsertOrUpdate);
      }
   }
}

void NODLdbCreateBulkInsertStatement(DBsqlInsert& SqlInsertCommand, const NTABtable&Table, bool SimpleCompound){
   COL_FUNCTION(NODLdbCreateBulkInsertStatement);
   NODLdbBulkInsertAddHeader(SqlInsertCommand, Table);
   NODLdbBulkInsertAddColumnNames (SqlInsertCommand, Table);
   int NumberOfRows = Table.size();
   for (int RowIndex = 0; RowIndex < NumberOfRows; ++RowIndex){
      NODLdbBulkInsertAddRowValues (SqlInsertCommand, Table, RowIndex, RowIndex >= NumberOfRows - 1, SimpleCompound);
   }
}

COLstring NODLactionMakeSql(const DBdatabasePtr pDatabase, const DBsqlCommand& Cmd){
   COLstring Sql;
   COLostream Stream(Sql);
   pDatabase->createSqlCommandWithoutBinding(Cmd, Stream);
   return Sql;
}