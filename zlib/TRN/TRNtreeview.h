#ifndef __TRN_TREEVIEW_H__
#define __TRN_TREEVIEW_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNtreeview
//
// Description:
//
// TRNtreeview.  Contains functions for writing out JSON for treeviews
// for NOD trees.
// Note that this module was moved from NWBtreeview.
//
// Author: Kevin Senn
// Date:   Wednesday, May 26th, 2010 @ 11:42:27 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLvar.h>

#include <LUA/LUAheader.h>

class NODaddress;

// These two are COLvar versions of the functions above. Using a COLvar allows the tree to be manipulated downstream.
void TRNbuildVarTree(const NODaddress& TreeRoot, const COLstring& StartingAddressJson, const COLstring& ExpansionTreeJson,
                     const COLstring& FileName, const COLstring& FunctionName, const int Call, const int Line,
                     COLvar* pTree);

#endif // end of defensive include
