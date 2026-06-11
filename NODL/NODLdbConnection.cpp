// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODLdbConnection
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 24 November 2022
// ---------------------------------------------------------------------------
#include <NODL/NODLdbConnection.h>
#include <NODL/NODLconstants.h>
#include <NODL/NODLconnectionUtils.h>
#include <NODL/NODLenvironment.h>
#include <NODL/NODLutils.h>

#include <DBSQL/DBSQLdatabaseSqlite.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <LUAC/lua.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAutils.h>
#include <LUAL/LUALutils.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAopen.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

const char* NODLdatabaseNameArray[] = {
   DBdatabase::DB_DATABASE_MY_SQL_NAME,
   DBdatabase::DB_DATABASE_ODBC_POSTGRESQL_NAME,
   DBdatabase::DB_DATABASE_ODBC_SYBASE_NAME,
   DBdatabase::DB_DATABASE_ODBC_SYBASE_ASE_NAME,
   DBdatabase::DB_DATABASE_ODBC_ACCESS_NAME,
   DBdatabase::DB_DATABASE_SQL_SERVER_NAME,
   DBdatabase::DB_DATABASE_ODBC_DB2_NAME,
   DBdatabase::DB_DATABASE_ODBC_INFORMIX_NAME,
   DBdatabase::DB_DATABASE_ODBC_INTERBASE_NAME,
   DBdatabase::DB_DATABASE_ODBC_FILEMAKER_NAME,
   DBdatabase::DB_DATABASE_OCI_ORACLE_NAME,
   DBdatabase::DB_DATABASE_ODBC_ORACLE_NAME,
   DBdatabase::DB_DATABASE_SQLITE_NAME,
   DBdatabase::DB_DATABASE_MARIA_DB_NAME,
   DBdatabase::DB_DATABASE_ODBC_MYSQL_NAME
};

