#ifndef __SFI_URI_UNESCAPE_FILTER_H__
#define __SFI_URI_UNESCAPE_FILTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: SFIuriUnescapeFilter
//
// Description:
// 
// A filter that will unescaped '%' encoded data.
//
// Author: Nasron Cheong
// Date:   Nov 2005
//
//---------------------------------------------------------------------------

#include <COL/COLfilter.h>

class SFIuriUnescapeFilter : public COLfilter
{
public:
   SFIuriUnescapeFilter(COLsink* pNext);
   virtual ~SFIuriUnescapeFilter();

   // Unencodes any percent-escaped entities in the buffer
   virtual COLuint32 write( const void* cpBuffer, 
                            COLuint32 SizeOfBuffer );

   virtual void resetFilter();

   virtual void onEndStream();

private:
   SFIuriUnescapeFilter( const SFIuriUnescapeFilter& Orig );
   SFIuriUnescapeFilter& operator=( const SFIuriUnescapeFilter& Orig );

   enum { URI_NORM, URI_PCT, URI_PCTHI } State;
   char SymbolBuffer[3];
};

COLstring SFIuriUnescape(const COLstring& Input);

#endif // end of defensive include
