//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcDb2
//
// Description:
//
// Implementation
//
// Author: Patrick Wisking
// Date:   Monday, July 27th, 2009 @ 04:54:35 PM
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBdatabaseOdbcDb2.h"
#include "DBescape.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

static const COLstring DBkeyModifiers(" not null ");
const COLstring& DBdatabaseOdbcDb2::dbKeyModifiers() const
{
   return DBkeyModifiers;
}

static const DBodbcErrorList AcceptableErrorStrings[] =
{
   {"00000"},
   {"42601"}
};

COLstring DBdatabaseOdbcDb2::acceptableErrorString(COLuint32 ErrorIndex) const
{
   return AcceptableErrorStrings[ErrorIndex].Code;
}

COLuint32 DBdatabaseOdbcDb2::acceptableErrorStringSize() const
{
   return sizeof(AcceptableErrorStrings) / sizeof(AcceptableErrorStrings[0]);
}

static const COLstring DB_DATATYPE_DB2_TIMESTAMP = "TIMESTAMP";

const COLstring& DBdatabaseOdbcDb2::dbDateTimeType() const
{
   return DB_DATATYPE_DB2_TIMESTAMP;
};

COLostream& DBdatabaseOdbcDb2::addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const{
   return DBescapeSql(Stream, UnescapedString);
}
