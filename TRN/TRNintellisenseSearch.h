#ifndef __TRN_INTELLISENSE_SEARCH_H__
#define __TRN_INTELLISENSE_SEARCH_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNintellisenseSearch
//
// Description:
//
// TRNintellisenseSearch - functions used to implement intellisense with search
// aka "Deep Intellisense"
//
// Author: Eliot Muir
// Date:   Tuesday, December 21st, 2010 @ 06:47:21 AM
//
//---------------------------------------------------------------------------

#include "TRNintellisenseContext.h"

#include <LUA/LUAutils.h>
class TRNtraverser;
class REXmatcher;

void TRNaddSearchMatchNodes(TRNtraverser& Traverser, const COLstring& RootVar, const COLstring& SearchString, TRNsortedOptionsList& Options, lua_State* L);

#endif // end of defensive include
