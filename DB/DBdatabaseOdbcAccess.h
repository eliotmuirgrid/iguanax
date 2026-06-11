#ifndef __DB_DATABASE_ODBC_ACCESS_H__
#define __DB_DATABASE_ODBC_ACCESS_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcAccess
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

class DBdatabaseOdbcAccess : public DBdatabaseOdbc
{
public:
   DBdatabaseOdbcAccess() {}
   virtual ~DBdatabaseOdbcAccess() {}

   virtual DBdatabaseType databaseType() const { return DB_ODBC_ACCESS; }

   virtual const COLstring& dbIntegerType() const;
   virtual const COLstring& dbDoubleType() const;
   virtual const COLstring& dbTextType() const;
   virtual void streamDateTime( COLostream& Stream, const COLdateTime& DateTime ) const;

   virtual COLostream& addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const;

private:
   virtual bool supportsDescribeParam() const;

   virtual bool selectCastSupported() const;

   virtual bool createTableDefaultConstraintSupported() const;

   // Override to clear vector, since SQLprimaryKeys doesn't work for Access
   virtual void buildPrimaryKeyList( COLarray<COLstring>& PrimaryKeyList, const char* TableName ) const;

   DBdatabaseOdbcAccess( DBdatabaseOdbcAccess& Orig ); // not allowed
   DBdatabaseOdbcAccess& operator=( DBdatabaseOdbcAccess& Orig ); // not allowed
};

#endif // end of defensive include
