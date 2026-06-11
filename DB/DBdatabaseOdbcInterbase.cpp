//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcInterbase
//
// Description:
//
// Implementation
//
// Author: Pat Wisking
// Date:   Tuesday, July 21st, 2009 @ 12:11:56 PM
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBdatabaseOdbcInterbase.h"
#include "DBresultSetPtr.h"
#include "DBvariant.h"
#include "DBresultSetRow.h"
#include "DBescape.h"

#include <COL/COLlog.h>

COL_LOG_MODULE;

static const DBodbcErrorList AcceptableErrorStrings[] =
{
   {"00000"},
   {"42000"},
   {"HY000"}
};

static const COLstring DBdoubleType("DOUBLE PRECISION");
const COLstring& DBdatabaseOdbcInterbase::dbDoubleType() const
{
   return DBdoubleType;
}

static const COLstring DBdateTimeType("TIMESTAMP");
const COLstring& DBdatabaseOdbcInterbase::dbDateTimeType() const
{
   return DBdateTimeType;
}

static const COLstring DBtextType("BLOB");
const COLstring& DBdatabaseOdbcInterbase::dbTextType() const
{
   return DBtextType;
}

static const COLstring DBstringMaxKeyLength("252");
const COLstring& DBdatabaseOdbcInterbase::dbStringMaxKeyLength() const
{
   return DBstringMaxKeyLength;
}

static const COLstring DBkeyModifiers(" not null ");
const COLstring& DBdatabaseOdbcInterbase::dbKeyModifiers() const
{
   return DBkeyModifiers;
}

bool DBdatabaseOdbcInterbase::selectCastSupported() const
{
   return false;
}

COLstring DBdatabaseOdbcInterbase::acceptableErrorString(COLuint32 ErrorIndex) const
{
   return AcceptableErrorStrings[ErrorIndex].Code;
}

COLuint32 DBdatabaseOdbcInterbase::acceptableErrorStringSize() const
{
   return sizeof(AcceptableErrorStrings) / sizeof(AcceptableErrorStrings[0]);
}

bool DBdatabaseOdbcInterbase::supportsDefault() const
{
   return false;
}

bool DBdatabaseOdbcInterbase::useWideChar() const
{
   return false;
};


DBresultSetPtr DBdatabaseOdbcInterbase::fetchNonSystemDatabaseTables() const
{
   COLstring Query = "SELECT DISTINCT RDB$RELATION_NAME AS TABLE_NAME"
      " FROM RDB$RELATIONS"
      " WHERE RDB$SYSTEM_FLAG=0";

   DBresultSetPtr QueryResultSet = executeSqlString(Query);

   DBresultSetPtr pResultSet = new DBresultSet();  

   pResultSet->addColumn( "TABLE_NAME", DB_STRING );

   COLuint32 RowIndex = 0;

   // Build a vector of rows, where each row contains a vector of column values.
   for(; RowIndex < QueryResultSet->countOfRow(); RowIndex++)
   {
      COLstring TableName = COLstripWhiteSpace(QueryResultSet->value(RowIndex, 0).string(), true, true);
      // Interbase apparently returns them padded with spaces
      pResultSet->addRow();
      pResultSet->setValue(RowIndex, 0, TableName);
   }

   pResultSet->setCountOfDatabaseRow( RowIndex );
   return pResultSet;
}

DBresultSetPtr DBdatabaseOdbcInterbase::fetchDatabaseTables() const
{
   COLstring Query = "SELECT DISTINCT RDB$RELATION_NAME AS TABLE_NAME"
      " FROM RDB$RELATIONS";      

   DBresultSetPtr QueryResultSet = executeSqlString(Query);

   DBresultSetPtr pResultSet = new DBresultSet();  

   pResultSet->addColumn( "TABLE_NAME", DB_STRING );

   COLuint32 RowIndex = 0;

   // Build a vector of rows, where each row contains a vector of column values.
   for(; RowIndex < QueryResultSet->countOfRow(); RowIndex++)
   {
      COLstring TableName = COLstripWhiteSpace(QueryResultSet->value(RowIndex, 0).string(), true, true);
      // Interbase apparently returns them padded with spaces
      pResultSet->addRow();
      pResultSet->setValue(RowIndex, 0, TableName);
   }

   pResultSet->setCountOfDatabaseRow( RowIndex );
   return pResultSet;
}

bool DBdatabaseOdbcInterbase::isSystemTable(const char* TableName) const
{
   COLstring Query = "SELECT DISTINCT RDB$RELATION_NAME AS TABLE_NAME"
      " FROM RDB$RELATIONS"
      " WHERE RDB$SYSTEM_FLAG <> 0";

    DBresultSetPtr QueryResultSet = executeSqlString(Query);
    return QueryResultSet->countOfRow()?true:false;
};

COLostream& DBdatabaseOdbcInterbase::addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const{
   return DBescapeSql(Stream, UnescapedString);
}
