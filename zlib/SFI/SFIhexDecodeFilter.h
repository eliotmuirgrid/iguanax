#ifndef __SFI_HEX_DECODE_FILTER_H__
#define __SFI_HEX_DECODE_FILTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIhexDecodeFilter
//
// Description:
//
// This class converts a HEX data stream into a stream of binary data.
//
// Author: Henry Tran
// Date: July 6, 2004 
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLfilter.h>

class SFIhexDecodeFilterPrivate;

class SFIhexDecodeFilter : public COLfilter
{
public:
   SFIhexDecodeFilter(COLsink* ipNext);
   virtual ~SFIhexDecodeFilter();

   // Decodes the stream in hex, and writes to next
   virtual COLuint32 write( const void* cpBuffer, COLuint32 SizeOfBuffer );

   virtual void resetFilter();

   // If any bytes left over after processing - application should raise an error.
   COLuint32 unprocessedCharacterCount() const;

private:
   SFIhexDecodeFilterPrivate* pMember;

   SFIhexDecodeFilter( SFIhexDecodeFilter& Orig ); // not allowed
   SFIhexDecodeFilter& operator=( SFIhexDecodeFilter& Orig ); // not allowed
};

// Returns false if fails - error message will be in pOutput
bool SFIhexDecode(const COLstring& Input, COLstring* pOutput);

bool SFIisHex(const COLstring& Input);


#endif // end of defensive include
