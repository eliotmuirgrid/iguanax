#ifndef __TRN_ADD_SPECIFIC_NODE_TYPE_OPTIONS_H__
#define __TRN_ADD_SPECIFIC_NODE_TYPE_OPTIONS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNaddSpecificNodeTypeOptions
//
// Description:
//
// Helper used for intellisense code.
//
// Author: Eliot Muir
// Date:   Monday, December 20th, 2010 @ 02:09:31 PM
//---------------------------------------------------------------------------

class COLstring;
class NODplace;
class TRNsortedOptionsList;

void TRNaddSpecificNodeTypeOptions(const NODplace& Place, const COLstring& RootVar, TRNsortedOptionsList& Options);

#endif // end of defensive include
