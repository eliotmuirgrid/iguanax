#ifndef _MATCHUTILS_H
#define _MATCHUTILS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: MATCHutils.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-08, 9:57p.m.
//  ---------------------------------------------------------------------------
#include <MATCH/MATCHpositions.h>
class COLstring;

void MATCHfindExclusions(const COLstring& Input, MATCHpositions* pPositions);
bool MATCHoverlapsExclusions(size_t Start, size_t End, const MATCHpositions& Exclusions);
COLstring MATCHescapeRegex(const COLstring& String);

#endif  // end of defensive include
