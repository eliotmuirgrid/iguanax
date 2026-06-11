//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAescape
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Monday, April 18th, 2011 @ 03:29:20 PM
//
//---------------------------------------------------------------------------
#include "LUAprecomp.h"
#pragma hdrstop

#include "LUAescape.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <ctype.h> // for iscntrl

// Lua escape sequences as found on:
//     http://www.lua.org/pil/2.4.html
// Function just copied from CSjsEscape.
//
// \a  Bell
// \b  Backspace.
// \f  Form feed.
// \n  Newline.
// \0  Nul character.
// \r  Carriage return.
// \t  Horizontal tab.
// \v  Vertical tab.
// \'  Single quote or apostrophe.
// \"  Double quote.
// \\  Backslash.
COLstring LUAstringEscape(const char* UnescapedString, int Size){
   COLstring EscapedString;
   const int BufferSize = 256; // diminishing returns if larger
   int BufferCount = 0;
   char Buffer[BufferSize];    // minimize expensive calls to COLsink::write()
   const int BufferHighWater = sizeof(Buffer) - 5;
   char Ch;
   for (int CharIndex = 0; CharIndex < Size; CharIndex++){
      Ch = UnescapedString[CharIndex];
      switch(Ch)
      {
      case '\\':
      case '\'':
      case '\"':
         Buffer[BufferCount++] = '\\';
         Buffer[BufferCount++] = Ch;
         break;
      case '\a':
         Buffer[BufferCount++] = '\\';
         Buffer[BufferCount++] = 'a';
      case '\b':
         Buffer[BufferCount++] = '\\';
         Buffer[BufferCount++] = 'b';
         break;
      case '\f':
         Buffer[BufferCount++] = '\\';
         Buffer[BufferCount++] = 'f';
         break;
      case '\n':
         Buffer[BufferCount++] = '\\';
         Buffer[BufferCount++] = 'n';
         break;
      case '\r':
         Buffer[BufferCount++] = '\\';
         Buffer[BufferCount++] = 'r';
         break;
      case '\t':
         Buffer[BufferCount++] = '\\';
         Buffer[BufferCount++] = 't';
         break;
      case '\v':
         Buffer[BufferCount++] = '\\';
         Buffer[BufferCount++] = 'v';
         break;
      default:
         if (iscntrl((unsigned char)Ch)){
            BufferCount += snprintf(Buffer + BufferCount, BufferSize - BufferCount, "\\%03d", Ch);
         } else {
            Buffer[BufferCount++] = Ch;
         }
      }
      if (BufferCount >= BufferHighWater)
      {
         EscapedString.write(Buffer, BufferCount);
         BufferCount = 0;
      }
   }
   if (BufferCount > 0){
      EscapedString.write(Buffer, BufferCount);
   }
   return EscapedString;
}
