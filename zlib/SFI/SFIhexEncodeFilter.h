#ifndef __SFI_HEX_ENCODE_FILTER_H__
#define __SFI_HEX_ENCODE_FILTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIhexEncodeFilter
//
// Description:
//
// This class converts a binary data stream into a stream of HEX data.
//
// Author: Henry Tran
// Date: July 6, 2004 
//
//---------------------------------------------------------------------------

#include <COL/COLfilter.h>

class SFIhexEncodeFilter : public COLfilter
{
public:
   SFIhexEncodeFilter( COLsink* ipNext ) : COLfilter( ipNext ) {}
   SFIhexEncodeFilter() : COLfilter(NULL) {}
   virtual ~SFIhexEncodeFilter() {}

   // Encodes the stream in hex, and writes to next
   virtual COLuint32 write( const void* cpBuffer, COLuint32 SizeOfBuffer );
   virtual void resetFilter(){};
   
private:  
   SFIhexEncodeFilter( SFIhexEncodeFilter& Orig ); // not allowed
   SFIhexEncodeFilter& operator=( SFIhexEncodeFilter& Orig ); // not allowed
};

// void SFIhexEncode(const COLstring& Input, COLstring* pOutput);

COLstring SFIhexEncode(const void* pBuffer, int Size);
COLstring SFIhexEncode(const COLstring& Input);

bool SFIhexEncode(const COLstring& Input, COLstring* pOutput);

#endif // end of defensive include
