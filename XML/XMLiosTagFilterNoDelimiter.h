#ifndef _IOS_XML_TAG_SINK_NO_DELIMITER_H_
#define _IOS_XML_TAG_SINK_NO_DELIMITER_H_
//---------------------------------------------------------------------------
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Filter that does escaping for Tags in XML streams, but 
// does not differentiate between delimiters and non delimiters.
// Hence, the setXmlDelimiter does nothing.
//
// Author: Nasron Cheong
// Date:   Nov 2003
//
//---------------------------------------------------------------------------
#include <XML/XMLiosTagFilter.h>

class XMLiosTagFilterNoDelimiter : public XMLiosTagFilter
{
public:
   XMLiosTagFilterNoDelimiter();
   virtual ~XMLiosTagFilterNoDelimiter(){};

   //do nothing
   virtual void setXmlDelimiter(char XmlDelimiter){};

private: 
   XMLiosTagFilterNoDelimiter(const XMLiosTagFilterNoDelimiter& Orig);
   XMLiosTagFilterNoDelimiter& operator=(const XMLiosTagFilterNoDelimiter& Orig);
};

#endif // end of defensive include
