// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMread
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Thursday 02 February 2023 - 03:20PM
// ---------------------------------------------------------------------------
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DB/DBdatabaseFactory.h>
#include <DB/DBdatabasePtr.h>
#include <DB/DBgrammar.h>
#include <DB/DBgrammarSave.h>
#include <DB/DBresultSetPtr.h>
#include <DB/DBresultSetRow.h>
#include <DB/DBvariant.h>
#include <DBIM/DBIMcolumnType.h>
#include <DBIM/DBIMconnect.h>
#include <DBIM/DBIMimportQuerySql.h>
#include <DBIM/DBIMread.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static bool DBIMisKey(const DBdatabaseType& DatabaseType, const DBvariant& Value){
   COL_FUNCTION(DBIMisKey);
   switch(DatabaseType){
   case DB_SQLITE:
      return 1 == Value.toInteger();
   case DB_SQL_SERVER:
      return COL_T("pk") == Value.toString().substr(0, 2);
   case DB_MY_SQL:
   case DB_MARIA_DB:
   case DB_ODBC_MYSQL:
      // Mysql, MariaDB have the convention that primary keys are "PRI"
      return COL_T("PRI") == Value.toString();
   case DB_OCI_ORACLE:
   case DB_ODBC_ORACLE:
      return COL_T("P") == Value.toString();
   case DB_ODBC_POSTGRESQL:
      return COL_T("Yes") == Value.toString();
   default:
      return false;
   }
}

//  --- Helper functions ---
static COLstring DBIMlistTablesAndColumns(const DBdatabaseType& DatabaseType, const COLstring& DatabaseName){
   COL_FUNCTION(DBIMlistTablesAndColumns);
   COL_VAR(DatabaseType);
   switch (DatabaseType){
      case DBdatabaseType::DB_SQLITE:
         return DBIMsqliteGetTableData(); // SQLite has one database per file - we only need to get all data for the provided connection.
      case DBdatabaseType::DB_SQL_SERVER:
         return DBIMsqlServerGetTableData(DatabaseName);
      case DBdatabaseType::DB_OCI_ORACLE:
      case DBdatabaseType::DB_ODBC_ORACLE:
         return DBIMoracleGetTableData();
      case DBdatabaseType::DB_MY_SQL:
      case DBdatabaseType::DB_MARIA_DB:
      case DBdatabaseType::DB_ODBC_MYSQL:
         return DBIMmysqlGetTableData(DatabaseName);
      case DBdatabaseType::DB_ODBC_POSTGRESQL:
         return DBIMpostgresGetTableData();
      default:
         return "";
   }
}

// Worker thread functions
static bool DBIMdbTablesAndColumnsToVdb(DBdatabaseType Type, DBresultSetPtr pResultSet, DBcollection* pVdb, COLstring* pError){
   COL_FUNCTION(DBIMdbTablesAndColumnsToVdb);
   // We should have a table where each row has the following data:
   // Table name, column name, column data type, whether the column is a primary key
   // We'll need to map the column data type to one of the types we support: string, integer, double, datetime
   COLstring CurrentTable;
   for (int i = 0; i < pResultSet->countOfRow(); i++){
      DBresultSetRow CurrentRow = pResultSet->row(i);
      if (CurrentRow.countOfColumnValue() != 4) { 
         COL_TRC("Error: The results from the database were returned in an unsupported format.");
         *pError = COL_T("Error: The results from the database were returned in an unsupported format.");
         return false;
      }
       if (CurrentRow.columnValue(0).dataType() != DB_STRING || CurrentRow.columnValue(1).dataType() != DB_STRING) {
         COL_TRC("Error: Did not get string data from database for table and column names.");
         *pError = COL_T("Error: Did not get string data from database for table and column names.");
         return false;
      }
      CurrentTable = CurrentRow.columnValue(0).toString();
      if (!pVdb->Tables.count(CurrentTable)){
         pVdb->Tables.add(CurrentTable, DBtable());
      }
      DBcolumn CurrentColumn = DBcolumn(CurrentRow.columnValue(1).toString(), COLstring(), 
                                        DBIMcolumnType(CurrentRow.columnValue(2)), DBIMisKey(Type, CurrentRow.columnValue(3)));
      pVdb->Tables[CurrentTable].Columns.push_back(CurrentColumn);
   }
   return true;
}

