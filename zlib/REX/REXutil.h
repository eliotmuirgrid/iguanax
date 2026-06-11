#ifndef __REX_UTIL_H__
#define __REX_UTIL_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: REXutil
//
// Description:
//
// Common helper routines that are useful common regex operations.  They are
// generally useful for convenience - because the regex is created and destroyed
// each time they are generally not going to be good if placed in tight inner
// loops etc. where efficiency matters.
//
// Author: Eliot Muir
// Date:   Friday, December 21st, 2012 @ 09:13:53 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
class COLstring;

// Scan the input and replace patterns matching the regex with the string in With
COLstring REXreplace(const COLstring& Input, const char* Regex, const COLstring& With, bool CaseSensitive = true);

// Returns true if the Input matches the supplied regular expression.
bool REXmatches(const COLstring& Input, const char* Regex);


#endif // end of defensive include
