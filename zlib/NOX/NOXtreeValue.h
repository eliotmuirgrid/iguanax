#ifndef __NOX_TREE_VALUE_H__
#define __NOX_TREE_VALUE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOXtreeValue
//
// Description:
//
// NOXtreeValue will write XML for XML nodes.  Newlines are represented as
// \n across all platforms.  Is this a good idea?  I don't know, but it sure
// makes testing easier.
//
// If pCallback is present, then newlines and indentation will be omitted.
// Instead of newlines, a single space will be printed out.  This is better
// for display purposes, which is what pCallback is for.
//
// Author: Kevin Senn
// Date:   Tuesday, November 9th, 2010 @ 03:15:34 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class NODplace;

void NOXtreeValue(COLostream& Stream, const NODplace& Place, bool TRNhtmlPrint);

#endif // end of defensive include
