//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIbase64EncodeFilter
//
// Description:
//
// Used for base 64 conversion of a string.
//
// based on: http://jabberd.jabberstudio.org/1.4/codedoc/base64_8c.html
//
// Author: Rob Moyse
// Date: July 15, 2004 
//
//---------------------------------------------------------------------------
#include "SFIprecomp.h"
#pragma hdrstop

#include "SFIbase64.h"
#include "SFIbase64EncodeFilter.h"

#include <COL/COLsinkString.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <string.h>

class SFIbase64EncodeFilterPrivate
{
public:
   SFIbase64EncodeFilterPrivate(COLuint32 iLineLength, const char* pNewline) 
   :
      LineLength(iLineLength),
      Newline(pNewline),
      TotalAmountWritten(0)
   {
      COLPRECONDITION(LineLength % 4 == 0);
   }
   ~SFIbase64EncodeFilterPrivate(){}

   COLuint32 filter(const COLuint8* pData, COLuint32 Size, COLsink* pSink)
   {
      Size -= Size %3;
      COLuint32 WriteAmount = 0;
      COLuint8 Result[4];
      while(WriteAmount < Size)
      {
         SFIbase64::encodeTriple(pData,Result);
         doNewline(pSink);
         pSink->write(Result,sizeof(Result));
         WriteAmount += 3;
         pData += 3;
         TotalAmountWritten += sizeof(Result);
      }
      return Size;
   }

   void doNewline(COLsink* pSink)
   {
      // Newline strategy: last line never ends with newline, even if it is complete.
      if (LineLength > 0 && TotalAmountWritten > 0 && TotalAmountWritten % LineLength == 0)
      {
         pSink->write(Newline.c_str(), (COLuint32) Newline.size());
      }
   }

   COLuint32 LineLength;
   COLstring Newline;
   COLuint32 TotalAmountWritten;
   COLstring Buffer;
};

SFIbase64EncodeFilter::SFIbase64EncodeFilter(COLsink* ipNext, COLuint32 LineLength, const char* pNewline) 
:
   COLfilter(ipNext)
{
   pMember = new SFIbase64EncodeFilterPrivate(LineLength, pNewline);
}

SFIbase64EncodeFilter::SFIbase64EncodeFilter() 
:
   COLfilter(NULL)
{
   pMember = new SFIbase64EncodeFilterPrivate(SFI_DEFAULT_LINE_LENGTH, SFI_DEFAULT_NEWLINE);
}

SFIbase64EncodeFilter::~SFIbase64EncodeFilter()
{
   delete pMember;
}

COLuint32 SFIbase64EncodeFilter::write(const void* cpBuffer, COLuint32 SizeOfBuffer )
{
   if (pMember->Buffer.size())
   {
      pMember->Buffer.append((const char*)cpBuffer, SizeOfBuffer);
      
      while (pMember->Buffer.size())
      {
         COLuint32 AmountFiltered = pMember->filter((COLuint8*)pMember->Buffer.c_str(), pMember->Buffer.size(),&next());
         if (!AmountFiltered) {
            break; //need more data
         }
         pMember->Buffer.remove(0, AmountFiltered);
      }
   }
   else {
      COLuint32 TotalAmountFiltered = 0;
      const COLuint8* pData = (const COLuint8*)cpBuffer;
      while (TotalAmountFiltered < SizeOfBuffer)
      {
         COLuint32 AmountFiltered = pMember->filter(pData,SizeOfBuffer-TotalAmountFiltered,&next());
         if (!AmountFiltered)
         {
            break; //need more data
         }
         pData += AmountFiltered;
         TotalAmountFiltered += AmountFiltered;
      }
      //buffer the rest
      if (TotalAmountFiltered < SizeOfBuffer) {
         pMember->Buffer.append((const char*)pData, SizeOfBuffer - TotalAmountFiltered);
      }
   }
   return SizeOfBuffer;
}

void SFIbase64EncodeFilter::resetFilter()
{
   pMember->Buffer.clear();
   pMember->TotalAmountWritten = 0;
}

void SFIbase64EncodeFilter::onEndStream()
{
   //if theres data left in the buffer, pad it so it can be written out
   if (pMember->Buffer.size())
   {
      pMember->doNewline(&next());
   
      COLPRECONDITION(pMember->Buffer.size() <= 2); //we should only have a remaining 2 or 1 bytes
      if (pMember->Buffer.size() == 1)
      {
         COLuint8 Triple[3];
         Triple[0] = pMember->Buffer[0];
         Triple[1] = 0;
         Triple[2] = 0;
         COLuint8 Result[4];
         SFIbase64::encodeTriple(Triple,Result);
         Result[2] = '=';
         Result[3] = '=';
         next().write(Result,sizeof(Result));
         pMember->Buffer.remove(0, 1);
      }
      else if (pMember->Buffer.size() == 2)
      {
         COLuint8 Triple[3];
         Triple[0] = pMember->Buffer[0];
         Triple[1] = pMember->Buffer[1];
         Triple[2] = 0;
         COLuint8 Result[4];
         SFIbase64::encodeTriple(Triple,Result);
         Result[3] = '=';
         next().write(Result,sizeof(Result));
         pMember->Buffer.remove(0, 2);
      }
   }
   COLPOSTCONDITION(pMember->Buffer.size() == 0);
   COLfilter::onEndStream();
}

void SFIbase64Encode(const COLstring& Input, COLstring* pOutput){
   COL_FUNCTION(SFIbase64Encode);
   COLsinkString Sink(pOutput, false);
   SFIbase64EncodeFilter Filter(&Sink);
   Filter.write(Input.c_str(), Input.size());
   Filter.onEndStream();
}

COLstring SFIbase64Encode(const COLstring& Input){
   COL_FUNCTION(SFIbase64Encode);
   COLstring Result;
   SFIbase64Encode(Input, &Result);
   return Result;
}

void SFIbase64EncodeOneLine(const COLstring& Input, COLstring* pOutput){
   COL_FUNCTION(SFIbase64EncodeOneLine);
   COLsinkString Sink(pOutput, false);
   SFIbase64EncodeFilter Filter(&Sink, 0);
   Filter.write(Input.c_str(), Input.size());
   Filter.onEndStream();
}
