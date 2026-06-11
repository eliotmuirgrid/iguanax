#include <DBIM/DBIMcheckSqlite.h>
#include <DBIM/DBIMimportQuerySql.h>

#include <FIL/FILutils.h>

#include <PRM/PRMcallbacks.h>

#include <ROL/ROLid.h>

#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <DB/DBdatabaseFactory.h>
#include <DB/DBresultSetPtr.h>
#include <DB/DBresultSetRow.h>
#include <DB/DBvariant.h>
#include <DBSQL/DBSQLdatabaseSqlite.h>
#include <SCK/SCKloop.h>

#include <COL/COLthreadPool.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void DBIMcheckSqliteTables(DBresultSetPtr pResultSet, COLvar* pOut) {
   COL_FUNCTION(DBIMcheckSqliteTables);
   COLstring CurrentTable;
   for (int i = 0; i < pResultSet->countOfRow(); i++){
      DBresultSetRow CurrentRow = pResultSet->row(i);
      CurrentTable = CurrentRow.columnValue(0).toString();
      COL_VAR(CurrentTable);
      (*pOut)["tables"].push_back(CurrentTable);
   }
}

static void DBIMcheckSqliteImpl(COLstring Filename, COLaddress Address) {
   COL_FUNCTION(DBIMcheckSqliteImpl);
   COLvar Result;
   Result["is_sqlite"] = false;
   COLstring Out;
   try {
      FILreadFile(Filename, &Out);
      if(Out.substr(0, 13) != "SQLite format") { return COLrespondSuccess(Address, Result); }
   } catch (const COLerror& Error) {
      COL_ERR(Error.description());
      return COLrespondError(Address, Error.description());
   }
   Result["is_sqlite"] = true;
   COL_TRC("Trying to establish database connection");
   COLstring ErrorMessage;
   DBSQLdatabaseSqlite Database;
   try {
      Database.connect(Filename.c_str(), "", "");
   } catch (const COLerror& Error) {
      return COLrespondError(Address, Error.description());
   }
   COLarray<DBresultSetPtr> ResultSetVector;
   DBresultSetPtr pResultSet;
   COLstring Sql = "SELECT name FROM sqlite_master WHERE type='table';";
   try {
      COL_TRC("Executing sql -- " << Sql);
      pResultSet = Database.executeSqlString(Sql, 0, 0, &ResultSetVector);
   } catch (const COLerror& Error){
      COL_ERR(Error.description());
      return COLrespondError(Address, Error.description());
   }
   COL_VAR(pResultSet->countOfRow());
   DBIMcheckSqliteTables(pResultSet, &Result);
   COL_TRC("Closing connection");
   Database.disconnect();
   COLrespondSuccess(Address, Result);
}

// /database/check_sqlite
void DBIMcheckSqlite(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(DBIMcheckSqlite);
   const COLstring FileName = COLvarRequiredString(Request.Data, "filename");
   const COLstring Component = COLvarRequiredString(Request.Data, "component");
   bool IsDev = COLvarRequiredBool(Request.Data, "development");
   ROLid Id = IsDev ? ROLid::EDIT_SCRIPT : ROLid::VIEW_SCRIPT;
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, Component, Id, &Error)){
      return COLrespondError(Request.Address, PRMsetError(Id));
   }
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&DBIMcheckSqliteImpl, FileName, Request.Address));
}