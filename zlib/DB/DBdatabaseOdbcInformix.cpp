//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcInformix
//
// Description:
//
// Implementation
//
// Author: Pat Wisking
// Date:   Monday, July 27th, 2009 @ 12:00:03 PM
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBdatabaseOdbcInformix.h"
#include "DBescape.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

static const DBodbcErrorList AcceptableErrorStrings[] =
{
   {"00000"},
   {"42000"}
};

COLstring DBdatabaseOdbcInformix::acceptableErrorString(COLuint32 ErrorIndex) const
{
   return AcceptableErrorStrings[ErrorIndex].Code;
}

COLuint32 DBdatabaseOdbcInformix::acceptableErrorStringSize() const
{
   return sizeof(AcceptableErrorStrings) / sizeof(AcceptableErrorStrings[0]);
}

const COLstring DB_DATATYPE_INFORMIX_FLOAT = "FLOAT";

const COLstring& DBdatabaseOdbcInformix::dbDoubleType() const
{
   return DB_DATATYPE_INFORMIX_FLOAT;
};

const COLstring DB_DATATYPE_INFORMIX_DATETIME = "DATETIME YEAR TO SECOND";

const COLstring& DBdatabaseOdbcInformix::dbDateTimeType() const
{
   return DB_DATATYPE_INFORMIX_DATETIME;
};


COLostream& DBdatabaseOdbcInformix::addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const{
   return DBescapeSql(Stream, UnescapedString);
}
