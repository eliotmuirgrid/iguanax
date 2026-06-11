#ifndef __SFI_XML_DATA_ENCODE_FILTER_H__
#define __SFI_XML_DATA_ENCODE_FILTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIxmlDataEncodeFilter
//
// Description:
//
// SFIxmlDataEncodeFilter class.
//
// Author: Matt Lawrence
// Date:   Wednesday, July 26th, 2006 @ 02:16:28 PM
//---------------------------------------------------------------------------

#include <COL/COLfilter.h>

// WARNING THIS CLASS NEEDS SOME LOVE - see ticket #24133

struct SFIxmlDataEncodeFilterPrivate;

class SFIxmlDataEncodeFilter : public COLfilter
{
public:
   SFIxmlDataEncodeFilter(COLsink* ipNext);
   virtual ~SFIxmlDataEncodeFilter();

   // This will encode the buffer such that it conforms to the XML rules
   // for attribute data. Basically, any of '"&<> will be replaced by the
   // appropriate entity.
   virtual COLuint32 write( const void* cpBuffer, COLuint32 SizeOfBuffer );
   virtual void resetFilter() {}
   
private:
   SFIxmlDataEncodeFilter(const SFIxmlDataEncodeFilter& Orig); // not allowed
   SFIxmlDataEncodeFilter& operator=(const SFIxmlDataEncodeFilter& Orig); // not allowed

   SFIxmlDataEncodeFilterPrivate *pMember;
};

void SFIxmlDataEncode(const COLstring& Input, COLstring* pOutput);
COLstring SFIxmlDataEncode(const COLstring& Input);

#endif // end of defensive include
