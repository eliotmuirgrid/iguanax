//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIhtmlEscape
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Wednesday, April 8th, 2009 @ 09:24:58 AM
//
//---------------------------------------------------------------------------
#include "SFIprecomp.h"
#pragma hdrstop

#include <COL/COLsinkString.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "SFIhtmlEscape.h"

// If changing this function, please update the SFIhtmlEscapeNoNewline below.
// The discussion on optimization of HTML escaping is in #15638
void SFIhtmlEscape(const char* Data, int SizeOfData, COLsink& Sink){
   const int BufferSize = 256; // diminishing returns if larger
   int BufferCount = 0;
   char Buffer[BufferSize];    // minimize expensive calls to COLsink::write()
   const int BufferHighWater = sizeof(Buffer) - 7; // "&quot;" is the longest

   for (int CharIndex = 0; CharIndex < SizeOfData; CharIndex++){
      char Ch;
      switch(Ch = Data[CharIndex]){
      case ';':
         Buffer[BufferCount++] = '&';
         Buffer[BufferCount++] = '#';
         Buffer[BufferCount++] = '5';
         Buffer[BufferCount++] = '9';
         Buffer[BufferCount++] = ';';
         break;
      case '&':
         Buffer[BufferCount++] = '&';
         Buffer[BufferCount++] = 'a';
         Buffer[BufferCount++] = 'm';
         Buffer[BufferCount++] = 'p';
         Buffer[BufferCount++] = ';';
         break;
      case '<':
         Buffer[BufferCount++] = '&';
         Buffer[BufferCount++] = 'l';
         Buffer[BufferCount++] = 't';
         Buffer[BufferCount++] = ';';
         break;
      case '>':
         Buffer[BufferCount++] = '&';
         Buffer[BufferCount++] = 'g';
         Buffer[BufferCount++] = 't';
         Buffer[BufferCount++] = ';';
         break;
      case '\"':
         Buffer[BufferCount++] = '&';
         Buffer[BufferCount++] = 'q';
         Buffer[BufferCount++] = 'u';
         Buffer[BufferCount++] = 'o';
         Buffer[BufferCount++] = 't';
         Buffer[BufferCount++] = ';';
         break;
      case '\n':
         Buffer[BufferCount++] = '<';
         Buffer[BufferCount++] = 'b';
         Buffer[BufferCount++] = 'r';
         Buffer[BufferCount++] = '>';
         break;
      default:
         Buffer[BufferCount++] = Ch;
      }
      if (BufferCount >= BufferHighWater){
         Sink.write(Buffer, BufferCount);
         BufferCount = 0;
      }
   }
   if (BufferCount > 0){
      Sink.write(Buffer, BufferCount);
   }
}

// The exact same copy of above except it does not do the newline escaping
void SFIhtmlEscapeNoNewline (const char* Data, int SizeOfData, COLsink& Sink)
{
   const int BufferSize = 256; // diminishing returns if larger
   int BufferCount = 0;
   char Buffer[BufferSize];    // minimize expensive calls to COLsink::write()
   const int BufferHighWater = sizeof(Buffer) - 7; // "&quot;" is the longest

   for (int CharIndex = 0; CharIndex < SizeOfData; CharIndex++)
   {
      char Ch;
      switch(Ch = Data[CharIndex])
      {
      case ';':
         Buffer[BufferCount++] = '&';
         Buffer[BufferCount++] = '#';
         Buffer[BufferCount++] = '5';
         Buffer[BufferCount++] = '9';
         Buffer[BufferCount++] = ';';
         break;
      case '&':
         Buffer[BufferCount++] = '&';
         Buffer[BufferCount++] = 'a';
         Buffer[BufferCount++] = 'm';
         Buffer[BufferCount++] = 'p';
         Buffer[BufferCount++] = ';';
         break;
      case '<':
         Buffer[BufferCount++] = '&';
         Buffer[BufferCount++] = 'l';
         Buffer[BufferCount++] = 't';
         Buffer[BufferCount++] = ';';
         break;
      case '>':
         Buffer[BufferCount++] = '&';
         Buffer[BufferCount++] = 'g';
         Buffer[BufferCount++] = 't';
         Buffer[BufferCount++] = ';';
         break;
      case '\"':
         Buffer[BufferCount++] = '&';
         Buffer[BufferCount++] = 'q';
         Buffer[BufferCount++] = 'u';
         Buffer[BufferCount++] = 'o';
         Buffer[BufferCount++] = 't';
         Buffer[BufferCount++] = ';';
         break;
      default:
         Buffer[BufferCount++] = Ch;
      }
      if (BufferCount >= BufferHighWater)
      {
         Sink.write(Buffer, BufferCount);
         BufferCount = 0;
      }
   }
   if (BufferCount > 0)
   {
      Sink.write(Buffer, BufferCount);
   }
}

