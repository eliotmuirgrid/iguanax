//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcSybaseASE
//
// Description:
//
// This class handles calls to Sybase ASA through the ODBC C API. This class 
// links the API with the common interface provided by the DBdatabase parent 
// class.
//
// Author: Henry Tran, Nima Soltani
// Date:   March 18, 2005
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBdatabaseOdbcSybaseASE.h"
#include "DBsqlCreateTable.h"
#include "DBsqlCreateTableColumn.h"
#include "DBescape.h"

bool DBdatabaseOdbcSybaseASE::selectUnionSeparateOrderBySupported() const
{
   return true;
}

COLostream& DBdatabaseOdbcSybaseASE::streamName( COLostream& Stream, const COLstring& Name, bool QuoteName ) const
{
   COLstring LowerCaseName = Name;
   LowerCaseName.toLowerCase();
   if ( LowerCaseName == "message" || LowerCaseName == "count" )
   {
      QuoteName = true;
   }

   if ( QuoteName == true )
   {
      // This function used to place double quotes around the name but that requires that 
      // the option, "QUOTED_IDENTIFIER", be turned on. For most databases this is on by 
      // default, but there was a customer, Global Innova/Baxter (Spain), where this option
      // was mysteriously being turned off. Instead of messing around with database options, 
      // we'll just use the square brackets approach instead.
      Stream << '[' << Name << ']';
   }
   else
   {
      Stream << Name;
   }
   return Stream;
}

void DBdatabaseOdbcSybaseASE::streamCreateTableNotNull
( 
   COLostream& Stream, 
   const DBsqlCreateTable& SqlCreateTableCommand, 
   COLuint32 ColumnIndex 
) const
{
}

void DBdatabaseOdbcSybaseASE::streamCreateTableNotNull2
( 
   COLostream& Stream, 
   const DBsqlCreateTable& SqlCreateTableCommand,
   COLuint32 ColumnIndex
) const
{
   if ( SqlCreateTableCommand.column( ColumnIndex ).requiredFlag() == true
        || SqlCreateTableCommand.column( ColumnIndex ).primaryKeyFlag())
   {
      Stream << COL_T(" NOT NULL ");
   }
   else
   {
      // 
      // must be explicit, else fields may be created with not null as default.
      //
      Stream << COL_T(" NULL ");
   }
}

static const COLstring DBdoubleType("DOUBLE PRECISION");
const COLstring& DBdatabaseOdbcSybaseASE::dbDoubleType() const
{ 
   return DBdoubleType;
}

void DBdatabaseOdbcSybaseASE::adjustEmptyString(COLstring& ColumnData) const
{
   if (ColumnData == " ")
   {
      ColumnData = "";
   }
}

bool DBdatabaseOdbcSybaseASE::supportsDescribeParam() const
{
   return false;
}

COLostream& DBdatabaseOdbcSybaseASE::addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const{
   return DBescapeSql(Stream, UnescapedString);
}
