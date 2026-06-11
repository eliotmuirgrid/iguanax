// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODLdbExecute
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 24 November 2022
// ---------------------------------------------------------------------------
#include <NODL/NODLdbExecute.h>

#include <LND/LNDutils.h>

#include <LUAC/lua.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAtimeout.h>
#include <LUA/LUAtable.h>

#include <NOD/NODplace.h>
#include <NODB/NODBquery.h>
#include <NODB/NODBresultSet.h>
#include <NODL/NODLconstants.h>
#include <NODL/NODLdbConnection.h>
#include <NODL/NODLpreviewListener.h>
#include <NODL/NODLutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Instead of a simple string error, we create a table with two entries:
// 'message': the error message string
// 'code': error code 
static void NODLluaErrorWithCode( const COLerror &Error, lua_State* L ){
   COL_FUNCTION(NODLluaErrorWithCode);
   COLstring ErrorString; 
   COLostream ErrorStream(ErrorString); 
   ErrorStream << Error.description();
   long ErrorCode = Error.code();
   lua_newtable (L);
   lua_pushstring(L, "message");
   lua_pushstring(L, ErrorString.c_str());
   lua_settable(L, -3);

   lua_pushstring(L, "code");
   lua_pushnumber(L, ErrorCode);
   lua_settable(L, -3);
   // Assign a metatable to override the functions that convert the error table into strings
   lua_getfield(L, LUA_REGISTRYINDEX, NODL_DB_ERROR_TABLE_TO_STRING);
   lua_setmetatable(L, -2);
   lua_error (L);
}

// Note - the Iguana 6 code had a bunch of TRACE_QUERY macro calls that were not brought over as we no longer want that behaviour.
static NODplace* NODLexecuteQuery(lua_State* L, const DBdatabase& Database, const COLstring& Query, COLarray<DBresultSetPtr>& ResultSetVector){
   COL_FUNCTION(NODLexecuteQuery);
   NODplace* pResult;
   try {
      pResult = NODBexecuteQuery(Database, Query, &ResultSetVector);
   } catch(...) {
      throw;
   }
   return pResult;
}

static bool NODLdbSqlAltersDatabase(const COLstring& Sql){
   COL_FUNCTION(NODLdbSqlAltersDatabase);
   // we only do a crude heuristic to detect INSERT/UPDATE statements - TODO also dis-allow stored procedures.
   if (Sql.size() < 6) { return false; } // it's too small
   COLstring Substring = Sql.substr(0, COL_MIN(Sql.size(), 20));
   Substring.toLowerCase();
   Substring = COLstripWhiteSpace(Substring, true, false);
   COL_VAR(Substring);
   if (strncmp("insert", Substring.c_str(), 6) == 0){ return true; }
   if (strncmp("update", Substring.c_str(), 6) == 0){ return true; }
   return false;
}

int NODLdoDbExecute(lua_State* L, bool IsQuery){  
   COL_FUNCTION(NODLdoDbExecute);
   NODLdbConnectionParamToTable(L, "sql");
   COLref<NODLdbConnection> pConn = NODLdbGetConnection(L, NODL_EXECUTE_OPTIONS);   
   COLstring Sql = LUAexpectedStringParam(L, 1, "sql");
   COL_VAR(Sql);
   if (IsQuery && NODLdbSqlAltersDatabase(Sql)) {
      LUAraiseLuaError(L, COL_T("This query appears to alter the database.  This is not permitted\n"
         "since the call can be executed every time auto-completion or annotations are calculated.\n"
         "For INSERT/UPDATE and calling stored procedures please use the execute method instead.\n"
         "This call is not run by the editor."));
   }
   bool NoAct = !pConn->isOperationLive(L, IsQuery); //Query is 'live' by default, execute isn't.
   try{
      COL_TRC("Got database.");
      if (NoAct){ 
         NODLpreview(L, Sql, NODLpreviewListener::SQL_STRING, pConn->m_DatabaseApi); 
      } 
      COL_TRC("About to execute SQL");
      COLarray<DBresultSetPtr> ResultSetVector;
      COLref<NODplace> pLastResultSet;
      if (NoAct) {
         // Create a fake result for auto-completion.
         COLref<DBresultSet> FakeResultSet (new DBresultSet);
         pLastResultSet = new NODBresultSet(FakeResultSet.get());
         ResultSetVector.push_back(DBresultSetPtr (FakeResultSet.get()));
      } else {
         pLastResultSet = NODLexecuteQuery(L, *(pConn->m_pDb), Sql, ResultSetVector);
      }
      if (pLastResultSet.get()!=NULL) {
         // Push the last record set for backward compatibility.
         LNDpushNode(L, pLastResultSet.get());
         // Create a Lua table of all result sets 
         lua_newtable (L); 
         for (int i = 0; i < ResultSetVector.size (); i++) {
            LNDpushNode(L, new NODBresultSet(ResultSetVector[i].get()) );
            lua_rawseti(L, -2, i + 1);
         }
         // Assign a metatable to override the __tostring() call to pretty print the summary
         lua_getfield(L, LUA_REGISTRYINDEX, NODL_DB_RECORDSET_ARRAY_METATABLE);
         lua_setmetatable(L, -2);
         LUAcheckTimeout(L);
         return 2 + pConn->nonLiveConnectionWarning(L); // number of results
      } else {
         LUAcheckTimeout(L);
         return 0 + pConn->nonLiveConnectionWarning(L);
      }
   }
   catch (const COLerror& Error){
      NODLluaErrorWithCode(Error, L);
   }
   return 0;
}
