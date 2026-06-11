#ifndef MATCHSEARCH_H
#define MATCHSEARCH_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: MATCHsearch.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-07, 8:38p.m.
//  ---------------------------------------------------------------------------
#include <MATCH/MATCHpositions.h>

// Our backend regex returns end positions inclusive, which means do end + 1 if you want to get the correct substring
bool MATCHsearch(const COLstring& Filter, const COLstring& String, bool ExactMatch, MATCHpositions* pPositions,
                 int MaxMatches = 1000);

// use this if you dont care about positions
bool MATCHsearch(const COLstring& Filter, const COLstring& String, bool ExactMatch, int MaxMatches = 1000);

#endif  // end of defensive include
