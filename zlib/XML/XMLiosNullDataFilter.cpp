//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc. All Rights Reserved
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
#include "XMLprecomp.h"
#pragma hdrstop

#include <XML/XMLiosNullDataFilter.h>

XMLiosNullDataFilter::XMLiosNullDataFilter(COLsink* ipSink)
: XMLiosDataFilter(ipSink)
{   
}

XMLiosNullDataFilter::XMLiosNullDataFilter()
{
}

XMLiosNullDataFilter::~XMLiosNullDataFilter()
{
}

void XMLiosNullDataFilter::onDefaultEscape(const char Input, COLsink& Output) const
{
   //this is really sanity check
   COLPRECONDITION(m_pSink == &Output);
   static const COLuint32 charLen = sizeof(char);
   Output.write(&Input,charLen);
}
