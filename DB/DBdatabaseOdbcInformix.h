#ifndef __DB_DATABASE_ODBC_INFORMIX_H__
#define __DB_DATABASE_ODBC_INFORMIX_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcInformix
//
// Description:
//
// DBdatabaseOdbcInformix class.
//
// Author: Pat Wisking
// Date:   Monday, July 27th, 2009 @ 12:00:03 PM
//
//---------------------------------------------------------------------------

#include "DBdatabaseOdbc.h"

class DBdatabaseOdbcInformix : public DBdatabaseOdbc 
{
public:
   DBdatabaseOdbcInformix(){}
   virtual ~DBdatabaseOdbcInformix() {}

   virtual DBdatabaseType databaseType() const { return DB_ODBC_INFORMIX; }

   virtual COLstring acceptableErrorString(COLuint32 ErrorIndex) const;

   virtual COLuint32 acceptableErrorStringSize() const;

   const COLstring& dbDoubleType() const;

   const COLstring& dbDateTimeType() const;

   virtual COLostream& addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const;
private:

   DBdatabaseOdbcInformix(const DBdatabaseOdbcInformix& Orig); // not allowed
   DBdatabaseOdbcInformix& operator=(const DBdatabaseOdbcInformix& Orig); // not allowed
};

#endif // end of defensive include
