#ifndef __TRN_RENDER_NOD_TREE_H__
#define __TRN_RENDER_NOD_TREE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNrenderNodTree
//
// Description:
//
// Helper
//
// Author: Eliot Muir
// Date:   Wednesday, November 24th, 2010 @ 01:19:34 PM
//
//---------------------------------------------------------------------------

class COLvar;
class COLstring;
class TRNarg;
class NODplace;

void TRNrenderNodTree(NODplace& Place, const TRNarg& Arg, const COLstring& RootAddressJson, const COLstring& ExpansionTreeJson, COLvar* pTree);

#endif // end of defensive include
