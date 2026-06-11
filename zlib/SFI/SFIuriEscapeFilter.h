#ifndef __SFI_URI_ESCAPE_FILTER_H__
#define __SFI_URI_ESCAPE_FILTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIuriEscapeFilter
//
// Description:
//
// This class converts a binary data stream into a stream of HEX data.
//
// Author: Matt Lawrence
// Date: August 9, 2006
//
//---------------------------------------------------------------------------

#include <COL/COLfilter.h>

class SFIuriEscapeFilterPrivate;

class SFIuriEscapeFilter : public COLfilter
{
public:
   SFIuriEscapeFilter( COLsink* ipNext );
   virtual ~SFIuriEscapeFilter();

   // This is not to be used on an entire URI. Assuming the following format
   // for a URI, this should only be used on the individual keys or values.
   //
   //   http://<resource>?<key>=<value>&<key>=<value>
   //   
   // Any characters not in the following set will be percent-encoded.
   // [A-Za-z0-9\-._~]
   // if Generous is true, these characters will also be left untouched: !'()*
   // Generous mode matches the behaviour of Javascript's encodeURIComponent().
   virtual COLuint32 write( const void* cpBuffer, COLuint32 SizeOfBuffer );
   COLuint32 write(const void* cpBuffer, COLuint32 SizeOfBuffer, bool SpacesToPlusses, bool Generous);
   virtual void resetFilter() {}
   
private:  
   SFIuriEscapeFilter( SFIuriEscapeFilter& Orig ); // not allowed
   SFIuriEscapeFilter& operator=( SFIuriEscapeFilter& Orig ); // not allowed
};

COLstring SFIuriEscape(const COLstring& Input, bool SpacesToPlusses = true, bool Generous = false);

#endif // end of defensive include
