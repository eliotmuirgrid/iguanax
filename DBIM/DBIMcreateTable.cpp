// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMcreateTable
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Tuesday 07 March 2023 - 01:21PM
// ---------------------------------------------------------------------------

#include <DBIM/DBIMcreateTable.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLstring DBIMcreateStringType(const DBdatabaseType& ApiType){
   COL_FUNCTION(DBIMcreateStringType);
   switch (ApiType){
   case DB_MARIA_DB:
   case DB_MY_SQL:
   case DB_ODBC_MYSQL:
      return "VARCHAR(500)";
   case DB_SQL_SERVER:
      return "NVARCHAR(MAX)";
   case DB_SQLITE:
      return "TEXT";
   case DB_ODBC_POSTGRESQL:
      return "VARCHAR";
   case DB_OCI_ORACLE:
   case DB_ODBC_ORACLE:
      return "NVARCHAR2(500)";
   default:
      return "TEXT";
   }
}

static COLstring DBIMcreateIntegerType(const DBdatabaseType& ApiType){
   COL_FUNCTION(DBIMcreateIntegerType);
   switch (ApiType){
   case DB_MARIA_DB:
   case DB_MY_SQL:
   case DB_ODBC_MYSQL:
   case DB_ODBC_POSTGRESQL:
      return "INTEGER";
   case DB_OCI_ORACLE:
   case DB_ODBC_ORACLE:
   case DB_SQLITE:
   case DB_SQL_SERVER:
      return "INT";
   default:
      return "TEXT";
   }
}

static COLstring DBIMcreateDoubleType(const DBdatabaseType& ApiType){
   COL_FUNCTION(DBIMcreateDoubleType);
   switch (ApiType){
   case DB_MARIA_DB:
   case DB_MY_SQL:
   case DB_ODBC_MYSQL:
      return "DOUBLE";
   case DB_ODBC_POSTGRESQL:
      return "DOUBLE PRECISION";
   case DB_SQLITE:
      return "REAL";
   case DB_OCI_ORACLE:
   case DB_ODBC_ORACLE:
      return "NUMBER";
   case DB_SQL_SERVER:
      return "FLOAT";
   default:
      return "TEXT";
   }
}

static COLstring DBIMcreateDatetimeType(const DBdatabaseType& ApiType){
   COL_FUNCTION(DBIMcreateDatetimeType);
   switch (ApiType){
   case DB_SQLITE:
      return "TEXT";
   case DB_MARIA_DB:
   case DB_MY_SQL:
   case DB_ODBC_MYSQL:
   case DB_ODBC_POSTGRESQL:
      return "DATETIME";
   case DB_OCI_ORACLE:
   case DB_ODBC_ORACLE:
      return "DATE";
   case DB_SQL_SERVER:
      return "DATETIME2";
   default:
      return "TEXT";
   }
}

static COLstring DBIMquoteValue(const COLstring& Value, DBdatabaseType Type){
   COL_FUNCTION(DBIMquoteValue);
   char QuoteChar = '"';
   switch(Type){
      case DB_MY_SQL:
      case DB_MARIA_DB:
         QuoteChar = '`';
         break;
      case DB_SQLITE:
         QuoteChar = '"';
         break;
      default:
         break;
   }
   return QuoteChar + Value + QuoteChar;
}

static COLstring DBIMcreateColumnType(const DBsimpleType& ColumnType, const DBdatabaseType& ApiType){
   switch (ColumnType){
      case DBS_STRING:   return DBIMcreateStringType  (ApiType);
      case DBS_INTEGER:  return DBIMcreateIntegerType (ApiType);
      case DBS_DOUBLE:   return DBIMcreateDoubleType  (ApiType);
      case DBS_DATETIME: return DBIMcreateDatetimeType(ApiType);
      default:           return DBIMcreateStringType  (ApiType);
   }
}

void DBIMcreateTableAddColumn(DBIMcreateTable* pTable, DBIMtableColumn& Column){
   COL_FUNCTION(DBIMcreateTableAddColumn);
   pTable->m_Columns.push_back(Column);
}

static void DBIMaddDropTableLine(const COLstring& TableName, COLstring* pOut, const DBdatabaseType& ApiType){
   COL_FUNCTION(DBIMaddDropTableLine);
   *pOut += COL_T("DROP TABLE ") + DBIMquoteValue(TableName, ApiType) + COL_T(";\n"); // Do not put comments in sql it generates errors in sql generator.
   COL_VAR(ApiType);
}

COLstring DBIMgenerateCreateStatements(const COLarray<DBIMcreateTable>& Tables, const COLmap<COLstring, bool>& ExistingTables, const DBdatabaseType& ApiType){
   COL_FUNCTION(DBIMgenerateCreateStatements);
   COLstring Out;
   for (int i = 0; i < Tables.size(); i++){
      COLarray<COLstring> PrimaryKeyCols;
      if (ExistingTables.find(Tables[i].m_Name) != ExistingTables.end()){ 
         DBIMaddDropTableLine(Tables[i].m_Name, &Out, ApiType); 
      }
      Out.append(COL_T("CREATE TABLE ") + DBIMquoteValue(Tables[i].m_Name, ApiType) + COL_T("(\n"));
      for (int j = 0; j < Tables[i].m_Columns.size(); j++){
         Out.append(COL_T("\t") + DBIMquoteValue(Tables[i].m_Columns[j].m_ColumnName, ApiType) + COL_T(" "));
         Out.append(DBIMcreateColumnType(Tables[i].m_Columns[j].m_ColumnType, ApiType));
         if (Tables[i].m_Columns[j].m_IsKey) PrimaryKeyCols.push_back(Tables[i].m_Columns[j].m_ColumnName);
         if (PrimaryKeyCols.size()){
            Out.append(COL_T(",\n"));
         } else if (j != Tables[i].m_Columns.size() - 1) {
            Out.append(COL_T(",\n"));
         }
      }
      if (PrimaryKeyCols.size()){
         Out.append("\tPRIMARY KEY (");
         for (int k = 0; k < PrimaryKeyCols.size(); k++){
            Out.append(DBIMquoteValue(PrimaryKeyCols[k], ApiType));
            if (k != PrimaryKeyCols.size() - 1) Out.append(COL_T(", "));
         }
         Out.append(COL_T(")\n")); // close primary key
      } else {
         Out.append(COL_T("\n")); // newline after last column
      }
      Out.append(COL_T(");\n"));
   }
   COL_VAR(Out);
   return Out;
}
