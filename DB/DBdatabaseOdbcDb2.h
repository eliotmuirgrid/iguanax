#ifndef __DB_DATABASE_ODBC_DB2_H__
#define __DB_DATABASE_ODBC_DB2_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcDb2
//
// Description:
//
// DBdatabaseOdbcDb2 class.
//
// Author: Patrick Wisking
// Date:   Monday, July 27th, 2009 @ 04:54:35 PM
//
//---------------------------------------------------------------------------

#include "DBdatabaseOdbc.h"

class DBdatabaseOdbcDb2 : public DBdatabaseOdbc 
{
public:
   DBdatabaseOdbcDb2() {}
   virtual ~DBdatabaseOdbcDb2() {}

   virtual DBdatabaseType databaseType() const { return DB_ODBC_DB2; }

   virtual const COLstring& dbKeyModifiers() const;

   virtual COLstring acceptableErrorString(COLuint32 ErrorIndex) const;

   virtual COLuint32 acceptableErrorStringSize() const;

   const COLstring& dbDateTimeType() const;

   virtual COLostream& addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const;

private:

   DBdatabaseOdbcDb2(const DBdatabaseOdbcDb2& Orig); // not allowed
   DBdatabaseOdbcDb2& operator=(const DBdatabaseOdbcDb2& Orig); // not allowed
};

#endif // end of defensive include
