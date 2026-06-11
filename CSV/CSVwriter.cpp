//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CSVwriter
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, September 1st, 2009 @ 04:00:20 PM
//
//---------------------------------------------------------------------------
#include "CSVprecomp.h"
#pragma hdrstop

#include "CSVwriter.h"

#include <COL/COLsink.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;
#include <COL/COLdateTime.h>


static void CSVquote(COLsink& Target, const char* pSource, size_t SourceSize)
{
   COLPRECONDITION( sizeof(char) == 1 );
   const char* Replacements[256] = {0};

   Replacements['\0'] = " ";
   Replacements['"']  = "\"\"";
   Replacements['\r'] = "\\r";
   Replacements['\n'] = "\\n";

   const char* pSourceEnd = pSource + SourceSize;

   Target.write("\"", 1);

   // This will also find embedded NUL characters.
   //
   for(const char* pCurrent; (pCurrent = pSource + strcspn(pSource, "\"\r\n")) != pSourceEnd; pSource = pCurrent + 1)
   {
      const char* pReplacementText = Replacements[*(unsigned char*)pCurrent];

      Target.write(pSource, pCurrent - pSource);
      Target.write(pReplacementText, strlen(pReplacementText));
   }

   Target.write(pSource, pSourceEnd - pSource);
   Target.write("\"", 1);
}

void CSVwriter::writeUnquotedString(const COLstring& Value)
{
   if (RowStarted)
   {
      Sink.write(",", 1);
   }

   Sink.write(Value.c_str(), Value.size());
   RowStarted = true;
}

void CSVwriter::writeQuotedString(const COLstring& Value)
{
   writeQuotedString(Value.c_str(), Value.size());
}

void CSVwriter::writeQuotedString(const char* pValue, size_t Size)
{
   if (RowStarted)
   {
      Sink.write(",", 1);
   }

   CSVquote(Sink, pValue, Size);
   RowStarted = true;
}

void CSVwriter::writeTimestamp(const COLdateTime& Timestamp)
{
   writeUnquotedString(Timestamp.format("%Y/%m/%d %H:%M:%S"));
}

void CSVwriter::writeDateOnly(const COLdateTime& Date)
{
   writeUnquotedString(Date.format("%Y/%m/%d"));
}

void CSVwriter::writeUint32(COLuint32 Value)
{
   if (RowStarted)
   {
      Sink.write(",", 1);
   }

   COLostream(Sink) << Value;
   RowStarted = true;
}

void CSVwriter::endRow()
{
   Sink.write("\r\n", 2);
   RowStarted = false;
}
