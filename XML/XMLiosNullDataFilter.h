#ifndef __XML_IOS_NULL_DATA_FILTER_H__
#define __XML_IOS_NULL_DATA_FILTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// An XML data filter that does nothing
//
// Author: Nasron Cheong
// Date:   Nov 2003
//
//---------------------------------------------------------------------------
#include <XML/XMLiosDataFilter.h>

class XMLiosNullDataFilter : public XMLiosDataFilter
{
public:
   XMLiosNullDataFilter(COLsink* pSink);
   XMLiosNullDataFilter();
   virtual ~XMLiosNullDataFilter();

private: 
   //overrides XMLiosDataFilter::onDefaultEscape
   void onDefaultEscape(const char Input, COLsink& Output) const;
   XMLiosNullDataFilter(const XMLiosNullDataFilter& Orig);
   XMLiosNullDataFilter& operator=(const XMLiosNullDataFilter& Orig);
};

#endif // end of defensive include
