#ifndef __XML_IOS_FILTER_H__
#define __XML_IOS_FILTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Abstract implementation of XML filter
//
// Author: Nasron Cheong
// Date:   Nov 2003
//
//---------------------------------------------------------------------------
#include <COL/COLsink.h>

class XMLiosFilter : public COLsink
{
public:
   XMLiosFilter(COLsink* pSink) : m_pSink(pSink){};
   XMLiosFilter() : m_pSink(0){};
   virtual ~XMLiosFilter(){};

   void setSink(COLsink* pSink) { m_pSink = pSink; }

protected:
   COLsink* m_pSink;

private: 
   XMLiosFilter(const XMLiosFilter& Orig);
   XMLiosFilter& operator=(const XMLiosFilter& Orig);
};

#endif // end of defensive include