void SFIhtmlEscapeWithCRAsNewline(const char* Data, int SizeOfData, COLsink& Sink){
   const int BufferSize = 256; // diminishing returns if larger
   int BufferCount = 0;
   char Buffer[BufferSize];    // minimize expensive calls to COLsink::write()
   const int BufferHighWater = sizeof(Buffer) - 7; // "&quot;" is the longest

   for (int CharIndex = 0; CharIndex < SizeOfData; CharIndex++){
      char Ch;
      switch(Ch = Data[CharIndex]){
      case 0:
         Buffer[BufferCount++] = ' ';  // Convert embedded NULL characters to spaces
         break;
      case ';':
         Buffer[BufferCount++] = '&';
         Buffer[BufferCount++] = '#';
         Buffer[BufferCount++] = '5';
         Buffer[BufferCount++] = '9';
         Buffer[BufferCount++] = ';';
         break;
      case '&':
         Buffer[BufferCount++] = '&';
         Buffer[BufferCount++] = 'a';
         Buffer[BufferCount++] = 'm';
         Buffer[BufferCount++] = 'p';
         Buffer[BufferCount++] = ';';
         break;
      case '<':
         Buffer[BufferCount++] = '&';
         Buffer[BufferCount++] = 'l';
         Buffer[BufferCount++] = 't';
         Buffer[BufferCount++] = ';';
         break;
      case '>':
         Buffer[BufferCount++] = '&';
         Buffer[BufferCount++] = 'g';
         Buffer[BufferCount++] = 't';
         Buffer[BufferCount++] = ';';
         break;
      case '\"':
         Buffer[BufferCount++] = '&';
         Buffer[BufferCount++] = 'q';
         Buffer[BufferCount++] = 'u';
         Buffer[BufferCount++] = 'o';
         Buffer[BufferCount++] = 't';
         Buffer[BufferCount++] = ';';
         break;
      case '\r':
         switch (Data[CharIndex+1]){
      case '\n':
         CharIndex++;  // we skip the \n if we have \r\n
         break;
         }
      case '\n':
         Buffer[BufferCount++] = '<';
         Buffer[BufferCount++] = 'b';
         Buffer[BufferCount++] = 'r';
         Buffer[BufferCount++] = '>';
         break;
      default:
         Buffer[BufferCount++] = Ch;
      }
      if (BufferCount >= BufferHighWater){
         Sink.write(Buffer, BufferCount);
         BufferCount = 0;
      }
   }
   if (BufferCount > 0){
      Sink.write(Buffer, BufferCount);
   }
}

void SFIhtmlEscapeNewlineOnly(const char* Data, int SizeOfData, COLsink& Sink){
   const int BufferSize = 256; // diminishing returns if larger
   int BufferCount = 0;
   char Buffer[BufferSize];    // minimize expensive calls to COLsink::write()
   const int BufferHighWater = sizeof(Buffer) - 5; // "<br>" is the longest

   for (int i = 0; i < SizeOfData; i++){
      char Ch;
      switch(Ch = Data[i]){
      case '\r':
         switch (Data[i+1]){
         case '\n':
            i++;  // we skip the \n if we have \r\n
            break;
         }
      case '\n':
         Buffer[BufferCount++] = '<';
         Buffer[BufferCount++] = 'b';
         Buffer[BufferCount++] = 'r';
         Buffer[BufferCount++] = '>';
         break;
      default:
         Buffer[BufferCount++] = Ch;
      }
      if (BufferCount >= BufferHighWater){
         Sink.write(Buffer, BufferCount);
         BufferCount = 0;
      }
   }
   if (BufferCount > 0){
      Sink.write(Buffer, BufferCount);
   }
}

void SFIhtmlEscapeNewlineOnly(const char* Data, int SizeOfData, COLstring& String){
   COLsinkString Sink(String);
   SFIhtmlEscapeNewlineOnly(Data, SizeOfData, Sink);
}


// This is not super efficient - but has a convenient calling syntax.  Use only
// for small strings.
COLstring SFIhtmlEscapeWithCRAsNewlineString(const COLstring& String){
   COL_FUNCTION(SFIhtmlEscapeWithCRAsNewlineString);
   COLstring Result;
   COLsinkString Sink(Result);
   SFIhtmlEscapeWithCRAsNewline(String.c_str(), String.size(), Sink);
   return Result;
}

void SFIhtmlEscapeNoNewline(const char* Data, int SizeOfData, COLstring& String){
   // overloaded to make taking COLsink out of COLstring easier.
   COLsinkString Sink(String);
   SFIhtmlEscapeNoNewline(Data, SizeOfData, Sink);
}

void SFIhtmlEscape(const char* Data, int SizeOfData, COLstring& String){
   // overloaded to make taking COLsink out of COLstring easier.
   COLsinkString Sink(String);
   SFIhtmlEscape(Data, SizeOfData, Sink);
}
