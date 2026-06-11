//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBmariaDb
//
// Description:
//
// MariaDB C API wrapper
//
//
// Author: Julian Muir
// Date:   May 03 2021 
//
//---------------------------------------------------------------------------

#include "DBmariaDb.h"
// #include "DBdriverMariaDb.h"
#include "DBresultSetPtr.h"

#include <COL/COLstring.h>
#include <COL/COLvar.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

DBmariaDb::DBmariaDb() {
   COL_METHOD(DBmariaDb::DBmariaDb);
}

DBmariaDb::~DBmariaDb() {
   COL_METHOD(DBmariaDb::~DBmariaDb);
}

bool DBmariaDb::setDriver(const COLstring& LibraryPath) {
   COL_METHOD(DBmariaDb::setDriver);
   COL_VAR(LibraryPath);
   
   m_Driver.loadLibrary(LibraryPath);
   if (m_Driver.isLoaded()) {
      COL_VAR2(m_Driver.getLibraryVersionString(), m_Driver.getLibraryVersionUint());
      return true;
   } else {
      COL_ERR("Could not load MariaDB Driver!");
      return false;
   }
}

void DBmariaDb::connect(const COLvar& ConnectionInfo) {
   COL_METHOD(DBmariaDb::connect);
   COL_VAR(ConnectionInfo);

   if (!m_Driver.isLoaded()) {
      COL_TRC("MariaDB Driver has not been loaded");
      return;      
   }
   COL_TRC("MariaDB Driver has been loaded");

   m_pMariaDbObject = m_Driver.mariadb_init(NULL);
   if (!m_pMariaDbObject) {
      COL_TRC("mariadb_init failed");
      return;
   }
   COL_TRC("MARIADB* initialized (mariadb_init succeeded)");

   COLstring Host           = ConnectionInfo["host"].asString();
   COLstring User           = ConnectionInfo["user"].asString();
   COLstring Password       = ConnectionInfo["password"].asString();
   COLstring DatabaseName   = ConnectionInfo.exists("database") ? ConnectionInfo["database"].asString() : COLstring("");
   int Port                 = ConnectionInfo.exists("port") ? ConnectionInfo["port"].asInt() : 0;
   COLstring UnixSocket     = ConnectionInfo.exists("socket") ? ConnectionInfo["socket"].asString() : COLstring("");
   unsigned long ClientFlag = ConnectionInfo.exists("clientFlag") ? (unsigned long) ConnectionInfo["clientFlag"].asInt() : 0;
   if (DatabaseName.is_null()) { DatabaseName.zero(); }
   MARIADB* pResult;
   if (UnixSocket.is_null()) {
      pResult = m_Driver.mariadb_real_connect(m_pMariaDbObject, Host.c_str(), User.c_str(), Password.c_str(), DatabaseName.c_str(), Port, NULL, ClientFlag);
   } else {
      pResult = m_Driver.mariadb_real_connect(m_pMariaDbObject, Host.c_str(), User.c_str(), Password.c_str(), DatabaseName.c_str(), Port, UnixSocket.c_str(), ClientFlag);
   }

   if (!pResult) {
      COL_VAR2(m_Driver.mariadb_errno(m_pMariaDbObject), m_Driver.mariadb_error(m_pMariaDbObject));
   } else {
      COL_VAR(m_Driver.mariadb_ping(m_pMariaDbObject)); // 0 = success
      COL_TRC("Connected to MariaDB!");
   }
}

void DBmariaDb::disconnect() {
   COL_METHOD(DBmariaDb::disconnect);
   if (!m_Driver.isLoaded()) {
      COL_TRC("MariaDB Driver has not been loaded");
      return;      
   }
   
   if (NULL != m_pMariaDbObject) {
      m_Driver.mariadb_close(m_pMariaDbObject);
      m_pMariaDbObject = NULL; //Nullify MARIADB* pointer so it uses a fresh value next time connect is called? 
      COL_TRC("MariaDB connection closed");
   } else {
      COL_TRC("No MariaDB connection to close");
   }
}

bool DBmariaDb::isConnected() {
   COL_METHOD(DBmariaDb::isConnected);
   if (!m_Driver.isLoaded()) {
      COL_TRC("MariaDB Driver has not been loaded");
      return false;      
   }

   if (NULL != m_pMariaDbObject) {
      int Result = m_Driver.mariadb_ping(m_pMariaDbObject);
      if (0 == Result) {
         COL_TRC("Connected to MariaDB database.");
         return true;
      } else {
         COL_VAR2(m_Driver.mariadb_errno(m_pMariaDbObject), m_Driver.mariadb_error(m_pMariaDbObject));
         return false;
      }
   } else {
      COL_ERR("MariaDBObject is null!");
      return false;
   }
}

DBresultSetPtr DBmariaDb::executeSqlString(const COLstring& SqlString, COLuint32 ResultSetStart, COLuint32 ResultSetSize, COLarray<DBresultSetPtr> *pAllResultSets) {
   COL_METHOD(DBmariaDb::executeSqlString);
   COL_VAR3(ResultSetStart, ResultSetSize, SqlString);

   if (!m_Driver.isLoaded()) {
      COL_TRC("MariaDB Driver has not been loaded");
      return NULL;      
   }

   if (!m_pMariaDbObject) {
      COL_ERR("No initialized MARIADB*");
      return NULL;
   }

   // TODO consider implementing mariadb_real_query instead
   // mariadb_real_query is slightly faster and supports binary data 
   if (0 < m_Driver.mariadb_query( m_pMariaDbObject, SqlString.c_str() ) ) {
      COL_ERR(COL_T("executeSqlString failed: ") << SqlString);
      COL_VAR2(m_Driver.mariadb_errno(m_pMariaDbObject), m_Driver.mariadb_error(m_pMariaDbObject));
      return NULL;
   }

   MARIADB_RES* pMariaDbResultSet = m_Driver.mariadb_store_result( m_pMariaDbObject );
   if ( NULL == pMariaDbResultSet && 0 < m_Driver.mariadb_errno(m_pMariaDbObject)) {
      COL_ERR(COL_T("executeSqlString mariadb_store_result failed: ") << SqlString);
      COL_VAR2(m_Driver.mariadb_errno(m_pMariaDbObject), m_Driver.mariadb_error(m_pMariaDbObject));
      return NULL; //An error occured
   } else if (NULL == pMariaDbResultSet) {
      return NULL; //Result is empty
   } else {
      //TODO reimplement
      //return pMember->fetchMariaDbResultSet( pMariaDbResultSet, ResultSetStart, ResultSetSize );
      m_Driver.mariadb_free_result(pMariaDbResultSet);
      return NULL; //TODO return a real value
   }
}