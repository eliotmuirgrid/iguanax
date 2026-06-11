//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNrenderNodTree
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, November 24th, 2010 @ 01:19:34 PM
//---------------------------------------------------------------------------
#include "TRNrenderNodTree.h"
#include "TRNargumentRequest.h"
#include "TRNtreeview.h"
#include <NOD/NODaddress.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

 void TRNrenderNodTree(NODplace& Place, const TRNarg& Arg, const COLstring& RootAddressJson,
                             const COLstring& ExpansionTreeJson, COLvar* pTree) {
   COL_FUNCTION(TRNrenderNodTree);
   NODaddress Address;
   Address.setParent(&Place);
   TRNbuildVarTree(Address, RootAddressJson, ExpansionTreeJson, Arg.m_FileName, Arg.m_FunctionName, Arg.m_Call, Arg.m_Line, pTree);
}
