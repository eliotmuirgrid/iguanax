#ifndef _IOS_XML_TAG_FILTER_H_
#define _IOS_XML_TAG_FILTER_H_
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Filter that does escaping for Tags in XML streams
//
// Author: Nasron Cheong
// Date:   Nov 2003
//
//---------------------------------------------------------------------------
#include <XML/XMLiosFilter.h>

struct XMLiosTagFilterPrivate;

class XMLiosTagFilter : public XMLiosFilter
{
public:
   XMLiosTagFilter(char XmlDelimiter, COLsink* pSink);
   XMLiosTagFilter(char XmlDelimiter);
   virtual ~XMLiosTagFilter();

   virtual COLuint32 write(const void* cpBuffer, 
                           COLuint32 SizeOfBuffer);
 
   virtual void setXmlDelimiter(char XmlDelimiter);

protected:
   XMLiosTagFilterPrivate* pMember;   

private: 
   XMLiosTagFilter(const XMLiosTagFilter& Orig);
   XMLiosTagFilter& operator=(const XMLiosTagFilter& Orig);
};

#endif // end of defensive include
