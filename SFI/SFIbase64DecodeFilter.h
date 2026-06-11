#ifndef __SFI_BASE64DECODE_FILTER_H__
#define __SFI_BASE64DECODE_FILTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIbase64DecodeFilter
//
// Description: base64 Decode COLfilter
//
// Author: Nasron Cheong
// Date: July 15, 2004 
//
//---------------------------------------------------------------------------

#include <COL/COLfilter.h>

class SFIbase64DecodeFilter : public COLfilter
{
public:
   SFIbase64DecodeFilter(COLsink* pNext_) : COLfilter(pNext_), CurrentGroupIndex(0) {};
   SFIbase64DecodeFilter() : COLfilter(NULL), CurrentGroupIndex(0) {};
   virtual ~SFIbase64DecodeFilter(){};

   virtual void resetFilter();

   //Decodes the stream in base64, and writes to next
   virtual COLuint32 write(const void* cpBuffer, COLuint32 SizeOfBuffer );

   //flushes buffer
   virtual void onEndStream(); 

private:
   char CurrentGroup[4];
   COLuint32 CurrentGroupIndex;
   SFIbase64DecodeFilter(const SFIbase64DecodeFilter&);
   SFIbase64DecodeFilter& operator=(const SFIbase64DecodeFilter&);   
};

void SFIbase64Decode(const COLstring& Input, COLstring* pOutput);

#endif // end of defensive include
