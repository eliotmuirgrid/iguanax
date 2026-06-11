//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcAccess
//
// Description:
//
// This class handles calls to SQL Server through the ODBC C API. This class 
// links the API with the common interface provided by the DBdatabase parent 
// class.
//
// Author: Henry Tran
// Date:   February 23, 2004 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBdatabaseOdbcAccess.h"
#include "DBescape.h"
#include "DBresultSetPtr.h"

#include <COL/COLdateTime.h>

static const COLstring DBintegerType("LONG");
const COLstring& DBdatabaseOdbcAccess::dbIntegerType() const
{
   return DBintegerType;
}

static const COLstring DBdoubleType("FLOAT");
const COLstring& DBdatabaseOdbcAccess::dbDoubleType() const
{ 
   return DBdoubleType;
}

static const COLstring DBtextType("MEMO");
const COLstring& DBdatabaseOdbcAccess::dbTextType() const
{
   return DBtextType;
}

bool DBdatabaseOdbcAccess::selectCastSupported() const
{
   return false;
}

bool DBdatabaseOdbcAccess::createTableDefaultConstraintSupported() const
{
   return false;
}

bool DBdatabaseOdbcAccess::supportsDescribeParam() const
{
   return false;
}

void DBdatabaseOdbcAccess::streamDateTime( COLostream& Stream, const COLdateTime& DateTime ) const
{
   // main difference from the base class implementation is '#' instead '"'
   if ( !DateTime.isNull() )
   {
      // Most SQL datetime values may be specified as strings in the format:
      // 'YYYY-MM-DD HH:MM:SS'
      // It is not necessary to specify two digits for date part values 
      // less that are less than 10 because of the date part delimiters.
      Stream << '#' << DateTime.year() << '-'
             << DateTime.month() << '-'
             << DateTime.day() << ' '
             << DateTime.hour() << ':'
             << DateTime.minute() << ':'
             << DateTime.second() << '#';
   }
   else
   {
      streamNullValue( Stream );
   }
}

void DBdatabaseOdbcAccess::buildPrimaryKeyList( COLarray<COLstring>& PrimaryKeyList, const char* TableName ) const
{
   PrimaryKeyList.clear();
}

COLostream& DBdatabaseOdbcAccess::addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const{
   return DBescapeSql(Stream, UnescapedString);
}
