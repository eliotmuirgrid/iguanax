#ifndef __DB_DATABASE_ODBC_INTERBASE_H__
#define __DB_DATABASE_ODBC_INTERBASE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcInterbase
//
// Description:
//
// DBdatabaseOdbcInterbase class.
//
// Author: Pat Wisking
// Date:   Tuesday, July 21st, 2009 @ 12:11:56 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include "DBdatabaseOdbc.h"

//May be inherited some day (by Firebird?)

class DBdatabaseOdbcInterbase : public DBdatabaseOdbc 
{
public:
   DBdatabaseOdbcInterbase() {}
   virtual ~DBdatabaseOdbcInterbase() {}

   virtual DBdatabaseType databaseType() const { return DB_ODBC_INTERBASE; }

   virtual const COLstring& dbDoubleType() const;

   virtual const COLstring& dbDateTimeType() const;

   virtual const COLstring& dbTextType() const;
       
   virtual const COLstring& dbStringMaxKeyLength() const;

   virtual const COLstring& dbKeyModifiers() const;

   virtual bool selectCastSupported() const;

   virtual COLstring acceptableErrorString(COLuint32 ErrorIndex) const;

   virtual COLuint32 acceptableErrorStringSize() const;

   DBresultSetPtr fetchNonSystemDatabaseTables() const;
   DBresultSetPtr fetchDatabaseTables() const;
   bool isSystemTable(const char* TableName) const;

   bool useWideChar() const;

   virtual COLostream& addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const;

private:

   virtual bool supportsDefault() const;
   DBdatabaseOdbcInterbase(const DBdatabaseOdbcInterbase& Orig); // not allowed
   DBdatabaseOdbcInterbase& operator=(const DBdatabaseOdbcInterbase& Orig); // not allowed
};

#endif // end of defensive include
