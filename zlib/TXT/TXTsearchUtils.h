#ifndef __TXT_SEARCH_UTILS_H__
#define __TXT_SEARCH_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TXTsearchUtils
//
// Description:
//
// Contains various functions that help with searching through blocks of text.
//
// Author: Eric Mulvaney
// Date:   Wednesday, April 29th, 2009 @ 11:43:18 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

// Perform a case-insensitive search.  Search for pNeedle in pHaystack.
// Returns a pointer to the first occurrence of pNeedle in pHaystack, or
// NULL if pNeedle was not found.
const char* TXTcaseInsensitiveSearch(const char* pHaystack, const char* pNeedle);

#endif // end of defensive include
