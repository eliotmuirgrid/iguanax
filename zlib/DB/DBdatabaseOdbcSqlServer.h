#ifndef __DB_DATABASE_ODBC_SQL_SERVER_H__
#define __DB_DATABASE_ODBC_SQL_SERVER_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcSqlServer
//
// Description:
//
// This class handles calls to SQL Server through the ODBC C API. This class 
// links the API with the common interface provided by the DBdatabase parent 
// class.
//
// Author: Henry Tran
// Date:   February 23, 2004 
//
//---------------------------------------------------------------------------

#include "DBdatabaseOdbc.h"

class DBdatabaseOdbcSqlServer :  public DBdatabaseOdbc
{
public:
   DBdatabaseOdbcSqlServer() {}
   virtual ~DBdatabaseOdbcSqlServer() {}

   virtual DBdatabaseType databaseType() const { return DB_SQL_SERVER; }

   virtual const COLstring& dbDoubleType() const;

   virtual COLostream& addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const;

   // Override from DBdatabase
   void streamBinary(COLostream&, const COLstring&) const;

private:
   DBdatabaseOdbcSqlServer( DBdatabaseOdbcSqlServer& Orig ); // not allowed
   DBdatabaseOdbcSqlServer& operator=( DBdatabaseOdbcSqlServer& Orig ); // not allowed
};

#endif // end of defensive include
