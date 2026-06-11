//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSMwriteBuffer
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, February 12th, 2007 @ 02:11:52 PM
//
//---------------------------------------------------------------------------
#include "TSMprecomp.h"
#pragma hdrstop

#include "TSMwriteBuffer.h"

#include <memory.h>

TSMwriteBuffer::TSMwriteBuffer(COLsink& iSink)
: SizeOfData(0),
  pSink(&iSink)
{
}

TSMwriteBuffer::~TSMwriteBuffer()
{
   if (SizeOfData > 0)
   {
      flush();
   }
}

void TSMwriteBuffer::flush()
{
   pSink->write(Buffer, SizeOfData);
   pSink->flush();
   SizeOfData = 0;
}

COLuint32 TSMwriteBuffer::write(const void* cpBuffer, COLuint32 SizeOfNewData)
{   
   COLuint32 SizeOfDataRemaining = SizeOfNewData;
   while(SizeOfDataRemaining + SizeOfData > MaxBufferSize)
   {
      memcpy(Buffer + SizeOfData, (COLuint8*)cpBuffer + (SizeOfNewData - SizeOfDataRemaining), MaxBufferSize - SizeOfData);
      SizeOfDataRemaining -= (MaxBufferSize - SizeOfData);
      SizeOfData = MaxBufferSize;
      flush();      
   }   
   memcpy(Buffer + SizeOfData, (COLuint8*)cpBuffer + (SizeOfNewData - SizeOfDataRemaining), SizeOfDataRemaining);
   SizeOfData += SizeOfDataRemaining;

   return SizeOfNewData;
}
