#ifndef _MATCHSQL_H
#define _MATCHSQL_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: MATCHsql.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-09-12, 11:44a.m.
//  ---------------------------------------------------------------------------
#include <MATCH/MATCHpositions.h>

bool MATCHsql(const COLstring& Filter, const COLstring& String, bool ExactMatch, MATCHpositions* pPositions,
              int MaxMatches);

#endif  // end of defensive include
