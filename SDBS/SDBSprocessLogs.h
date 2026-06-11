#ifndef _SDBSPROCESSLOGS_H
#define _SDBSPROCESSLOGS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBSprocessLogs.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-20, 3:50p.m.
//  ---------------------------------------------------------------------------
#include <COL/COLlist.h>
#include <COL/COLarray.h>
class COLwebRequest;
class SDBcontent;

bool SDBSprocessList(const COLlist<SDBcontent*>& List, int Limit, bool Forward, const COLstring& MessageSearch,
                     const COLstring& TagSearch, bool Exact, bool IncludeNonMatches, bool Formatted, bool Minify,
                     bool Relevant, COLarray<COLvar>& Results);

#endif  // end of defensive include
