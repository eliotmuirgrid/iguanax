#ifndef __DB_DATABASE_ODBC_ORACLE_H__
#define __DB_DATABASE_ODBC_ORACLE_H__
//---------------------------------------------------------------------------
//
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
// Date:   April 15, 2004 
//
//
//---------------------------------------------------------------------------

#include "DBdatabaseOdbc.h"

class DBdatabaseOdbcOracle :  public DBdatabaseOdbc
{
public:
   DBdatabaseOdbcOracle();
   virtual ~DBdatabaseOdbcOracle();
   
   virtual DBdatabaseType databaseType() const { return DB_ODBC_ORACLE; }
 
   virtual const COLstring& dbDoubleType() const;
   virtual const COLstring& dbDateTimeType() const;
   virtual const COLstring& dbTextType() const;

   // Override because ODBC Oracle requires table names to be in upper case.
   virtual bool isSystemTable( const char* TableName ) const;
   virtual DBresultSetPtr fetchDatabaseTableColumns( const char* TableName ) const;

   DBresultSetPtr fetchDatabaseTables() const;
   DBresultSetPtr fetchNonSystemDatabaseTables() const;

   // Override because ODBC Oracle uses a different string to ping the DB
   virtual const COLstring& sqlPingCommand() const;

   virtual COLostream& addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const;

   // Override from DBdatabase
   void streamBinary(COLostream&, const COLstring&) const;

private:
   virtual void streamCreateTableForeignKeySuffix( COLostream& Stream, 
                                                   const DBsqlCreateTable& SqlCreateTableCommand,
                                                   COLuint32 ColumnIndex ) const;
  
   virtual void streamDateTime( COLostream& Stream, const COLdateTime& DateTime ) const;
   virtual void streamNullValue( COLostream& Stream ) const;

   DBdatabaseOdbcOracle( DBdatabaseOdbcOracle& Orig ); // not allowed
   DBdatabaseOdbcOracle& operator=( DBdatabaseOdbcOracle& Orig ); // not allowed
};

#endif // end of defensive include