void luaErrorWithCode( const COLerror &Error, lua_State* L ) 
{
   // Instead of a simple string error, we create a table with two entries:
   // 'message': the error message string
   // 'code': error code 
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

static void NODLuserPasswordParam(lua_State* L, const COLstring& DbApi, COLstring& UserName, COLstring& Password){
   //some db's don't need user/pass
   if (DbApi != DBdatabase::DB_DATABASE_SQLITE_NAME){
      UserName = LUAexpectedStringParam(L, 1, NODL_DB_PARAM_USER);
      Password = LUAexpectedStringParam(L, 1, NODL_DB_PARAM_PASSWORD);
   }
}

COLstring NODLdatabaseApi(lua_State* L, int DbConstant){
   if (DbConstant < 1000 || DbConstant > 1015){
      COLstring ErrorMessage;
      COL_VAR(ErrorMessage);
      COL_VAR(DbConstant);
      COLostream Stream(ErrorMessage);
      Stream << COL_T("Invalid database api.  Valid choices are:\n" 
                      "   db.MY_SQL\n"
                      "   db.ORACLE_OCI\n"
                      "   db.ORACLE_ODBC\n"
                      "   db.SQL_SERVER\n"
                      "   db.POSTGRES\n"
                      "   db.DB2\n"
                      "   db.INFORMIX\n"
                      "   db.INTERBASE\n"
                      "   db.FILEMAKER\n"
                      "   db.SYBASE_ASA\n"
                      "   db.SYBASE_ASE\n"
                      "   db.SQLITE\n"
                      "   db.MARIA_DB\n"
                      "   db.MY_SQL_ODBC");
      LUAraiseLuaError(L, ErrorMessage);
   }
   return NODLdatabaseNameArray[DbConstant-1001];
}


NODLdbConnection::NODLdbConnection() :
  m_L(NULL), // initialized in getParamsFromCall
  m_ConnectionCaching(false),
  m_Live(false),
  m_Closed(false),
  m_DatabaseApiIndex(0),
  m_UseUnicode(0),
  m_TimeoutSeconds(-1),
  m_LastOperationLive(false),
  m_EncryptionKey("") // #30678
   { 
      COL_METHOD(NODLdbConnection::NODLdbConnection);
   }

NODLdbConnection::~NODLdbConnection() {
   COL_METHOD(NODLdbConnection::~NODLdbConnection);
   //NODLdbConnection is only passed by reference, so when it's destroyed we need to update connection count.
   //Cached connections are decremented elsewhere - either when explicitly closed or when inactive.
   if (m_L && !m_ConnectionCaching && !m_Closed && isConnectionLive(m_L)) {
      NODLdbDecrementConnectionCount(m_L);
   }
}

static void NODLdetermineSqlitePath(lua_State* L, COLstring& DataSource){
   COL_FUNCTION(NODLdetermineSqlitePath);
   COLstring ComponentDir = FILworkingDir();
   FILaddPathSeparator(ComponentDir);
   DataSource = DBSQLresolve(DataSource, ComponentDir);
}

//Gets connection parameters from parameter table for current method call.
void NODLdbConnection::getParamsFromCall(lua_State* L) {      
   COL_METHOD(NODLdbConnection::getParamsFromCall);
   m_L = L;

   // TODO some more polish could be added if we used a different helper routine to give an error message for invalid choices like '' or misspelling db.MY_SQL
   m_DatabaseApiIndex = NODLexpectedIntParam   (L, 1, NODL_DB_PARAM_API);
   m_DatabaseApi= NODLdatabaseApi(L, m_DatabaseApiIndex);
   m_DataSource = LUAexpectedStringParam(L, 1, NODL_DB_PARAM_NAME);
   if(m_DatabaseApi == DBdatabase::DB_DATABASE_SQLITE_NAME){ NODLdetermineSqlitePath(L, m_DataSource); }
   NODLuserPasswordParam(L, m_DatabaseApi, m_UserName, m_Password);
   // #30678
   //m_EncryptionKey = LUAoptionalStringParam(L, 1, NODL_DB_PARAM_ENCRYPTION_KEY, "");
   m_UseUnicode = LUAoptionalBoolParam(L, 1, NODL_DB_PARAM_UNICODE, false);
   m_TimeoutSeconds = LUAoptionalIntParam(L, 1, NODL_DB_PARAM_TIMEOUT, -1);      
   m_Live = LUAoptionalBoolParam(L, 1, NODL_DB_PARAM_LIVE, true);
   COL_VAR4(m_DatabaseApi, m_DataSource, m_UserName, m_Password);
   COL_VAR(m_EncryptionKey);
   COL_VAR2(m_DatabaseApiIndex, NODLdatabaseNameArray[m_DatabaseApiIndex - 1001]);
}

void NODLdbConnection::open(lua_State* L) {
   COL_METHOD(NODLdbConnection::open);
   try {
      COLPRECONDITION(!m_pDb.get());
      COL_VAR(m_EncryptionKey);
      m_pDb = NODLdbGetDatabase(L, m_DatabaseApi, m_DataSource, m_UserName, m_Password, m_UseUnicode, m_TimeoutSeconds, m_ConnectionCaching, m_EncryptionKey);
      if (isConnectionLive(L) && !m_pDb->isConnected()) { 
         m_pDb->connect(m_DataSource.c_str(), m_UserName.c_str(), m_Password.c_str(), m_EncryptionKey.c_str());
         NODLdbIncrementConnectionCount(L);            
      }
      COLPOSTCONDITION(m_pDb.get());         
   } catch (const COLerror& Error){
      luaErrorWithCode(Error, L);         
   }
}

void NODLdbConnection::close(lua_State* L) {
   COL_METHOD(NODLdbConnection::close);
   try {
      if (isConnectionLive(L)) {
         if (m_ConnectionCaching) m_pDb = NODLdbRemoveFromCache(L, m_DatabaseApi, m_DataSource, m_UserName, m_Password, m_UseUnicode, m_EncryptionKey);
         m_pDb->disconnect();
         NODLdbDecrementConnectionCount(L);
      }
      m_Closed = true;         
   } catch (const COLerror& Error){
      luaErrorWithCode(Error, L);         
   }
}

bool NODLdbConnection::isConnectionLive(lua_State* L) {
   COL_METHOD(NODLdbConnection::isConnectionLive);
   return !LUALisEnvironmentTest(L) || m_Live;
}

bool NODLdbConnection::isOperationLive(lua_State* L, bool LiveDefault) {      
   COL_METHOD(NODLdbConnection::isOperationLive);
   if (LUALisEnvironmentStartup(L)) { 
      m_LastOperationLive = false; // setting this just in case its read later
      return false; 
   }
   if (LUALisEnvironmentTest(L)) {
      if (m_ConnectionCaching) { //Assume not live, unless specified otherwise.
         m_LastOperationLive = LUAoptionalBoolParam(L, 1, NODL_DB_PARAM_LIVE, LiveDefault);
      } else {
         if (m_Live) {
            m_LastOperationLive = LUAoptionalBoolParam(L, 1, NODL_DB_PARAM_LIVE, LiveDefault);
         } else {
            LUAoptionalBoolParam(L, 1, NODL_DB_PARAM_LIVE, false); //Keeping this as a boolean parameter check, though we disregard its value.
            m_LastOperationLive = false;
         }
      }
   } else {
      m_LastOperationLive = true;
   }
   return m_LastOperationLive;
}

static int NODLdbNotLiveWarningDisplay(lua_State* L){   
   LUApushString(L, "Operation not live."); //Short label.
   LUApushString(L, ""); //No long label.
   LUApushString(L, NODLwarningDisplayType); //Warning icon.
   return 3;
} 

int NODLdbConnection::nonLiveConnectionWarning(lua_State* L) {
   COL_METHOD(NODLdbConnection::nonLiveConnectionWarning);
   if (m_LastOperationLive) { return 0; }
   lua_newuserdata(L, 0); //Push empty userdata - a vehicle for the warning message.
   if (luaL_newmetatable(L, NODL_DB_NON_LIVE_WARNING)) { //Push metatable.
      //Need to define the metatable.
      lua_pushcfunction(L, &NODLdbNotLiveWarningDisplay);
      lua_setfield(L, -2, "__display"); //Pop function.
   }
   lua_setmetatable(L, -2); //Pop metatable.
   return 1;
}

int NODLdbConnection::pushInfoTable(lua_State* L) {
   COL_METHOD(NODLdbConnection::pushInfoTable);
   //Pushes table to stack with connection info
   lua_newtable(L);
   lua_pushnumber(L, m_DatabaseApiIndex);
   lua_setfield(L, -2, NODL_DB_PARAM_API);
   lua_pushstring(L, m_DataSource.c_str());
   lua_setfield(L, -2, NODL_DB_PARAM_NAME);
   lua_pushstring(L, m_UserName.c_str());
   lua_setfield(L, -2, NODL_DB_PARAM_USER);
   lua_pushstring(L, m_Password.c_str());
   lua_setfield(L, -2, NODL_DB_PARAM_PASSWORD);
// #30678 - disabling this for 6.0.7
//      lua_pushstring(L, m_EncryptionKey.c_str());
//      lua_setfield(L, -2, NODL_DB_PARAM_ENCRYPTION_KEY);
   lua_pushboolean(L, m_UseUnicode);      
   lua_setfield(L, -2, NODL_DB_PARAM_UNICODE);
   lua_pushnumber(L, m_TimeoutSeconds);
   lua_setfield(L, -2, NODL_DB_PARAM_TIMEOUT);
   lua_pushboolean(L, m_Live);
   lua_setfield(L, -2, NODL_DB_PARAM_LIVE);
   return 1; //info table pushed to stack
}

//Retrieves and returns first method parameter as a connection handle (if it is one).
//If CheckClosed is true, does not allow use if connection has been closed.
COLref<NODLdbConnection> NODLdbLoadConnectionHandle(lua_State* L, bool CheckClosed) {
   COLref<NODLdbConnection> pConn = *(COLref<NODLdbConnection>*) LUAcheckUdata(L, 1, NODL_DB_CONNECTION_METATABLE, "Invalid database connection. Please supply a valid database connection.");
   if (CheckClosed && pConn->m_Closed) { LUAraiseLuaError(L, COL_T("Cannot use a closed connection.")); }
   return pConn;
}

//For current method call - checks parameters against possible ones and returns a connection.
//If connection provided as first parameter, removes it from parameters and returns it.
//If no more parameters exist, pushes an empty table onto the stack to allow conn:begin() to be used like conn:begin{}.
//Otherwise uses parameter table to open and return a connection, potentially using caching.
COLref<NODLdbConnection> NODLdbGetConnection(lua_State* L, const char* const MethodParams[]) {
   if(lua_isuserdata(L,1)) {    
      COLref<NODLdbConnection> pConn = NODLdbLoadConnectionHandle(L, true);
      lua_remove(L,1);      
      if (lua_isnone(L,1)) { //i.e., call looks like conn:method()
         lua_newtable(L); //push empty table onto stack
      }
      NODLcheckCombinedParams(L, 1, false, MethodParams);      
      return pConn;
   } else {
      COLref<NODLdbConnection> pConn(new NODLdbConnection());
      NODLcheckCombinedParams(L, 1, true, MethodParams);
      pConn->m_ConnectionCaching = true;
      pConn->getParamsFromCall(L);      
      pConn->open(L);
      return pConn;
   }
}

int NODLdbConnectionDestructor(lua_State* L) {
   COL_FUNCTION(NODLdbConnectionDestructor);
   // Check for programmer error; this should always be passed a
   // DB connection, so raise it as a failed assertion, not a user
   // error.
   void* ud = luaL_checkudata(L, 1, NODL_DB_CONNECTION_METATABLE);
   luaL_argcheck(L, ud != NULL, 1, "'db connection' expected");
   ((COLref<NODLdbConnection>*)ud)->~COLref<NODLdbConnection>(); // Call destructor
   return 0;
}

int NODLdbConnectionToString(lua_State* L) {
   COL_FUNCTION(NODLdbConnectionToString);     
   COLref<NODLdbConnection> pConn = NODLdbLoadConnectionHandle(L, false);
   COLstring Result;
   COLostream ResultStream(Result);
   ResultStream << COL_T("db.connection (");
   if (pConn->m_Closed) {
      ResultStream << COL_T("closed");
   } else {
      if (pConn->m_DatabaseApi == DBdatabase::DB_DATABASE_SQLITE_NAME && FILfileExists(pConn->m_DataSource)){
         // We have the absolute path to the sqlite file on disk, this is usually too long to display in translator so just display the file name.
         ResultStream << FILpathName(pConn->m_DataSource);
      } else { ResultStream << pConn->m_DataSource; }
   }
   ResultStream << ')';
   LUApushString(L, Result);
   return 1;
}
