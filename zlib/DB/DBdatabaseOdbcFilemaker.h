#ifndef __DB_DATABASE_ODBC_FILEMAKER_H__
#define __DB_DATABASE_ODBC_FILEMAKER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcFilemaker
//
// Description:
//
// DBdatabaseOdbcFilemaker class.
//
// Author: Patrick Wisking
// Date:   Tuesday, July 28th, 2009 @ 04:33:15 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include "DBdatabaseOdbc.h"

class DBdatabaseOdbcFilemaker : public DBdatabaseOdbc
{
public:
   DBdatabaseOdbcFilemaker() {}
   virtual ~DBdatabaseOdbcFilemaker() {}

   virtual DBdatabaseType databaseType() const { return DB_ODBC_FILEMAKER; }

   virtual COLstring acceptableErrorString(COLuint32 ErrorIndex) const;

   virtual COLuint32 acceptableErrorStringSize() const;

   const COLstring& dbDateTimeType() const;

   const COLstring& dbIntegerType() const;

   const COLstring& dbDoubleType() const;

   const COLstring& dbStringType() const;

   virtual COLostream& addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const;

private:

   virtual bool supportsDescribeParam() const;

   virtual bool supportsLongVarChar() const {return false;}

   virtual bool supportsDefinePrimaryKey() const;

   DBdatabaseOdbcFilemaker(const DBdatabaseOdbcFilemaker& Orig); // not allowed
   DBdatabaseOdbcFilemaker& operator=(const DBdatabaseOdbcFilemaker& Orig); // not allowed
};


#endif // end of defensive include
