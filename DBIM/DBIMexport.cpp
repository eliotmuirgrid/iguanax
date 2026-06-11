// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMexport
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Tuesday 04 April 2023 - 10:39AM
// ---------------------------------------------------------------------------
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DB/DBdatabaseFactory.h>
#include <DB/DBdatabasePtr.h>
#include <DB/DBgrammar.h>
#include <DB/DBgrammarLoad.h>
#include <DB/DBgrammarSave.h>
#include <DB/DBresultSetPtr.h>
#include <DB/DBresultSetRow.h>
#include <DBIM/DBIMconnect.h>
#include <DBIM/DBIMexport.h>
#include <DBIM/DBIMutils.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static bool DBIMsplitQuery(COLarray<COLstring>* pQueryList, COLstring& Sql){
   COL_FUNCTION(DBIMsplitQuery);
   COLsplit(pQueryList, Sql, ";\n");
   for (int i = pQueryList->size() - 1; i > -1; i--){
      if ((*pQueryList)[i].isWhitespace()) { // Clean out bad user input or extra new lines
         COL_TRC("index " << i << " is whitespace - removing");
         pQueryList->remove(i);
      } else {
         (*pQueryList)[i] = COLstripWhiteSpace((*pQueryList)[i], true, true); // handle extra spaces
      }
   }
   return pQueryList->size() > 0; 
} 

static void DBIMrunCreateStatements(const COLarray<COLstring>& Queries, DBdatabasePtr pDatabase){
   COL_FUNCTION(DBIMrunCreateStatements);
   COLarray<DBresultSetPtr> ResultSetVector;
   DBresultSetPtr pResultSet;
   COL_VAR(Queries.size());
   for (int i = 0; i < Queries.size(); i++){
      COL_TRC("Running statement " << i);
      COL_VAR(Queries[i]);
      pResultSet = pDatabase->executeSqlString(Queries[i], 0, 0, &ResultSetVector);
   }
}

static bool DBIMexportSetDefaultDatasource(const COLvar& Params, COLstring* pError){
   COL_FUNCTION(DBIMexportSetDefaultDatasource);
   const COLstring& Vdb        = COLvarGet(Params, "filepath",   "");
   const COLstring& Component  = COLvarGet(Params, "component",  "");
   const COLstring& Datasource = COLvarGet(Params, "datasource", "");
   DBcollection Grammar;
   COLstring VdbFile;
   if (!DBIMreadVdbToDbCollection(Vdb, Component, &VdbFile, &Grammar, pError)) {
      return false;
   }
   if (Grammar.Export.size()) { Grammar.Export["datasource"] = Datasource;    } 
   else                       { Grammar.Export.add("datasource", Datasource); }
   COLstring VdbWithExport;
   DBsave(Grammar, &VdbWithExport);
   FILwriteFile(VdbFile, VdbWithExport);
   return true;
}

static void DBIMexportWorker(COLvar ConnectionParams, COLarray<COLstring> QueryList, DBdatabasePtr pDatabase, SCKloop* pLoop, COLaddress Address){
   COL_FUNCTION(DBIMexportWorker);
   const COLstring& Api        = COLvarGet(ConnectionParams, "type",        "");
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
   COL_TRC("Start transaction");
   try{
      pDatabase->beginTransaction();
      DBIMrunCreateStatements(QueryList, pDatabase);
      COL_TRC("All tables created successfully, committing transaction.");
      pDatabase->commitTransaction();
   } catch (const COLerror& Error){
      COL_TRC("Ran into an error - rolling back transaction.");
      pDatabase->rollbackTransaction();
      DBIMdisconnect(pDatabase);
      return COLrespondError(Address, Error.description());
   }
   COL_TRC("End transaction.");
   COL_TRC("Closing connection");
   DBIMdisconnect(pDatabase);
   if(!DBIMexportSetDefaultDatasource(ConnectionParams, &ErrorMessage)){
      return COLrespondError(Address, ErrorMessage);
   }
   return COLrespondSuccess(Address);
}

//  --- Main thread functions ---
static void DBIMexportImpl(const COLvar& InputParams, SCKloop* pLoop, const COLaddress& Address, DBdatabaseFactory* pDatabaseFactory){
   COL_FUNCTION(DBIMexportImpl);
   DBdatabasePtr pDatabase = pDatabaseFactory->getDatabase(InputParams["type"]);
   COLstring Sql = InputParams["sql"];
   COL_VAR(Sql);
   if (Sql.is_null()) { 
      return COLrespondError(Address, "Error: No sql commands detected.");
   }
   COLarray<COLstring> QueryList;
   if (!DBIMsplitQuery(&QueryList, Sql)){
      return COLrespondError(Address, "Error: Could not split provided sql into individual create table queries. Please make sure the entered sql is valid.");
   }
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&DBIMexportWorker, InputParams, QueryList, pDatabase, pLoop, Address));
}

// /database/export
void DBIMexport(const COLwebRequest& Request, SCKloop* pLoop, DBdatabaseFactory* pDatabaseFactory){
   COL_FUNCTION(DBIMexport);
   COL_VAR(Request.Data.json(1));
   const COLstring& Datasource = COLvarRequiredString(Request.Data, "datasource");
   const COLstring& Component  = COLvarRequiredString(Request.Data, "component");
   bool Development           = COLvarRequiredBool(Request.Data, "development");
   if(!Development)          { return COLrespondError(Request.Address, "Cannot export from a run repository."); }
   if (Datasource.is_null()) { return COLrespondError(Request.Address, "A datasource name must be provided."); }
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::DATABASE, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::DATABASE));
   }
   DBIMexportImpl(Request.Data, pLoop, Request.Address, pDatabaseFactory);
}