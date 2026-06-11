// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBset
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Wednesday 28 December 2022 - 12:06PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <IDB/IDBdatabaseConfig.h>
#include <IDB/IDBset.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKworker.h>
COL_LOG_MODULE;

static void IDBsetDriverPathsCallback(COLvar Result, COLaddress Address){
   COL_FUNCTION(IDBsetDriverPathsCallback);
   COL_VAR(Result);
	Result["success"].asBool() ? COLrespondSuccess(Address) : COLrespondError(Address, Result["error"].asString());
}

static void IDBsetDriverPathsWorker(const COLvar& In, COLvar* pOut) {
	COL_FUNCTION(IDBsetDriverPathsWorker);
	COLstring Error;
	const bool Success = FILsave(In["file"].asString(), In["config"].json(1), &Error);
	(*pOut)["success"] = Success;
	if(!Success) { (*pOut)["error"] = Error; }
}

static bool IDBcheckFilesExist(const COLstring& MySql, const COLstring& Oracle, const COLstring& Odbc, COLvar& Result){
   COL_FUNCTION(IDBcheckFilesExist);
   if (!MySql.is_null()){
      if (!FILfileExists(MySql)) { Result["issue"]["mysql"]  = COL_T("Error: Failed to find file. It may have been moved or deleted."); }
   }
   if (!Oracle.is_null()){
      if (!FILfileExists(Oracle)){ Result["issue"]["oracle"] = COL_T("Error: Failed to find file. It may have been moved or deleted."); }
   }
   if (!Odbc.is_null()){
      if (!FILfileExists(Odbc))  { Result["issue"]["odbc"]   = COL_T("Error: Failed to find file. It may have been moved or deleted."); }
   }
   return !Result.exists("issue");
} 

// /settings/db/set
void IDBsetDriverPaths(const COLwebRequest& Request, SCKloop* pLoop, IDBdatabaseConfig* pDatabaseConfig){
   COL_FUNCTION(IDBsetDriverPaths);
   COL_VAR(Request.Data);
   const COLstring& MySqlPath  = COLvarGet(Request.Data, "mysql",  "");
   const COLstring& OraclePath = COLvarGet(Request.Data, "oracle", "");
   const COLstring& OdbcPath   = COLvarGet(Request.Data, "odbc",   "");
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::DATABASE, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::DATABASE));
   }

   COLvar Result;
   if (!IDBcheckFilesExist(MySqlPath, OraclePath, OdbcPath, Result)) { // if any of the files doesn't exist we early return
      COL_TRC("Failed to find driver files");
      return COLrespondSuccess(Request.Address, Result); 
   }
   if (!MySqlPath.is_null()) { pDatabaseConfig->mySql().setPreferredMySqlDll(MySqlPath);    }
   if (!OraclePath.is_null()){ pDatabaseConfig->oracle().setPreferredOracleDll(OraclePath); }
   if (!OdbcPath.is_null())  { pDatabaseConfig->odbc().setPreferredOdbcDll(OdbcPath);       }
   COLvar Data;
	Data["user"]				 = Request.User;
	Data["file"]				 = DIRfileConfigDatabase();
	Data["config"]["mysql"]	 = pDatabaseConfig->mySql().preferredMySqlDll();  // config as it is the source of truth
	Data["config"]["oracle"] = pDatabaseConfig->oracle().preferredOracleDll();
	Data["config"]["odbc"]	 = pDatabaseConfig->odbc().preferredOdbcDll();
	SCKinvokeWorker(pLoop, IDBsetDriverPathsWorker, Data, COLnewClosure1(IDBsetDriverPathsCallback, Request.Address));
}
