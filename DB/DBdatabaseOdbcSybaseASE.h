#ifndef __DB_DATABASE_ODBC_SYBASE_ASE_H__
#define __DB_DATABASE_ODBC_SYBASE_ASE_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcSybaseASE
//
// Description:
//
// This class handles calls to Sybase ASA through the ODBC C API. This class 
// links the API with the common interface provided by the DBdatabase parent 
// class.
//
// Author: Henry Tran, Nima Soltani
// Date:   March 18, 2005 
//
//---------------------------------------------------------------------------

#include "DBdatabaseOdbc.h"

class DBdatabaseOdbcSybaseASE : public DBdatabaseOdbc
{
public:
   DBdatabaseOdbcSybaseASE() {}
   virtual ~DBdatabaseOdbcSybaseASE(){}
   
   DBdatabaseType databaseType() const { return DB_ODBC_SYBASE_ASE; }

   virtual bool selectUnionSeparateOrderBySupported() const;

   virtual COLostream& streamName( COLostream& Stream, const COLstring& Name, bool QuoteName ) const;

   const COLstring& dbDoubleType() const;

   virtual COLostream& addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const;

private:

   virtual void adjustEmptyString(COLstring& ColumnData) const;

   virtual bool supportsDescribeParam() const;

   virtual bool supportsLongVarChar() const {return false;}
   
   virtual void streamCreateTableNotNull( COLostream& Stream, 
   const DBsqlCreateTable& SqlCreateTableCommand, COLuint32 ColumnIndex ) const;
   
   virtual void streamCreateTableNotNull2( COLostream& Stream, 
   const DBsqlCreateTable& SqlCreateTableCommand, COLuint32 ColumnIndex) const;

   DBdatabaseOdbcSybaseASE( DBdatabaseOdbcSybaseASE& Orig ); // not allowed
   DBdatabaseOdbcSybaseASE& operator=( DBdatabaseOdbcSybaseASE& Orig ); // not allowed
};


#endif // end of defensive include
