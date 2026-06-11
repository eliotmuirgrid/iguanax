//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIjsonEscape
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, February 9th, 2009 @ 09:49:57 AM
//
//---------------------------------------------------------------------------
#include "SFIprecomp.h"
#pragma hdrstop

#include "SFIjsonEscape.h"

#include <ctype.h>

#include <COL/COLsinkString.h>

void SFIjsonEscape(const char* Data, int SizeOfData, COLsink& Sink)
{
   //COLPRECONDITION(SizeOfData > 0);

   const int BufferSize = 256; // diminishing returns if larger
   int BufferCount = 0;
   char Buffer[BufferSize];    // minimize expensive calls to COLsink::write()
   const int BufferHighWater = sizeof(Buffer) - 7;

   for (int CharIndex = 0; CharIndex < SizeOfData; CharIndex++)
   {
      char Ch;
      switch(Ch = Data[CharIndex])
      {
      case '\\':
      case '\"':
         Buffer[BufferCount++] = '\\';
         Buffer[BufferCount++] = Ch;
         break;
      case '\f':
         Buffer[BufferCount++] = '\\';
         Buffer[BufferCount++] = 'f';
         break;
      case '\b':
         Buffer[BufferCount++] = '\\';
         Buffer[BufferCount++] = 'b';
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
      default:
         if(!iscntrl((unsigned char)Ch)) {
            Buffer[BufferCount++] = Ch;
         } else {
            snprintf(Buffer+BufferCount, BufferSize-BufferCount, "\\u%04X", (unsigned char)Ch);
            BufferCount += 6;
         }
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

void SFIjsonEscape(const char* Data, int SizeOfData, COLstring& String){
   COLsinkString Sink(String);
   SFIjsonEscape(Data, SizeOfData, Sink);
}

bool SFIisJsonSafe(const char* pSource, size_t Size, char Delimiter /* = 0 */)
{
   const char *const pEnd = pSource + Size;

   for(; pSource < pEnd; ++pSource)
   {
      switch(*pSource)
      {
      case '\'':
      case '"':
         if(!Delimiter || Delimiter == *pSource)
         {
            return false;
         }
         break;
      case '\\':
         if(++pSource == pEnd)
         {
            return false;
         }
         /* No break: we want to test the escaped character. */
      default:
         if(!isprint(*pSource))
         {
            return false;
         }
      }
   }
   return true;
}
