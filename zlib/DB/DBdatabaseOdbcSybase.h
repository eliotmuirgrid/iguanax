#ifndef __DB_DATABASE_ODBC_SYBASE_H__
#define __DB_DATABASE_ODBC_SYBASE_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcSybase
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

class DBdatabaseOdbcSybase : public DBdatabaseOdbc
{
public:
   DBdatabaseOdbcSybase() {}
   virtual ~DBdatabaseOdbcSybase() {}

   DBdatabaseType databaseType() const { return DB_ODBC_SYBASE; }

   static bool initialize();

   virtual bool selectUnionSeparateOrderBySupported() const;

   virtual COLostream& streamName( COLostream& Stream, const COLstring& Name, bool QuoteName ) const;

   bool useWideChar() const;

   
   virtual COLostream& addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const;

private:
   DBdatabaseOdbcSybase( DBdatabaseOdbcSybase& Orig ); // not allowed
   DBdatabaseOdbcSybase& operator=( DBdatabaseOdbcSybase& Orig ); // not allowed
};


#endif // end of defensive include
