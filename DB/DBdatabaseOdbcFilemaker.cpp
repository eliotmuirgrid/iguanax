//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcFilemaker
//
// Description:
//
// Implementation
//
// Author: Patrick Wisking
// Date:   Tuesday, July 28th, 2009 @ 04:33:15 PM
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBdatabaseOdbcFilemaker.h"
#include "DBescape.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool DBdatabaseOdbcFilemaker::supportsDefinePrimaryKey() const{ return false;}

bool DBdatabaseOdbcFilemaker::supportsDescribeParam() const
{
   return false;
}

static const DBodbcErrorList AcceptableErrorStrings[] =
{
   {"00000"},
   {"42000"},
   {"HY000"}
};

COLstring DBdatabaseOdbcFilemaker::acceptableErrorString(COLuint32 ErrorIndex) const
{
   return AcceptableErrorStrings[ErrorIndex].Code;
}

COLuint32 DBdatabaseOdbcFilemaker::acceptableErrorStringSize() const
{
   return sizeof(AcceptableErrorStrings) / sizeof(AcceptableErrorStrings[0]);
}

const COLstring DB_TYPE_FILEMAKER_DECIMAL_INT = "DECIMAL(32,0)";

const COLstring& DBdatabaseOdbcFilemaker::dbIntegerType() const
{
   return DB_TYPE_FILEMAKER_DECIMAL_INT;
};

const COLstring DB_TYPE_FILEMAKER_DECIMAL = "DECIMAL";

const COLstring& DBdatabaseOdbcFilemaker::dbDoubleType() const
{
   return DB_TYPE_FILEMAKER_DECIMAL;
};

const COLstring DB_TYPE_FILEMAKER_VARCHAR = "VARCHAR";

const COLstring& DBdatabaseOdbcFilemaker::dbStringType() const
{
   return DB_TYPE_FILEMAKER_VARCHAR;
};

const COLstring DB_TYPE_FILEMAKER_TIMESTAMP = "TIMESTAMP";

const COLstring& DBdatabaseOdbcFilemaker::dbDateTimeType() const
{
   return DB_TYPE_FILEMAKER_TIMESTAMP;
};

COLostream& DBdatabaseOdbcFilemaker::addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const{
   return DBescapeSql(Stream, UnescapedString);
}
