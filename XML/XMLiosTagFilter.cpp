//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLiosTagFilter
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   Feb 17 2004 (last known history)
//
//---------------------------------------------------------------------------
#include "XMLprecomp.h"
#pragma hdrstop

#include <XML/XMLiosTagFilter.h>
#include <XML/XMLiosTagFilterPrivate.h>

XMLiosTagFilter::XMLiosTagFilter(char XmlDelimiter, COLsink* pSink)
 : XMLiosFilter(pSink)
{
   pMember = new XMLiosTagFilterPrivate(XmlDelimiter);
}

XMLiosTagFilter::XMLiosTagFilter(char XmlDelimiter)
{
   pMember = new XMLiosTagFilterPrivate(XmlDelimiter);
}

XMLiosTagFilter::~XMLiosTagFilter()
{
   delete pMember;
}

void XMLiosTagFilter::setXmlDelimiter(char XmlDelimiter)
{
   //assume that delimiters are either nullescaped or not escaped
   //reset the current delimiter to noescape
   pMember->EscapeFunctions[(COLuint8)pMember->XmlDelimiter] = XMLiosTagFilterPrivate::NoEscape;

   //set the new delimiter
   pMember->EscapeFunctions[(COLuint8)XmlDelimiter] = XMLiosTagFilterPrivate::NullEscape;   
   pMember->XmlDelimiter = XmlDelimiter;
}

COLuint32 XMLiosTagFilter::write(const void* cpBuffer, COLuint32 SizeOfBuffer)
{
   COLuint8 Character;
   for (COLuint32 CharIndex = 0; CharIndex < SizeOfBuffer; CharIndex++)
   {
      Character = ((COLuint8*)cpBuffer)[CharIndex];
      (*pMember->EscapeFunctions[Character])(Character,*m_pSink);
   }
   return SizeOfBuffer;
}
