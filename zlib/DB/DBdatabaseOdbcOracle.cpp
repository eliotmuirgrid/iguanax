//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcOracle
//
// Description:
//
// This class handles calls to Oracle through the ODBC C API. This class 
// links the API with the common interface provided by the DBdatabase parent 
// class.
//
// Author: Henry Tran
// Date:   February 23, 2004 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBdatabaseOdbcOracle.h"
#include "DBdatabaseOracle.h"
#include "DBresultSetPtr.h"
#include "DBsqlSelect.h"
#include "DBvariant.h"
#include "DBescape.h"
#include "DButils.h"

#include <COL/COLdateTime.h>

DBdatabaseOdbcOracle::DBdatabaseOdbcOracle() {}

DBdatabaseOdbcOracle::~DBdatabaseOdbcOracle(){}

const COLstring& DBdatabaseOdbcOracle::dbDoubleType() const
{ 
   return DBdatabaseOracle::dbDoubleType();
}

const COLstring& DBdatabaseOdbcOracle::dbDateTimeType() const
{
   return DBdatabaseOracle::dbDateTimeType();
}

const COLstring& DBdatabaseOdbcOracle::dbTextType() const
{
   return DBdatabaseOracle::dbTextType();
}

void DBdatabaseOdbcOracle::streamCreateTableForeignKeySuffix
( 
   COLostream& Stream, 
   const DBsqlCreateTable& SqlCreateTableCommand,
   COLuint32 ColumnIndex 
) const
{
   DBdatabaseOracle::streamCreateTableForeignKeySuffix( Stream, SqlCreateTableCommand, ColumnIndex );
}

void DBdatabaseOdbcOracle::streamDateTime( COLostream& Stream, const COLdateTime& DateTime ) const
{
   DBdatabaseOracle::streamDateTime( Stream, DateTime );
}

void DBdatabaseOdbcOracle::streamNullValue( COLostream& Stream ) const
{
   DBdatabaseOracle::streamNullValue( Stream );
}

bool DBdatabaseOdbcOracle::isSystemTable( const char* TableName ) const
{
   COLstring TableNameUpperCase(TableName);
   TableNameUpperCase.toUpperCase();
   return DBdatabaseOdbc::isSystemTable(TableNameUpperCase.c_str());
}

DBresultSetPtr DBdatabaseOdbcOracle::fetchDatabaseTableColumns( const char* TableName ) const
{
   COLstring TableNameUpperCase(TableName);
   TableNameUpperCase.toUpperCase();
   return DBdatabaseOdbc::fetchDatabaseTableColumns(TableNameUpperCase.c_str());
}


DBresultSetPtr DBdatabaseOdbcOracle::fetchDatabaseTables() const  // From OCI Oracle
{
   DBsqlSelect SelectTableNameStatement;
   SelectTableNameStatement.addTable("USER_TABLES");
   SelectTableNameStatement.addColumn("TABLE_NAME");
   return executeSqlCommand(SelectTableNameStatement);
}


DBresultSetPtr DBdatabaseOdbcOracle::fetchNonSystemDatabaseTables() const  // From OCI Oracle
{
   // fetchDatabaseTables already returns only user tables
   return fetchDatabaseTables();
}

static const COLstring OraclePingCommand("SELECT 0 FROM DUAL;");
const COLstring& DBdatabaseOdbcOracle::sqlPingCommand() const
{
   return OraclePingCommand;
}

COLostream& DBdatabaseOdbcOracle::addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const{
   if (useUnicode()){
      return DBescapeSqlWithUnicode(Stream, UnescapedString);
   } else {
      return DBescapeSql(Stream, UnescapedString);
   }
}

void DBdatabaseOdbcOracle::streamBinary(COLostream& Stream, const COLstring& Buf) const
{
   if(Buf.size() == 0) {
      streamNullValue(Stream);
   } else {
      Stream << "HEXTORAW('";
      DBbinaryToHex(*Stream.sink(), Buf.c_str(), Buf.size());
      Stream << "')";
   }
}
