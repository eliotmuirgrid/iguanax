// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMgenerateSql
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Tuesday 07 March 2023 - 10:07AM
// ---------------------------------------------------------------------------
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLmap.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DB/DBdatabaseFactory.h>
#include <DB/DBdatabasePtr.h>
#include <DB/DBgrammar.h>
#include <DB/DBgrammarLoad.h>
#include <DB/DBresultSetPtr.h>
#include <DB/DBresultSetRow.h>
#include <DB/DBvariant.h>
#include <DBIM/DBIMconnect.h>
#include <DBIM/DBIMcreateTable.h>
#include <DBIM/DBIMgenerateSql.h>
#include <DBIM/DBIMutils.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

void DBIMmakeCreateStatements(COLarray<DBIMcreateTable>& TablesToCreate, DBcollection& Grammar){
   COL_FUNCTION(DBIMmakeCreateStatements);
   COLmap<COLstring, DBtable>::const_iterator it = Grammar.Tables.begin();
   for (it; it != Grammar.Tables.end(); it++){
      COL_TRC("Adding table " + it->first);
      DBIMcreateTable Table(it->first);
      for(int i = 0; i != it->second.Columns.size(); i++){
         COL_TRC("Adding column " + it->second.Columns[i].Name);
         DBIMtableColumn Column(it->second.Columns[i].Name, it->second.Columns[i].Type, it->second.Columns[i].IsKey);
         DBIMcreateTableAddColumn(&Table, Column);
      }
      TablesToCreate.push_back(Table);
   }
}

static bool DBIMcheckExistingTables(DBdatabasePtr pDatabase, const COLstring& Location, const COLstring& Datasource, COLmap<COLstring, bool>* pTables, COLstring* pErrorMessage, const COLstring& Username,  const COLstring& Password){
   COL_FUNCTION(DBIMcheckExistingTables);
   COL_TRC("Trying to establish database connection");
   COL_TRC("Connect to database");
   if(!DBIMconnect(Datasource, Username, Password, Location, pDatabase, pErrorMessage)){
      *pErrorMessage = "Failed to connect to database";
      return false;
   }
   DBresultSetPtr pResultSet;
   try{
      COL_TRC("Get list of tables");
      pResultSet = pDatabase->fetchDatabaseTables();
   } catch (const COLerror& Error){
      COL_TRC("Ran into an error fetching database tables");
      DBIMdisconnect(pDatabase);
      *pErrorMessage = Error.description();
      return false;
   }
   COL_TRC("Closing connection");
   DBIMdisconnect(pDatabase);
   for (int i = 0; i < pResultSet->countOfRow(); i++){
      DBresultSetRow CurrentRow = pResultSet->row(i);
      pTables->add(CurrentRow.columnValue(0).toString(), true);
   }
   return true;
}

static bool DBIMdatasourceExists(const COLstring& Datasource, const COLstring& Location){
   COL_FUNCTION(DBIMdatasourceExists);
   if (Datasource.is_null()) { return false; }
   COL_TRC("Checking path " << Datasource);
   bool Exists = FILfileExists(FILexpand(Datasource)) && !FILisDirectory(FILexpand(Datasource)); 
   if (!Exists){
      COL_TRC("Now trying to resolve path using " << Location );
      Exists = FILfileExists(FILpathAppend(Location, Datasource));
   } 
   return Exists;
}

static void DBIMgenerateSqlWorker(COLvar Params, SCKloop* pLoop, COLaddress Address, DBdatabaseFactory* pDatabaseFactory){
   COL_FUNCTION(DBIMgenerateSqlWorker);
   const COLstring& Filepath   = COLvarGet(Params, "filepath", "");
   const COLstring& Component  = COLvarGet(Params, "component", "");
   const COLstring& Type       = COLvarGet(Params, "type", "");
   const COLstring& Datasource = COLvarGet(Params, "datasource", "");
   const COLstring& Username = COLvarGet(Params, "username", "");
   const COLstring& Password = COLvarGet(Params, "password", "");
   const COLstring& Location = FILworkingDir();
   DBcollection Grammar;
   COLstring ErrorMessage, ResolvedVdb;
   COLvar Result;
   COL_TRC("Trying to read vdb into DBcollection");
   if (!DBIMreadVdbToDbCollection(Filepath, Component, &ResolvedVdb, &Grammar, &ErrorMessage)){ return COLrespondError(Address, ErrorMessage); }
	DBdatabasePtr pDatabase;
	try {
		pDatabase = pDatabaseFactory->getDatabase(Type);
	} catch (const COLerror& Error) {
		COL_TRC(Error.description());
		return COLrespondError(Address, Error.description());
	}
   COLmap<COLstring, bool> ExistingTables; // map for easy searching
   if (pDatabase->databaseName() == COL_T("SQLite")) {
      Result["dbExists"] = DBIMdatasourceExists(Datasource, Location);
   } else {
      if(!DBIMconnect(Datasource, Username, Password, Location, pDatabase, &ErrorMessage)){
         ErrorMessage = "Please input valid username or password to generate SQL";
         return COLrespondError(Address, ErrorMessage);
      }
      Result["dbExists"] = true; // Since connect succeeded, the DB exists.
   }
   COL_TRC(Result["dbExists"]);
   if (Result["dbExists"].asBool()) {
      if (!DBIMcheckExistingTables(pDatabase, Location, Datasource, &ExistingTables, &ErrorMessage, Username, Password)) {
         return COLrespondError(Address, ErrorMessage);
      }
   }
   COLarray<DBIMcreateTable> TablesToCreate;
   DBIMmakeCreateStatements(TablesToCreate, Grammar);
   Result["sql"] = DBIMgenerateCreateStatements(TablesToCreate, ExistingTables, pDatabase->databaseType());
   COLrespondSuccess(Address, Result);
}

static void DBIMgenerateSqlImpl(const COLvar& Params, const COLaddress& Address, SCKloop* pLoop, DBdatabaseFactory* pDatabaseFactory){
   COL_FUNCTION(DBIMgenerateSqlImpl);
   if(!Params["development"].asBool()) { 
      return COLrespondError(Address, "Must be in development mode to perform this operation."); 
   }
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&DBIMgenerateSqlWorker, Params, pLoop, Address, pDatabaseFactory));
}

// /database/generate_sql
void DBIMgenerateSql(const COLwebRequest& Request, SCKloop* pLoop, DBdatabaseFactory* pDatabaseFactory){
   COL_FUNCTION(DBIMgenerateSql);
   const COLvar& Input = Request.Data;
   COL_VAR(Input.json(1));
   const COLstring& Component = COLvarRequiredString(Input, "component");
   const COLstring& Type      = COLvarRequiredString(Input, "type");
   COL_TRC("Will generate create statements for " + Type);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, Component, ROLid::DATABASE, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::DATABASE));
   }
   DBIMgenerateSqlImpl(Input, Request.Address, pLoop, pDatabaseFactory);
}