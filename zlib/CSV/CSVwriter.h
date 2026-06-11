#ifndef __CSV_WRITER_H__
#define __CSV_WRITER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CSVwriter
//
// Description:
//
// CSVwriter class.  Writes CSV content to a COLsink.
//
// Author: Kevin Senn
// Date:   Tuesday, September 1st, 2009 @ 04:00:20 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class COLdateTime;

class CSVwriter 
{
public:
   CSVwriter(COLsink& iSink)
    : Sink(iSink),
      RowStarted(false)
   {
   }

   // This is non virtual because we are assuming no classes will inherit from this class
   ~CSVwriter()
   {
   }

   void writeUnquotedString(const COLstring& Value);
   
   // Wraps the string in double-quotes, and escapes null characters, double-quotes, \r, and \n.
   void writeQuotedString(const COLstring& Value);
   void writeQuotedString(const char* pValue, size_t Size);

   // Writes a timestamp in the format YYYY/MM/DD HH:MM:SS
   void writeTimestamp(const COLdateTime& Timestamp);

   // Writes a timestamp in the format YYYY/MM/DD
   void writeDateOnly(const COLdateTime& Date);

   void writeUint32(COLuint32 Value);

   void endRow();

private:
   COLsink&   Sink;
   bool RowStarted;
};

#endif // end of defensive include
