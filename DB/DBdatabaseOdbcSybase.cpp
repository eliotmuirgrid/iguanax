//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbcSybase
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

#include "DBdatabaseOdbcSybase.h"
#include "DBescape.h"

bool DBdatabaseOdbcSybase::selectUnionSeparateOrderBySupported() const
{
   return false;
}

COLostream& DBdatabaseOdbcSybase::streamName( COLostream& Stream, const COLstring& Name, bool QuoteName ) const
{
   COLstring LowerCaseName = Name;
   LowerCaseName.toLowerCase();
   if ( "message" == LowerCaseName  )
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

bool DBdatabaseOdbcSybase::useWideChar() const
{
   return false; // Sybase ASA causes a segfault when using WideChar mode - FIXME debug it if we care
   // it's not required for Unicode because ODBC data source has a Character Set option, which can be set to utf8,
}

COLostream& DBdatabaseOdbcSybase::addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const{
   return DBescapeSql(Stream, UnescapedString);
}
