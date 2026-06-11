#ifndef __DB_MY_SQL_H__
#define __DB_MY_SQL_H__
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


#include "DBdatabase.h"
#include "DBdriverMySql.h"

#include <COL/COLclosure.h>
#include <COL/COLvar.h>

class DBresultSetPtr;
class DBresultSetRow;
class DBmySqlDll;     // JULIAN is this needed = did not find in DBmySql.cpp

class DBmySql {//: public DBdatabase {
public:
   DBmySql();
   ~DBmySql();

   // Do we need this and is this the best way to do it?
   // DBdatabaseType databaseType() const { return m_ComponentType; }

   //Set the dynamic client library this instance of DBmySql will use
   bool setDriver(const COLstring& LibraryPath);

   void setLogger(COLclosure1<COLstring>* pCallback);

   void configureReconnect(int MaxAttempts, int WaitTime);

   // Initializes any required parameters/settings and connects the DBdatabase object to the DBMS.
   bool connect(const COLvar& ConnectionInfo);

   void disconnect();

   bool isConnected();

   // if MaxTries == 0 we won't retry, WaitTime is in milliseconds
   bool reconnect(int MaxTries, int WaitTime);


   // TODO escape the SQL string
   // NOT YET: NOTE: Input strings are assumed to be unescaped as this function will escape any special SQL characters for you.
   bool executeSqlString(const COLstring& SqlString, COLvar* pResultSet, COLvar* pResultInfo);

   // TODO investigate if these need implementation
   // Gut reaction is saying no because anyone needing this would just make a SQL call
   // bool getDatabases(COLvar* pDatabases);
   // bool getDatabaseTables(COLvar* pTableSet);
   // bool getNonSystemDatabaseTables(COLvar* pTableSet);
   // bool getTableColumnInfo(COLvar* pColoumnInfo);
   // bool isSystemTable(const COLstring& TableName);

   // TODO implement transaction support
   // bool beginTransaction();
   // void commitTransaction();
   // void rollbackTransaction();

   // TODO what problem are we solving?
   // Using UTF-8 encoding seems desirable, there should be a good internal spot to set the encoding
   // Do we want to expose setting the database encoding to the API?
   // bool useUnicode() const; //This function name will have to change

private:
   // //#5699 Making DB_TEXT on MySql -> LONGTEXT
   // const COLstring& dbTextType() const; //override DBdatabase

   void log(const COLstring& LogText);

   DBdriverMySql              m_Driver;
   MYSQL*                     m_pMySqlObject;
   COLclosure1<COLstring>*    m_pLogger;
   COLvar                     m_CachedConnectionInfo;            

   // TODO what exactly are these not allowing? Deep copies?
   DBmySql( DBmySql& Orig ); // not allowed
   DBmySql& operator=( DBmySql& Orig ); // not allowed
};

#endif // end of defensive include
