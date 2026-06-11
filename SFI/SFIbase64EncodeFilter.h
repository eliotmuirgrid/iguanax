#ifndef __SFI_BASE64ENCODE_FILTER_H__
#define __SFI_BASE64ENCODE_FILTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIbase64encodeFilter
//
// Description: base64 encode COLfilter
//
// Author: Nasron Cheong
// Date: July 15, 2004 
//
//---------------------------------------------------------------------------

#include <COL/COLfilter.h>

class SFIbase64EncodeFilterPrivate;

#define SFI_DEFAULT_LINE_LENGTH 76
#define SFI_DEFAULT_NEWLINE "\r\n"

class SFIbase64EncodeFilter : public COLfilter
{
public:
   SFIbase64EncodeFilter(COLsink* ipNext, 
                         COLuint32 LineLength = SFI_DEFAULT_LINE_LENGTH, // must be divisible by 4, or 0 for no line breaks
                         const char* pNewline = SFI_DEFAULT_NEWLINE);
   SFIbase64EncodeFilter();
   virtual ~SFIbase64EncodeFilter();

   virtual void resetFilter();

   //encodes the stream in base64, and writes to next
   virtual COLuint32 write(const void* cpBuffer, COLuint32 SizeOfBuffer );

   virtual void onEndStream();

private:
   SFIbase64EncodeFilterPrivate* pMember;

   COLuint32 AmountUntilChunk;   
   COLuint32 ChunkLength;
   SFIbase64EncodeFilter(const SFIbase64EncodeFilter&);
   SFIbase64EncodeFilter& operator=(const SFIbase64EncodeFilter&);   
};

void SFIbase64Encode(const COLstring& Input, COLstring* pOutput);
COLstring SFIbase64Encode(const COLstring& Input);
void SFIbase64EncodeOneLine(const COLstring& Input, COLstring* pOutput);

#endif // end of defensive include
