//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBmysql
//
// Description:
//
// MySQL C API wrapper
//
//
// Author: Nicolas Lehman
// Date:   Tue Apr 27 2021
//
//---------------------------------------------------------------------------

#include "DBmySql.h"
#include "DBdriverMySql.h"
#include "DBmySqlType.h"

#include <COL/COLclosure.h>
#include <COL/COLstring.h>
#include <COL/COLvar.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
#include <COL/COLutils.h>
COL_LOG_MODULE;

DBmySql::DBmySql() {
   COL_METHOD(DBmySql::DBmySql);
   m_pLogger = NULL;
}

DBmySql::~DBmySql() {
   COL_METHOD(DBmySql::~DBmySql);
}

bool DBmySql::setDriver(const COLstring& LibraryPath) {
   COL_METHOD(DBmySql::setDriver);
   COL_VAR(LibraryPath);

   m_Driver.loadLibrary(LibraryPath);
   if (m_Driver.isLoaded()) {
      COL_VAR2(m_Driver.getLibraryVersionString(), m_Driver.getLibraryVersionUint());
      this->log(COL_T("Loaded MySql Driver version: ") + m_Driver.getLibraryVersionString());
      return true;
   } else {
      this->log(COL_T("Could not load MySQL Driver! At: ") + LibraryPath);
      return false;
   }
}

void DBmySql::setLogger(COLclosure1<COLstring>* pCallback) {
   COL_METHOD(DBmySql::setLogger);
   COL_VAR(pCallback);
   m_pLogger = pCallback;
}

bool DBmySql::connect(const COLvar& ConnectionInfo) {
   COL_METHOD(DBmySql::connect);
   COL_VAR(ConnectionInfo);

   if (!m_Driver.isLoaded()) {
      this->log("Connect called without loading MySql driver!");
      return false;
   }

   m_pMySqlObject = m_Driver.mysql_init(NULL);
   if (!m_pMySqlObject) {
      this->log(COL_T("Initializing MySql failed!") + m_Driver.mysql_error(m_pMySqlObject) + " Code: " + COLintToString(m_Driver.mysql_errno(m_pMySqlObject)));
      return false;
   }

   COLstring Host           = ConnectionInfo["host"].asString();
   COLstring User           = ConnectionInfo["user"].asString();
   COLstring Password       = ConnectionInfo["password"].asString();
   COLstring DatabaseName   = ConnectionInfo["database"].str();
   int Port                 = ConnectionInfo["port"].asInt();
   COLstring UnixSocket     = ConnectionInfo["socket"].str();
   unsigned long ClientFlag = (unsigned long) ConnectionInfo["clientFlag"].asInt();
   if (DatabaseName.is_null()) { DatabaseName.zero(); }
   MYSQL* pResult;
   if (UnixSocket.is_null()) { //NULL and "" don't produce the same behaviour in mysql_real_connect
      pResult = m_Driver.mysql_real_connect(m_pMySqlObject, Host.c_str(), User.c_str(), Password.c_str(), DatabaseName.c_str(), Port, NULL, ClientFlag);
   } else {
      pResult = m_Driver.mysql_real_connect(m_pMySqlObject, Host.c_str(), User.c_str(), Password.c_str(), DatabaseName.c_str(), Port, UnixSocket.c_str(), ClientFlag);
   }
   
   m_CachedConnectionInfo = ConnectionInfo;

   if (!pResult) {
      this->log(COL_T("MySql connection attempt failed: ") + m_Driver.mysql_error(m_pMySqlObject) + " Code: " + COLintToString(m_Driver.mysql_errno(m_pMySqlObject)));
      return false;
   } else {
      return this->isConnected();
   }
}

void DBmySql::disconnect() {
   COL_METHOD(DBmySql::disconnect);
   if (!m_Driver.isLoaded()) {
      this->log("Disconnect called without loading MySql driver.");
      return;
   }

   if (NULL != m_pMySqlObject) {
      m_Driver.mysql_close(m_pMySqlObject);
      m_pMySqlObject = NULL; //Nullify MYSQL* pointer so it uses a fresh value next time connect is called
      //Note this doesn't handle if connection was a unix socket not a port
      this->log(COL_T("Disconnected from MySql database: ") + m_CachedConnectionInfo["database"].asString() + " at " +
         m_CachedConnectionInfo["host"].asString() + ":" + m_CachedConnectionInfo["port"].asString());
   } else {
      this->log("Disconnect called but no MySql connection exists.");
   }
}

bool DBmySql::isConnected() {
   COL_METHOD(DBmySql::isConnected);

   if (!m_Driver.isLoaded()) {
      this->log("hasConnectionByGuid called without loading MySql driver.");
      return false;
   }

   int Result = m_Driver.mysql_ping(m_pMySqlObject);
   if (0 == Result) {
      COL_TRC(COL_T("Connected to MySql database: ") + m_CachedConnectionInfo["database"].asString() + " at " +
         m_CachedConnectionInfo["host"].asString() + ":" + m_CachedConnectionInfo["port"].asString());
      return true;
   } else {
      this->log(COL_T("Not connected to MySql Database: ") + m_Driver.mysql_error(m_pMySqlObject) + " Code: " +
      COLintToString(m_Driver.mysql_errno(m_pMySqlObject)));
      return false;
   }
}

