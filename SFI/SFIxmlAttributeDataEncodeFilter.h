#ifndef __SFI_XML_ATTRIBUTE_DATA_ENCODE_FILTER_H__
#define __SFI_XML_ATTRIBUTE_DATA_ENCODE_FILTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIxmlAttributeDataEncodeFilter
//
// Description:
//
// SFIxmlAttributeDataEncodeFilter class.
//
// Author: Matt Lawrence
// Date:   Wednesday, July 26th, 2006 @ 02:16:28 PM
//---------------------------------------------------------------------------

#include <COL/COLfilter.h>

// WARNING THIS CLASS NEEDS SOME LOVE - see ticket #24133

class SFIxmlAttributeDataEncodeFilterPrivate;

class SFIxmlAttributeDataEncodeFilter : public COLfilter
{
public:
   SFIxmlAttributeDataEncodeFilter(COLsink* ipNext);
   virtual ~SFIxmlAttributeDataEncodeFilter();

   // This will encode the buffer such that it conforms to the XML rules
   // for attribute data. Basically, any of '"&< will be replaced by the
   // appropriate entity.
   virtual COLuint32 write( const void* cpBuffer, COLuint32 SizeOfBuffer );
   virtual void resetFilter() {}
   
private:
   SFIxmlAttributeDataEncodeFilter(const SFIxmlAttributeDataEncodeFilter& Orig); // not allowed
   SFIxmlAttributeDataEncodeFilter& operator=(const SFIxmlAttributeDataEncodeFilter& Orig); // not allowed

   SFIxmlAttributeDataEncodeFilterPrivate* pMember;
};

void SFIxmlAttributeDataEncode(const COLstring& Input, COLstring* pOutput);

COLstring SFIxmlAttributeDataEncode(const COLstring& Input);

#endif // end of defensive include
