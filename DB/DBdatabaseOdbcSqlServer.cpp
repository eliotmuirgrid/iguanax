//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcSqlServer
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

#include "DBdatabaseOdbcSqlServer.h"
#include "DBescape.h"
#include "DButils.h"

static const COLstring DBdoubleType("FLOAT");
const COLstring& DBdatabaseOdbcSqlServer::dbDoubleType() const
{ 
   return DBdoubleType;
}

// TODO - be nice to use a function pointer...
COLostream& DBdatabaseOdbcSqlServer::addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const{
   if (useUnicode()){
      return DBescapeSqlWithUnicode(Stream, UnescapedString);
   } else{
      return DBescapeSql(Stream, UnescapedString);
   }
}

void DBdatabaseOdbcSqlServer::
streamBinary(COLostream& Stream, const COLstring& Buf) const
{
   if(Buf.size() == 0) {
      streamNullValue(Stream);
   } else {
      Stream << "0x";
      DBbinaryToHex(*Stream.sink(), Buf.c_str(), Buf.size());
   }
}