bool DBmySql::reconnect(int MaxTries, int WaitTime) {
   COL_METHOD(DBmySql::reconnect);

   if (this->isConnected()) {
      COL_TRC(COL_T("Conneted to ") + m_CachedConnectionInfo["database"].asString() + " at " +
         m_CachedConnectionInfo["host"].asString() + ":" + m_CachedConnectionInfo["port"].asString());
      return true;
   }

   int RetryCount = 0;
   bool Connected = false;

   while (!Connected && MaxTries > RetryCount) {
      Connected = this->connect(m_CachedConnectionInfo);
      RetryCount++;
      if (0 < WaitTime) {
         COLsleep(WaitTime);
      }
   }

   if (Connected) {
      this->log(COL_T("Reconnected to MySql database") + m_CachedConnectionInfo["database"].asString() + " at " +
         m_CachedConnectionInfo["host"].asString() + ":" + m_CachedConnectionInfo["port"].asString() + " after " +
         COLintToString(RetryCount) + " attempts. Waited " + COLintToString(WaitTime) + "ms between attempts.");
      return true;
   } else {
      this->log(COL_T("Unable to reconnect to MySql database") + m_CachedConnectionInfo["database"].asString() + " at " +
         m_CachedConnectionInfo["host"].asString() + ":" + m_CachedConnectionInfo["port"].asString() + " after " +
         COLintToString(RetryCount) + " attempts. Waited " + COLintToString(WaitTime) + "ms between attempts.");
      return false;
   }
}

/*
COLvar ResultSet
ResultSet[ColumnName][<column-index>]
ResultSet[ColumnType][<column-index>]
ResultSet[<row-index>][<column-index]
*/
bool DBmySql::executeSqlString(const COLstring& SqlString, COLvar* pResultSet, COLvar* pResultInfo) {
   COL_METHOD(DBmySql::executeSqlString);

   if (!m_Driver.isLoaded()) {
      COL_TRC("MySql Driver has not been loaded");
      return false;
   }

   if (!m_pMySqlObject) {
      COL_ERR("No initialized MYSQL*");
      return false;
   }

   //if (0 < m_Driver.mysql_real_query(m_pMySqlObject, SqlString.c_str(), SqlString.size())) {
   if (0 < m_Driver.mysql_query(m_pMySqlObject, SqlString.c_str())) {
      COL_ERR(COL_T("executeSqlString failed: ") << SqlString);
      COL_VAR2(m_Driver.mysql_errno(m_pMySqlObject), m_Driver.mysql_error(m_pMySqlObject));
      return false;
   }

   MYSQL_RES* pMySqlResultStore = m_Driver.mysql_store_result(m_pMySqlObject);
   if (0 < m_Driver.mysql_errno(m_pMySqlObject)) {
      COL_ERR(COL_T("executeSqlString mysql_store_result failed: ") << SqlString);
      COL_VAR2(m_Driver.mysql_errno(m_pMySqlObject), m_Driver.mysql_error(m_pMySqlObject));
      return false; //An error occured
   } else if (NULL == pMySqlResultStore) {
      COL_TRC("Query returned no result set.");
      return true;
   } else {
      //Store result into COLvar
      COLvar ResultInfo, ResultSet;
      unsigned int NumFields = m_Driver.mysql_num_fields(pMySqlResultStore);
      MYSQL_FIELD* Fields;
      Fields = m_Driver.mysql_fetch_fields(pMySqlResultStore);

      for (int i = 0; i < NumFields; i++) {
         ResultInfo["ColumnName"][i] = Fields[i].name;
         ResultInfo["ColumnType"][i] = DBMySqlTypeAsString(Fields[i].type);
      }
      COL_VAR(ResultSet);
      COL_VAR(ResultInfo);

      unsigned long* Lengths;

      MYSQL_ROW ResultRow = m_Driver.mysql_fetch_row(pMySqlResultStore);
      int RowCount = 0;
      ResultSet.setArrayType();
      while (NULL != ResultRow) {
         for (int i = 0; i < NumFields; i++) {
            ResultSet[RowCount][i] = ResultRow[i];
         }
         ResultRow = m_Driver.mysql_fetch_row(pMySqlResultStore);
         RowCount++;
      }
      COL_VAR(ResultSet);
      *pResultSet = ResultSet;
      *pResultInfo = ResultInfo;
      return true;
   }
}

void DBmySql::log(const COLstring& LogText) {
   COL_METHOD(DBmySql::log);
   COL_VAR(LogText);
   if(m_pLogger) {
      m_pLogger->run(LogText);
   } else {
      COL_TRC("Logger not set");
   }
}
