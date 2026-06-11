#ifndef __DB_DATABASE_ODBC_POSTGRE_SQL_H__
#define __DB_DATABASE_ODBC_POSTGRE_SQL_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcPostgreSQL
//
// Description:
//
// This class handles calls to PostgreSQL through the ODBC C API. This class 
// links the API with the common interface provided by the DBdatabase parent 
// class.
//
// Author:   Henry Tran
// Date:     November 18, 2004 
//
//
//---------------------------------------------------------------------------

#include "DBdatabaseOdbc.h"

class DBdatabaseOdbcPostgreSql :  public DBdatabaseOdbc
{
public:
   DBdatabaseOdbcPostgreSql() {}
   virtual ~DBdatabaseOdbcPostgreSql() {}

   DBdatabaseType databaseType() const { return DB_ODBC_POSTGRESQL; }
 
   virtual const COLstring& dbIntegerType() const;
   virtual const COLstring& dbDoubleType() const;
   virtual const COLstring& dbDateTimeType() const;

   virtual DBresultSetPtr fetchNonSystemDatabaseTables() const;
   bool isSystemTable( const char* TableName ) const;

   virtual COLostream& addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const;

   bool useWideChar() const;
   // returns true - driver config + set names utf8 + WideChar API
private:
   virtual bool supportsDescribeParam() const;

   DBdatabaseOdbcPostgreSql( DBdatabaseOdbcPostgreSql& Orig ); // not allowed
   DBdatabaseOdbcPostgreSql& operator=( DBdatabaseOdbcPostgreSql& Orig ); // not allowed
};

#endif // end of defensive include
