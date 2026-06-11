#ifndef __NODB_TREE_VALUE_H__
#define __NODB_TREE_VALUE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODBtreeValue
//
// Description:
//
// NODBtreeValue will write treeValues for NODB nodes.
//
// Author: Kevin Senn
// Date:   Tuesday, November 9th, 2010 @ 04:31:43 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class NODplace;

void NODBtreeValue(COLostream& Stream, const NODplace& Place, bool TRNhtmlPrint);

void NODBprintColumnValue(COLostream& Stream, const NODplace& Place);

#endif // end of defensive include
