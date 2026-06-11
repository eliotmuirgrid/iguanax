#ifndef _IOS_XML_ATTRIBUTE_DATA_FILTER_H_
#define _IOS_XML_ATTRIBUTE_DATA_FILTER_H_
//---------------------------------------------------------------------------
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// A filter than redirects write calls to its sink and does xml data escaping in 
// the write
//
// Author: Ryan Dewsbury
// Date:   Mar 2004
//
//---------------------------------------------------------------------------
#include <XML/XMLiosFilter.h>

struct XMLiosAttributeDataFilterPrivate;

class XMLiosAttributeDataFilter : public XMLiosFilter
{
public:
   XMLiosAttributeDataFilter(COLsink* pSink);
   XMLiosAttributeDataFilter();
   virtual ~XMLiosAttributeDataFilter();

   virtual COLuint32 write(const void* cpBuffer, 
                           COLuint32 SizeOfBuffer);

private: 
   friend struct XMLiosAttributeDataFilterPrivate;
   XMLiosAttributeDataFilterPrivate* pMember;
   bool SpaceFlag;
   XMLiosAttributeDataFilter(const XMLiosAttributeDataFilter& Orig);
   XMLiosAttributeDataFilter& operator=(const XMLiosAttributeDataFilter& Orig);
};

#endif // end of defensive include
