#ifndef MATCHREGEX_H
#define MATCHREGEX_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: MATCHregex.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-07, 8:51p.m.
//  ---------------------------------------------------------------------------
#include <MATCH/MATCHpositions.h>

bool MATCHisRegex(const COLstring& Filter);
bool MATCHregex(const COLstring& Filter, const COLstring& String, MATCHpositions* pPositions, int MaxMatches);

#endif  // end of defensive include
