//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBescape
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday, September 9th, 2010 @ 05:18:20 PM
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBescape.h"
#include "DButils.h"

#include <COL/COLsink.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLostream& DBescapePostgresSql(COLostream& Stream, const COLstring& UnescapedString){
   // It is assumed that strings are not pre-escaped for SQL
   COLsink* Sink = Stream.sink();
   const char* Data = UnescapedString.c_str();
   const int SizeOfData = UnescapedString.size();

   const int BufferSize = 256; // diminishing returns if larger
   int BufferCount = 0;
   char Buffer[BufferSize];    // minimize expensive calls to Sink->write()
   const int BufferHighWater = sizeof(Buffer) - 4;

   Buffer[BufferCount++] = '\'';
   for (int i = 0; i < SizeOfData; i++){
      const char Ch = Data[i];
      switch(Ch){
      case '\'':
      case '\\':
         Buffer[BufferCount++] = Ch;
         // fall through - do not break
      default:
         Buffer[BufferCount++] = Ch;
      }

      if (BufferCount >= BufferHighWater){
         Sink->write(Buffer, BufferCount);
         BufferCount = 0;
      }
   }
   Buffer[BufferCount++] = '\'';

   if (BufferCount > 0){
      Sink->write(Buffer, BufferCount);
   }
   return Stream;
}

COLostream& DBescapeSqlWithUnicode(COLostream& Stream, const COLstring& UnescapedString){

   if(!DBstringIsAscii(UnescapedString)){
      Stream << 'N';
   }
   return DBescapeSql(Stream, UnescapedString);
}

COLostream& DBescapeSql(COLostream& Stream, const COLstring& UnescapedString){
   COLsink* Sink = Stream.sink();
   const char* Data = UnescapedString.c_str();
   const int SizeOfData = UnescapedString.size();

   const int BufferSize = 256; // diminishing returns if larger
   int BufferCount = 0;
   char Buffer[BufferSize];    // minimize expensive calls to Sink->write()
   const int BufferHighWater = sizeof(Buffer) - 4;

   Buffer[BufferCount++] = '\'';
   for (int i = 0; i < SizeOfData; i++){
      const char Ch = Data[i];
      switch(Ch){
      case '\'':
         Buffer[BufferCount++] = '\'';
         // fall through - do not break
      default:
         Buffer[BufferCount++] = Ch;
      }

      if (BufferCount >= BufferHighWater){
         Sink->write(Buffer, BufferCount);
         BufferCount = 0;
      }
   }
   Buffer[BufferCount++] = '\'';

   if (BufferCount > 0){
      Sink->write(Buffer, BufferCount);
   }
   return Stream;
}


COLostream& DBescapeMySql( COLostream& Stream, const COLstring& UnescapedString ){
   // It is assumed that strings are not pre-escaped for SQL
   COLsink* Sink = Stream.sink();
   const char* Data = UnescapedString.c_str();
   const int SizeOfData = UnescapedString.size();

   const int BufferSize = 256; // diminishing returns if larger
   int BufferCount = 0;
   char Buffer[BufferSize];    // minimize expensive calls to Sink->write()
   const int BufferHighWater = sizeof(Buffer) - 4;

   Buffer[BufferCount++] = '\'';
   for (int i = 0; i < SizeOfData; i++){
      int Ch = Data[i];
      switch(Ch){
      case '\0':
         Ch = '0';  // Escape NULs as "\0".
         /* no break */
      case '\\':
      case '\'':
         Buffer[BufferCount++] = '\\';
         // fall through - do not break
      default:
         Buffer[BufferCount++] = Ch;
      }

      if (BufferCount >= BufferHighWater){
         Sink->write(Buffer, BufferCount);
         BufferCount = 0;
      }
   }
   Buffer[BufferCount++] = '\'';

   if (BufferCount > 0){
      Sink->write(Buffer, BufferCount);
   }
   return Stream;
}

COLostream& DBescapeMariaDb( COLostream& Stream, const COLstring& UnescapedString ){
   // It is assumed that strings are not pre-escaped for SQL
   COLsink* Sink = Stream.sink();
   const char* Data = UnescapedString.c_str();
   const int SizeOfData = UnescapedString.size();

   const int BufferSize = 256; // diminishing returns if larger
   int BufferCount = 0;
   char Buffer[BufferSize];    // minimize expensive calls to Sink->write()
   const int BufferHighWater = sizeof(Buffer) - 4;

   Buffer[BufferCount++] = '\'';
   for (int i = 0; i < SizeOfData; i++){
      int Ch = Data[i];
      switch(Ch){
      case '\0':
         Ch = '0';  // Escape NULs as "\0".
         /* no break */
      case '\\':
      case '\'':
         Buffer[BufferCount++] = '\\';
         // fall through - do not break
      default:
         Buffer[BufferCount++] = Ch;
      }

      if (BufferCount >= BufferHighWater){
         Sink->write(Buffer, BufferCount);
         BufferCount = 0;
      }
   }
   Buffer[BufferCount++] = '\'';

   if (BufferCount > 0){
      Sink->write(Buffer, BufferCount);
   }
   return Stream;
}
