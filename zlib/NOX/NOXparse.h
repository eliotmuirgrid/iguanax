#ifndef __NOX_PARSE_H__
#define __NOX_PARSE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOXparse
//
// Description:
//
// Contains NOXparseFile, a function which will parse an XML file.
//
// Author: Kevin Senn
// Date:   Wednesday, June 23rd, 2010 @ 09:32:20 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class NODplace;
class DOMnode;

// Remove all comment nodes, and text nodes which are just whitespace.
//
void NOXcleanDomTree(DOMnode* pNode);

NODplace* NOXparseString(const COLstring& XmlString);

#endif // end of defensive include