static void DBIMreadWorker(COLvar ConnectionParams, COLstring Sql, DBdatabasePtr pDatabase, SCKloop* pLoop, COLaddress Address){
   COL_FUNCTION(DBIMreadWorker);
   const COLstring& Api        = COLvarGet(ConnectionParams, "type",       "");
   const COLstring& Component  = COLvarGet(ConnectionParams, "component", "");
   const COLstring& Datasource = COLvarGet(ConnectionParams, "datasource", "");
   const COLstring& Database   = COLvarGet(ConnectionParams, "database",   Datasource); // for non-ODBC connections, database and datasource are the same thing
   const COLstring& Username   = COLvarGet(ConnectionParams, "username",   "");
   const COLstring& Password   = COLvarGet(ConnectionParams, "password",   "");
   const COLstring& Location   = FILworkingDir();
   COL_TRC("Trying to establish database connection");
   COLstring ErrorMessage;
   DBdatabaseType Type = pDatabase->databaseType();
   if(!DBIMconnect(Datasource, Username, Password, Location, pDatabase, &ErrorMessage)){
      return COLrespondError(Address, ErrorMessage);
   }
   COLarray<DBresultSetPtr> ResultSetVector;
   COL_TRC("Executing sql");
   DBresultSetPtr pResultSet;
   try {
      pResultSet = pDatabase->executeSqlString(Sql, 0, 0, &ResultSetVector);
   } catch (const COLerror& Error){
      return COLrespondError(Address, Error.description());
   }
   COL_VAR(pResultSet->countOfRow());
   if(pResultSet->countOfRow() == 0){
      COL_TRC("Could not find any tables in this database");
      ErrorMessage = "Error: Could not find any tables in this database.";
      DBIMdisconnect(pDatabase);
      return COLrespondError(Address, ErrorMessage);
   }
   COL_TRC("Closing connection");
   DBIMdisconnect(pDatabase);

   DBcollection Collection;
   COLstring Out;
   COLvar Result;
   if (!DBIMdbTablesAndColumnsToVdb(Type, pResultSet, &Collection, &ErrorMessage)){
      COL_TRC("Failed to convert schema to vdb format.");
      return COLrespondError(Address, ErrorMessage);
   } else {
      DBsave(Collection, &Out);
      Result.parse(Out);
      COL_TRC("Success");
      return COLrespondSuccess(Address, Result);
   }
}

//  --- Main thread functions ---
static void DBIMreadImpl(const COLvar& ConnectionParams, SCKloop* pLoop, const COLaddress& Address, DBdatabaseFactory* pDatabaseFactory){
   COL_FUNCTION(DBIMreadImpl);
   DBdatabasePtr pDatabase = pDatabaseFactory->getDatabase(ConnectionParams["type"]);
   const COLstring& Database = COLvarGet(ConnectionParams, "database", ConnectionParams["datasource"].asString());
   COLstring Sql = DBIMlistTablesAndColumns(pDatabase->databaseType(), Database);
   COL_VAR(Sql);
   if (Sql.is_null()) { 
      return COLrespondError(Address, "Error: Currently unsupported database type.");
   }
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&DBIMreadWorker, ConnectionParams, Sql, pDatabase, pLoop, Address));
}

// /database/read
void DBIMread(const COLwebRequest& Request, SCKloop* pLoop, DBdatabaseFactory* pDatabaseFactory){
   COL_FUNCTION(DBIMread);
   COL_VAR(Request.Data.json(1));
   const COLstring& Type       = COLvarRequiredString(Request.Data, "type");
   const COLstring& Component  = COLvarRequiredString(Request.Data, "component");
   const COLstring& Datasource = COLvarRequiredString(Request.Data, "datasource");
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::DATABASE, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::DATABASE));
   }
   COL_TRC("Connect to " << Type << " datasource " << Datasource);
   DBIMreadImpl(Request.Data, pLoop, Request.Address, pDatabaseFactory);
}