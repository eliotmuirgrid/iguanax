//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseFactory
//
// Description:
//
// Implementation.
//
// Author: Henry Tran
// Date:   November 27, 2003 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBdatabaseFactory.h"
#include "DBdatabasePtr.h"
#include "DBdatabaseType.h"

#include <COL/COLhashmap.h>
#include <COL/COLlocker.h>
#include <COL/COLmutex.h>

#include "DBdatabaseMySql.h"
#include "DBdatabaseOciOracle.h"
#include "DBdatabaseOdbcOracle.h"
#include "DBdatabaseOdbcPostgreSql.h"
#include "DBdatabaseOdbcMySql.h"
#include "DBdatabaseOdbcSybase.h"
#include "DBdatabaseOdbcSybaseASE.h"
#include "DBdatabaseOdbcDb2.h"
#include "DBdatabaseOdbcFilemaker.h"
#include "DBdatabaseOdbcInterbase.h"
#include "DBdatabaseOdbcInformix.h"
#include "DBdatabaseOdbcAccess.h"
#include "DBdatabaseOdbcSqlServer.h"

#include <DBSQL/DBSQLdatabaseSqlite.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

class DBdatabaseFactoryPrivate{
public:
   DBdatabaseFactoryPrivate() : IsInitialized(false) {}
   ~DBdatabaseFactoryPrivate() {}

   void addDatabase(DBcreateDb Creator, const char* pName);
   void initialize(COLostream&);
   COLarray<COLstring> supportedDatabases() { return DatabaseVector; }

   COLhashmap< COLstring, DBcreateDb > DatabaseCreateLookup;
   COLarray< COLstring > DatabaseVector;
   COLmutex ObjectSection;
   bool IsInitialized;
};

void (*DBdatabaseFactory::onInit)(DBdatabaseFactory& Factory) = 0;

DBdatabase* DBcreateSqlite(){
   COL_FUNCTION(DBcreateSqlite);
   return new DBSQLdatabaseSqlite();
}

DBdatabase* DBcreateMySql(){
   COL_FUNCTION(DBcreateMySql);
   return new DBdatabaseMySql(DB_MY_SQL);
}

DBdatabase* DBcreateMariaDb(){
   COL_FUNCTION(DBcreateMariaDb);
   //MariaDb is a drop in binary replacement for MySql,
   //So we can just use DBdatabaseMySql for both.
   return new DBdatabaseMySql(DB_MARIA_DB);
}

DBdatabase* DBcreateOciOracle(){
   COL_FUNCTION(DBcreateOciOracle);
   return new DBdatabaseOciOracle();
}

DBdatabase* DBcreateOdbcPostgresSql(){
   COL_FUNCTION(DBcreateOdbcPostgresSql);
   return new DBdatabaseOdbcPostgreSql();
}

DBdatabase* DBcreateOdbcMySql(){
   COL_FUNCTION(DBcreateOdbcMySql);
   return new DBdatabaseOdbcMySql();
}

DBdatabase* DBcreateOdbcDb2(){
   COL_FUNCTION(DBcreateOdbcDb2);
   return new DBdatabaseOdbcDb2();
}

DBdatabase* DBcreateOdbcFilemaker(){
   COL_FUNCTION(DBcreateOdbcFilemaker);
   return new DBdatabaseOdbcFilemaker();
}

DBdatabase* DBcreateOdbcInterbase(){
   COL_FUNCTION(DBcreateOdbcInterbase);
   return new DBdatabaseOdbcInterbase();
}

DBdatabase* DBcreateOdbcInformix(){
   COL_FUNCTION(DBcreateOdbcInformix);
   return new DBdatabaseOdbcInformix();
}

DBdatabase* DBcreateOdbcOracle(){
   COL_FUNCTION(DBcreateOdbcOracle);
   return new DBdatabaseOdbcOracle();
}

DBdatabase* DBcreateOdbcSybase(){
   COL_FUNCTION(DBcreateOdbcSybase);
   return new DBdatabaseOdbcSybase();
}

DBdatabase* DBcreateOdbcSybaseASE(){
   COL_FUNCTION(DBcreateOdbcSybaseASE);
   return new DBdatabaseOdbcSybaseASE();
}

DBdatabase* DBcreateOdbcAccess(){
   COL_FUNCTION(DBcreateOdbcAccess);
   return new DBdatabaseOdbcAccess();
}

DBdatabase* DBcreateOdbcSqlServer(){
   COL_FUNCTION(DBcreateOdbcSqlServer);
   return new DBdatabaseOdbcSqlServer(); 
}

void DBdatabaseFactory::initializeForThread(){
   COL_METHOD(DBdatabaseFactory::initializeForThread);
   COLlocker Lock(pMember->ObjectSection);
   //For now, only MySQL does this weirdness
   DBdatabaseMySql::initializeForThread();
}

void DBdatabaseFactory::uninitializeForThread(){
   COL_METHOD(DBdatabaseFactory::uninitializeForThread);
   COLlocker Lock(pMember->ObjectSection);
   DBdatabaseMySql::uninitializeForThread();
}

void DBdatabaseFactoryPrivate::addDatabase(DBcreateDb Creator, const char* pName){
   COL_METHOD(DBdatabaseFactoryPrivate::addDatabase);
   COL_VAR(*pName);
   COL_VAR(Creator);
   DatabaseCreateLookup.add(pName, Creator);
   DatabaseVector.push_back(pName);
   for(int i = 0; i < DatabaseVector.size(); i++){
      COL_VAR(DatabaseVector[i]);
   }
}

