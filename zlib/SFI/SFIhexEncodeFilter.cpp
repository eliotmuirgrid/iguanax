//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIhexEncodeFilter
//
// Description:
//
// Implementation.
//
// Author: Henry Tran
// Date: July 6, 2004
//
//---------------------------------------------------------------------------
#include "SFIprecomp.h"
#pragma hdrstop

#include "SFIhexEncodeFilter.h"

#include <COL/COLsinkString.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLuint32 SFIhexEncodeFilter::write(const void* pVoidBuffer, COLuint32 SizeOfBuffer){
   const unsigned char* pBuffer = (const unsigned char*)pVoidBuffer;
   COLsink& OutputSink = next();
   COLuint32 CharIndex = 0; 
   static const char hex[] = "0123456789ABCDEF";
   int OutputBufferCount = 0;
   const int OutputBufferSize = 256;    // size *MUST* be an even number.
   char OutputBuffer[OutputBufferSize]; // 256 seems to be optimal for core2 in tests
   for (CharIndex = 0; CharIndex < SizeOfBuffer; CharIndex++)
   {
      unsigned Value = pBuffer[CharIndex];
      OutputBuffer[OutputBufferCount++] = hex[Value >> 4];
      OutputBuffer[OutputBufferCount++] = hex[Value & 0xf];
      if (sizeof(OutputBuffer) == OutputBufferCount)
      {
         OutputSink.write(OutputBuffer, OutputBufferCount);
         OutputBufferCount = 0;
      }
   }
   if (OutputBufferCount > 0)
   {
      OutputSink.write(OutputBuffer, OutputBufferCount);
   }
   return SizeOfBuffer;
}

// Eliot - not super efficient creating an object et all but a fast way to get COLsink out of COLstring.
// void SFIhexEncode(const COLstring& Input, COLstring* pOutput){
//    COLsinkString Sink(pOutput, false);
//    SFIhexEncodeFilter Filter(&Sink);
//    Filter.write(Input.c_str(), Input.size());
// }

COLstring SFIhexEncode(const void* pBuffer, int Size){
   COLstring Output;
   COLsinkString Sink(Output);
   SFIhexEncodeFilter Filter(&Sink);
   Filter.write(pBuffer, Size);
   return Output;
}

COLstring SFIhexEncode(const COLstring& Input){
   return SFIhexEncode((void*)Input.c_str(), Input.size());
}

bool SFIhexEncode(const COLstring& Input, COLstring* pOutput){
   COL_FUNCTION(SFIhexEncode);
   COLsinkString Sink(pOutput, false);
   SFIhexEncodeFilter Filter(&Sink);
   try { 
      Filter.write(Input.c_str(), Input.size());
      return true;
   }
   catch (const COLerror& Error) { 
      *pOutput = "Error with hexadecimal encoding"; 
      return false;
   }
}