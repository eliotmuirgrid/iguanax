#ifndef __NTV_HL7_TREE_VALUE_H__
#define __NTV_HL7_TREE_VALUE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTVhl7TreeValue
//
// Description:
//
// NTVhl7TreeValue writes treeValues for HL7 nodes.
//
// Author: Kevin Senn
// Date:   Wednesday, November 10th, 2010 @ 03:20:50 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class NODplace;
class NODprintCallback;

void NTVhl7TreeValue(COLostream& Stream, const NODplace& Place, bool TRNhtmlPrint);

#endif // end of defensive include
