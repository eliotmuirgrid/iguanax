#ifndef __NTAB_TREE_VALUE_H__
#define __NTAB_TREE_VALUE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTABtreeValue
//
// Description:
//
// NTABtreeValue class.
//
// Author: Kevin Senn
// Date:   Tuesday, November 16th, 2010 @ 01:46:54 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class NODplace;

void NTABtreeValue(COLostream& Stream, const NODplace& Place, bool TRNhtmlPrint);

// pCallback is required for this one.
void NTABprintColumnValue(COLostream& Stream, const NODplace& Place, bool TRNhtmlPrint);

#endif // end of defensive include
