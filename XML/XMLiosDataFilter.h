#ifndef __XML_IOS_DATA_FILTER_H__
#define __XML_IOS_DATA_FILTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// A filter than redirects write calls to its sink and does xml data escaping in 
// the write
//
// Author: Nasron Cheong
// Date:   Nov 2003
//
//---------------------------------------------------------------------------
#include <XML/XMLiosFilter.h>

struct XMLiosDataFilterPrivate;

class XMLiosDataFilter : public XMLiosFilter
{
public:
   XMLiosDataFilter(COLsink* pSink);
   XMLiosDataFilter();
   virtual ~XMLiosDataFilter();

   COLuint32 write(const void* cpBuffer, 
                   COLuint32 SizeOfBuffer);

   //
   // override this function to define how characters outside the 
   // ascii range are escaped. By default, this will escape by doing
   // &#xHH where HH is the hex representation of the Input char.
   //
   virtual void onDefaultEscape(const char Input, COLsink& Output) const;

private: 
   XMLiosDataFilterPrivate* pMember;
   XMLiosDataFilter(const XMLiosDataFilter& Orig);
   XMLiosDataFilter& operator=(const XMLiosDataFilter& Orig);
};

#endif // end of defensive include
