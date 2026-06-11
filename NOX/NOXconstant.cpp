//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOXconstant
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, June 22nd, 2010 @ 03:05:03 PM
//
//---------------------------------------------------------------------------
#include "NOXprecomp.h"
#pragma hdrstop

#include "NOXconstant.h"

#include <NOD/NODtype.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

const COLstring NOXprotocolName("xml");
const COLstring NOXelementName("element");
const COLstring NOXattributeName("attribute");
const COLstring NOXcdataName("cdata");
const COLstring NOXtextName("text");
const COLstring NOXdocName("document");

void NOXregisterTypes(){
   COL_FUNCTION(NOXregisterTypes);
   NODtypeMapAdd(NOXelementName,   NOX_ELEMENT,   NOXprotocolName, NOX_XML);
   NODtypeMapAdd(NOXattributeName, NOX_ATTRIBUTE, NOXprotocolName, NOX_XML);
   NODtypeMapAdd(NOXcdataName,     NOX_CDATA,     NOXprotocolName, NOX_XML);
   NODtypeMapAdd(NOXtextName,      NOX_TEXT,      NOXprotocolName, NOX_XML);
   NODtypeMapAdd(NOXdocName,       NOX_DOCUMENT,  NOXprotocolName, NOX_XML);
}
