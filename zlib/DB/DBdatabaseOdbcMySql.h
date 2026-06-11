#ifndef __DB_DATABASE_ODBC_MY_SQL_H__
#define __DB_DATABASE_ODBC_MY_SQL_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2016 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcMySQL
//
// Description:
//
// This class handles calls to MySQL through the ODBC C API. This class 
// links the API with the common interface provided by the DBdatabase parent 
// class.
//
// Author:   Meeral Qureshi
// Date:     July 11, 2016
//
//---------------------------------------------------------------------------

#include "DBdatabaseOdbc.h"

class DBdatabaseOdbcMySql :  public DBdatabaseOdbc
{
public:
   DBdatabaseOdbcMySql() {}
   virtual ~DBdatabaseOdbcMySql() {}

   DBdatabaseType databaseType() const { return DB_ODBC_MYSQL; }
 
   virtual const COLstring& dbIntegerType() const;
   virtual const COLstring& dbDoubleType() const;

   // NOTE : This is normally a limitation of MySQL, but not MyOdbc, so it's left out here
   // virtual DBresultSetPtr fetchNonSystemDatabaseTables() const;
   bool isSystemTable( const char* TableName ) const;

   virtual COLostream& addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const;

   // NOTE : useWideChar would only be required if "set names" is supported - MyOdbc 3.51+ does not support it
   //        The ODBC driver should instead be configured for unicode
   //        If that changes in the future, uncomment this
   // bool useWideChar() const;
private:
   virtual bool supportsDescribeParam() const;

   DBdatabaseOdbcMySql( DBdatabaseOdbcMySql& Orig ); // not allowed
   DBdatabaseOdbcMySql& operator=( DBdatabaseOdbcMySql& Orig ); // not allowed
};

#endif // end of defensive include
