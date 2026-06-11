//---------------------------------------------------------------------------
// Copyright (C) 1997-2016 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcSQL
//
// Description:
//
// Implementation.
//
// Author:   Meeral Qureshi
// Date:     July 11, 2016 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBresultSetPtr.h"
#include "DBdatabaseOdbcMySql.h"
#include "DBdatabaseMySql.h"
#include "DBescape.h"
#include "DBvariant.h"
#include "DBresultSetRow.h"

static const COLstring DBintegerType("INT4");
const COLstring& DBdatabaseOdbcMySql::dbIntegerType() const
{
   return DBintegerType;
}

static const COLstring DBdoubleType("FLOAT8");
const COLstring& DBdatabaseOdbcMySql::dbDoubleType() const
{ 
   return DBdoubleType;
}

bool DBdatabaseOdbcMySql::supportsDescribeParam() const
{
   return false;
}

// DBresultSetPtr DBdatabaseOdbcMySql::fetchNonSystemDatabaseTables() const
// {
//    return DBdatabaseMySql::fetchNonSystemDatabaseTables();
// }

bool DBdatabaseOdbcMySql::isSystemTable( const char* TableName ) const
{
   // As in MySql  -> My Sql isn't a system table and doesn't have system tables
   return false;
}

COLostream& DBdatabaseOdbcMySql::addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const{
   if (useUnicode()){
      return DBescapeSqlWithUnicode(Stream, UnescapedString);
   } else{
      return DBescapeSql(Stream, UnescapedString);
   }
}

// NOTE : See header for information on this commented-out section:

// bool DBdatabaseOdbcMySql::useWideChar() const
// {
//    // ODBC - MySql may need WideChar API -
//    // the data source must be configured with a client_encoding to match the server,
//    // and then we do "set names utf8", so we can work with UTF8 directly
//    // WideChat (UTF16) is still needed -
//    // apparently something still thinks we're working in that client-server LATIN1 charset,
//    // and UTF8-as-LATIN1 corruption occurs in single-byte
// #ifdef _WIN32
//    executeSqlString("set names 'utf8'");
//    return useUnicode();
// #else
//    return false;
// #endif
// };
