//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: SFIuriUnescapeFilter
//
// Description:
// 
// Implementation
//
// Author: Nasron Cheong
// Date:   Nov 2005
//
//---------------------------------------------------------------------------
#include "SFIprecomp.h"
#pragma hdrstop

#include "SFIuriUnescapeFilter.h"

#include <ctype.h>
#include <stdlib.h>

#include <COL/COLsinkString.h>

#undef ERROR
#define ERROR(X) \
   do{ COL_ERROR_STREAM_PLAIN(X, COLerror::PreCondition); }while(0)

#define SFI_URI_BUFFER_SIZE 1024

SFIuriUnescapeFilter::SFIuriUnescapeFilter(COLsink* ipNext) 
   : COLfilter(ipNext), State(URI_NORM)
{
   SymbolBuffer[2] = '\0';
}

SFIuriUnescapeFilter::~SFIuriUnescapeFilter()
{
}

COLuint32 SFIuriUnescapeFilter::write
(
   const void* icpBuffer, 
   COLuint32 SizeOfBuffer 
)
{
   const char* cpBuffer = (const char*)icpBuffer;
   char WriteBuffer[SFI_URI_BUFFER_SIZE];
   size_t WriteBufferIndex = 0;
   for (COLuint32 CharIndex = 0; CharIndex < SizeOfBuffer; CharIndex++)
   {
      // DoWrite is set to true if we need to append a char to the internal buffer
      bool DoWrite = false;
      char Value = '\0';
      switch (State)
      {
         case URI_NORM:
            switch (cpBuffer[CharIndex])
            {
               case '%':
                  State = URI_PCT;      
                  break;
               case '+':
                  Value = ' ';
                  DoWrite = true;
                  break;
               default:
                  Value = cpBuffer[CharIndex];
                  DoWrite = true;
                  break;
            }
            break;
         case URI_PCT:
            if(!isxdigit((unsigned char)cpBuffer[CharIndex]))
               ERROR("Invalid %xx sequence.");
            SymbolBuffer[0] = cpBuffer[CharIndex];
            State = URI_PCTHI;
            break;
         case URI_PCTHI:
            if(!isxdigit((unsigned char)cpBuffer[CharIndex]))
               ERROR("Invalid %xx sequence.");
            SymbolBuffer[1] = cpBuffer[CharIndex];
            Value = ::strtol(SymbolBuffer, NULL, 16);
            DoWrite = true;
            State = URI_NORM;
            break;
         default:
            COLMSGPRECONDITION(false, "Invalid state.");
            break;
      }

      if (DoWrite)
      {
         WriteBuffer[WriteBufferIndex] = Value;
         if (++WriteBufferIndex == SFI_URI_BUFFER_SIZE)
         {
            next().write(WriteBuffer, SFI_URI_BUFFER_SIZE);
            WriteBufferIndex = 0;
         }
      }
   }
         
   if (WriteBufferIndex)
   {
      next().write(WriteBuffer, WriteBufferIndex);
   }
   return SizeOfBuffer;
}

void SFIuriUnescapeFilter::resetFilter()
{
   State = URI_NORM;
}

void SFIuriUnescapeFilter::onEndStream()
{
   // make sure we aren't halfway through an escape sequence
   if(State != URI_NORM)
      ERROR("Incomplete %xx sequence.");
   next().onEndStream();
}

// TODO - could be more efficient.
COLstring SFIuriUnescape(const COLstring& Input){
   COLstring Output;
   COLsinkString Sink(Output);
   SFIuriUnescapeFilter Filter(&Sink);
   Filter.write(Input.c_str(), Input.size());
   Filter.onEndStream();
   return Output;
}
