#ifndef __SRCH_UTILITY_H__
#define __SRCH_UTILITY_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SRCHutility
//
// Description:
//
// Implementation
//
// Author: Matthew Sobkowski
// Date:   Monday, June 20th, 2022
//---------------------------------------------------------------------------
#include <cstddef>
class COLstring;
class COLvar;

COLstring SRCHescapeUnprintable(const COLstring& Raw);
void      SRCHhighlightMatch(COLstring& Line, size_t Pos, size_t Length);
void      SRCHfilterMatches(const COLvar& Selections, COLvar* pMatches);

#endif