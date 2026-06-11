#ifndef __NTV_TREE_VALUE_H__
#define __NTV_TREE_VALUE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTVtreeValue
//
// Description:
//
// NTVtreeValue writes the treeValue for any node to a COLostream.
//
// Author: Kevin Senn
// Date:   Tuesday, November 9th, 2010 @ 03:04:01 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class NODplace;

void NTVtreeValue(COLostream& Stream, const NODplace& Place, bool TRNhtmlPrint);

#endif // end of defensive include
