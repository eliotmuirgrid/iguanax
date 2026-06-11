//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcPostgreSQL
//
// Description:
//
// Implementation.
//
// Author:   Henry Tran
// Date:     November 18, 2004 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBresultSetPtr.h"
#include "DBdatabaseOdbcPostgreSql.h"
#include "DBescape.h"
#include "DBvariant.h"
#include "DBresultSetRow.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

static const COLstring DBintegerType("INT4");
const COLstring& DBdatabaseOdbcPostgreSql::dbIntegerType() const
{
   COL_METHOD(DBdatabaseOdbcPostgreSql::dbIntegerType);
   return DBintegerType;
}

static const COLstring DBdoubleType("FLOAT8");
const COLstring& DBdatabaseOdbcPostgreSql::dbDoubleType() const
{ 
   COL_METHOD(DBdatabaseOdbcPostgreSql::dbDoubleType);
   return DBdoubleType;
}
  
static const COLstring DBdateTimeType("TIMESTAMP");
const COLstring& DBdatabaseOdbcPostgreSql::dbDateTimeType() const
{
   COL_METHOD(DBdatabaseOdbcPostgreSql::dbDateTimeType);
   return DBdateTimeType;
}

bool DBdatabaseOdbcPostgreSql::supportsDescribeParam() const
{
   COL_METHOD(DBdatabaseOdbcPostgreSql::supportsDescribeParam);
   return false;
}

DBresultSetPtr DBdatabaseOdbcPostgreSql::fetchNonSystemDatabaseTables() const
{
   COL_METHOD(DBdatabaseOdbcPostgreSql::fetchNonSystemDatabaseTables);
   COLstring Query = "SELECT table_name FROM information_schema.tables WHERE table_schema = 'public'";

   DBresultSetPtr QueryResultSet = executeSqlString(Query);

   DBresultSetPtr pResultSet = new DBresultSet();  

   pResultSet->addColumn( "TABLE_NAME", DB_STRING );

   COLuint32 RowIndex = 0;

   // Build a vector of rows, where each row contains a vector of column values.
   for(; RowIndex < QueryResultSet->countOfRow(); RowIndex++)
   {
      COLstring TableName = QueryResultSet->value(RowIndex, 0).string();
      pResultSet->addRow();
      pResultSet->setValue(RowIndex, 0, TableName);
   }

   pResultSet->setCountOfDatabaseRow( RowIndex );
   return pResultSet;
}

bool DBdatabaseOdbcPostgreSql::isSystemTable( const char* TableName ) const
{
   COL_METHOD(DBdatabaseOdbcPostgreSql::isSystemTable);
   COLstring Query = "SELECT table_schema from information_schema.tables WHERE table_name = '";
   Query += TableName;
   Query += "'";

   DBresultSetPtr QueryResultSet = executeSqlString(Query);

   if(!QueryResultSet->countOfRow())
   {
      // no such table at all - not a system table
      return false;
   }

   COLPRECONDITION( QueryResultSet->countOfRow() == 1 ); // sanity

   COLstring TableSchema = QueryResultSet->value(0, 0).string();
   return !!(TableSchema != "public");
}

COLostream& DBdatabaseOdbcPostgreSql::addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const{
   COL_METHOD(DBdatabaseOdbcPostgreSql::addEscapedStringToStream);
   return DBescapePostgresSql(Stream, UnescapedString);
}

bool DBdatabaseOdbcPostgreSql::useWideChar() const
{
   COL_METHOD(DBdatabaseOdbcPostgreSql::useWideChar);
   // ODBC - PostgreSQL does need WideChar API -
   // the data source must be configured with a client_encoding to match the server,
   // and then we do "set names utf8", so we can work with UTF8 directly
   // WideChat (UTF16) is still needed -
   // apparently something still thinks we're working in that client-server LATIN1 charset,
   // and UTF8-as-LATIN1 corruption occurs in single-byte
#ifdef _WIN32
   return useUnicode();
#else
   return false;
#endif
};
