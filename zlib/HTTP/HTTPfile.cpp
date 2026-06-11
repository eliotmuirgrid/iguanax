//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPfile
//
// Description:
//
// Implementation
//
// Author: Rob Moyse (ported by Eliot)
// Date:   Thursday, December 7th, 2006 @ 03:27:52 PM
//
//---------------------------------------------------------------------------
#include "HTTPprecomp.h"
#pragma hdrstop

#include "HTTPfile.h"

COLuint32 HTTPfile::size() const
{
   return SizeOfData;
}

const void* HTTPfile::data() const
{
   return pData;
}

// This does *not* take ownership
void HTTPfile::setData(const void* pStart, COLuint32 iSizeOfData)
{
   pData = pStart;
   SizeOfData = iSizeOfData;
}

const COLstring& HTTPfile::name() const
{
   return Name;
}

void HTTPfile::setName(const COLstring& iName)
{
   // TODO ... do we need to modify the path separator characters?
   Name = iName;
}

const COLstring& HTTPfile::mimeType() const
{
   return MimeType;
}

void HTTPfile::setMimeType(const COLstring& iMimeType)
{
   MimeType = iMimeType;
}
