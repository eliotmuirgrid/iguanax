#ifndef __SFI_HEX_VALID_FILTER_H__
#define __SFI_HEX_VALID_FILTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIhexValidFilter
//
// Description:
//
// This class filters out invalid HEX data from the stream.
//
// Author: Henry Tran
// Date: July 6, 2004 
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLfilter.h>

class SFIhexValidFilter : public COLfilter
{
public:
   SFIhexValidFilter( COLsink* ipNext ) : COLfilter( ipNext ) {}
   virtual ~SFIhexValidFilter() {}

   // This will strip out any non-hex characters in the buffer, basically
   // any that don't match [0-9A-Fa-f]
   virtual COLuint32 write( const void* cpBuffer, COLuint32 SizeOfBuffer );
   virtual void resetFilter(){};

private:
   SFIhexValidFilter( SFIhexValidFilter& Orig ); // not allowed
   SFIhexValidFilter& operator=( SFIhexValidFilter& Orig ); // not allowed
};

#endif // end of defensive include

