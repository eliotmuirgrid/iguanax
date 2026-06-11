#ifndef __NOX_CONSTANT_H__
#define __NOX_CONSTANT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOXconstant
//
// Description:
//
// NOXconstant class.
//
// Author: Kevin Senn
// Date:   Tuesday, June 22nd, 2010 @ 03:05:03 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

extern const COLstring NOXprotocolName;

extern const COLstring NOXelementName;
extern const COLstring NOXattributeName;
extern const COLstring NOXcdataName;
extern const COLstring NOXtextName;
extern const COLstring NOXdocName;

void NOXregisterTypes();

enum NOXconstant
{
   NOX_XML=30,
   NOX_ATTRIBUTE=1,
   NOX_CDATA=2,
   NOX_DOCUMENT=3,
   NOX_ELEMENT=4,
   NOX_TEXT=5
};

#endif // end of defensive include
