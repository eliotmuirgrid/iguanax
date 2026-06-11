#ifndef __SFI_XML_TAG_ENCODE_FILTER_H__
#define __SFI_XML_TAG_ENCODE_FILTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIxmlTagEncodeFilter
//
// Description:
//
// XML tag encoder 
//
// Author: Matt Lawrence
// Date:   Wednesday, July 26th, 2006 @ 02:16:28 PM
//---------------------------------------------------------------------------

#include <COL/COLfilter.h>

// WARNING THIS CLASS NEEDS SOME LOVE - see ticket #24133

class SFIxmlTagEncodeFilterPrivate;

class SFIxmlTagEncodeFilter : public COLfilter
{
public:
   SFIxmlTagEncodeFilter(COLsink* ipNext);
   virtual ~SFIxmlTagEncodeFilter();

   // This will clean the buffer such that it conforms to the XML standard
   // for tag naming. Basically, it will be reduced to alphanumeric characters
   // along with :-_. Additionally, the first character is guaranteed to be
   // either an alpha character, a : or a . If it is numeric or -_, then it is
   // replaced with an underscore.
   // 
   // Note: resetFilter() MUST be called in between tags, or else escaping may
   //       not occur correctly.
   virtual COLuint32 write( const void* cpBuffer, COLuint32 SizeOfBuffer );
   virtual void resetFilter();
   
private:
   SFIxmlTagEncodeFilter(const SFIxmlTagEncodeFilter& Orig); // not allowed
   SFIxmlTagEncodeFilter& operator=(const SFIxmlTagEncodeFilter& Orig); // not allowed

   SFIxmlTagEncodeFilterPrivate* pMember;
};

COLstring SFIxmlTagEncode(const COLstring& Input);

#endif // end of defensive include
