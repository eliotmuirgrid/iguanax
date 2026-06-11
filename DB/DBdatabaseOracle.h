#ifndef __DB_DATABASE_ORACLE_H__
#define __DB_DATABASE_ORACLE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOracle
//
// Description:
//
// Encapsulates functions that are common to both Oracle for OCI and ODBC.
// This class is not meant to be instantiated in any way.
//
// Author:   Henry Tran
// Date:     March 2005 
//
//---------------------------------------------------------------------------

class DBsqlCreateTable;
class COLdateTime;

class DBdatabaseOracle
{
public:
   ~DBdatabaseOracle() {}
   
   static const COLstring& dbDoubleType();
   static const COLstring& dbDateTimeType();
   static const COLstring& dbTextType();

   static void streamCreateTableForeignKeySuffix( COLostream& Stream, 
                                                   const DBsqlCreateTable& SqlCreateTableCommand,
                                                   COLuint32 ColumnIndex );
  
   static void streamDateTime( COLostream& Stream, const COLdateTime& DateTime );
   static void streamNullValue( COLostream& Stream );

private:
   DBdatabaseOracle() {}

   DBdatabaseOracle( DBdatabaseOracle& Orig ); // not allowed
   DBdatabaseOracle& operator=( DBdatabaseOracle& Orig ); // not allowed
};

#endif // end of defensive include