void DBdatabaseFactoryPrivate::initialize(COLostream& LogStream){
   COL_METHOD(DBdatabaseFactoryPrivate::initialize);
   DatabaseCreateLookup.clear();
   DatabaseVector.clear();

   COL_TRC("We can always add Sqlite");
   addDatabase(DBcreateSqlite, DBdatabase::DB_DATABASE_SQLITE_NAME);

   if (DBdatabaseMySql::isInitialized()){
      COL_TRC("Mysql compliant driver is initialized. Adding MySql and MariaDB databases");
      addDatabase(DBcreateMySql,   DBdatabase::DB_DATABASE_MY_SQL_NAME);
      addDatabase(DBcreateMariaDb, DBdatabase::DB_DATABASE_MARIA_DB_NAME);
   }
   if (DBdatabaseOciOracle::isInitialized()){
      COL_TRC("Oci Oracle compliant driver is initialized. Adding Oci Oracle database");
      addDatabase(DBcreateOciOracle, DBdatabase::DB_DATABASE_OCI_ORACLE_NAME);
   }
   if (DBdatabaseOdbc::isInitialized()){
      COL_TRC("ODBC driver is initialized. Adding ODBC databases");
      addDatabase(DBcreateOdbcPostgresSql, DBdatabase::DB_DATABASE_ODBC_POSTGRESQL_NAME);
      addDatabase(DBcreateOdbcMySql,       DBdatabase::DB_DATABASE_ODBC_MYSQL_NAME);
      addDatabase(DBcreateOdbcDb2,         DBdatabase::DB_DATABASE_ODBC_DB2_NAME);
      addDatabase(DBcreateOdbcFilemaker,   DBdatabase::DB_DATABASE_ODBC_FILEMAKER_NAME);
      addDatabase(DBcreateOdbcInterbase,   DBdatabase::DB_DATABASE_ODBC_INTERBASE_NAME);
      addDatabase(DBcreateOdbcInformix,    DBdatabase::DB_DATABASE_ODBC_INFORMIX_NAME);
      addDatabase(DBcreateOdbcOracle,      DBdatabase::DB_DATABASE_ODBC_ORACLE_NAME);
      addDatabase(DBcreateOdbcSybase,      DBdatabase::DB_DATABASE_ODBC_SYBASE_NAME);
      addDatabase(DBcreateOdbcSybaseASE,   DBdatabase::DB_DATABASE_ODBC_SYBASE_ASE_NAME);
      addDatabase(DBcreateOdbcAccess,      DBdatabase::DB_DATABASE_ODBC_ACCESS_NAME);
      addDatabase(DBcreateOdbcSqlServer,   DBdatabase::DB_DATABASE_SQL_SERVER_NAME);
   }
   IsInitialized = true;
}

DBdatabaseFactory::DBdatabaseFactory(){
   pMember = new DBdatabaseFactoryPrivate();
}

DBdatabaseFactory::~DBdatabaseFactory(){
   delete pMember;
}

void DBdatabaseFactory::initialize(COLostream& LogStream){
   COL_METHOD(DBdatabaseFactory::initialize);
   COLlocker Lock(pMember->ObjectSection);
   pMember->initialize(LogStream);
   if (onInit) onInit(*this);
}

void DBdatabaseFactory::addDatabase(DBcreateDb Creator, const char* pName){
   COL_METHOD(DBdatabaseFactory::addDatabase);
   COLlocker Lock(pMember->ObjectSection);
   pMember->addDatabase(Creator,pName);
}

bool DBdatabaseFactory::isInitialized() const{
   COL_METHOD(DBdatabaseFactory::isInitialized);
   COLlocker Lock(pMember->ObjectSection);
   return pMember->IsInitialized;
}

DBdatabasePtr DBdatabaseFactory::getDatabase( const COLstring& DatabaseAPI ){
   COL_METHOD(DBdatabaseFactory::getDatabase);
   COL_VAR(DatabaseAPI);
   COL_VAR2(pMember, pMember->IsInitialized);
   COLlocker Lock(pMember->ObjectSection);
   COLhashmapPlace LookupPlace = pMember->DatabaseCreateLookup.find( DatabaseAPI );
   if ( LookupPlace == NULL ){
      COL_ERROR_STREAM_PLAIN( COL_T("'") << DatabaseAPI << COL_T("' is not supported."), COLerror::PreCondition );
   }
   // Invoke the create function
   return pMember->DatabaseCreateLookup[LookupPlace](); 
}

bool DBdatabaseFactory::databaseSupported( const COLstring& DatabaseAPI ){
   COL_METHOD(DBdatabaseFactory::databaseSupported);
   COL_VAR(DatabaseAPI);
   COLlocker Lock(pMember->ObjectSection);
   COLhashmapPlace LookupPlace = pMember->DatabaseCreateLookup.find( DatabaseAPI );
   return LookupPlace != NULL;
}

COLarray<COLstring> DBdatabaseFactory::listDatabases(){
   COL_METHOD(DBdatabaseFactory::listDatabases);
   COLlocker Lock(pMember->ObjectSection);
   return pMember->supportedDatabases();
}

const COLstring& DBdatabaseFactory::databaseName( COLuint32 DatabaseIndex ){
   COL_METHOD(DBdatabaseFactory::databaseName);
   COLlocker Lock(pMember->ObjectSection);
   COLPRECONDITION( DatabaseIndex < pMember->DatabaseVector.size() )
   return pMember->DatabaseVector[ DatabaseIndex ];
}

COLuint32 DBdatabaseFactory::countOfDatabase(){
   COL_METHOD(DBdatabaseFactory::countOfDatabase);
   COLlocker Lock(pMember->ObjectSection);
   return pMember->DatabaseVector.size();
}
