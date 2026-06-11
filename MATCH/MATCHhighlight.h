#ifndef __MATCHHIGHLIGHT_H__
#define __MATCHHIGHLIGHT_H__
// //  ---------------------------------------------------------------------------
// //  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
// //
// //  Module: MATCHhighlight.h
// //
// //  Description: Implementation
// //
// //  Author: Matthew Sobkowski
// //  Date:   14/05/24 2:16 PM
// //  ---------------------------------------------------------------------------
#include <MATCH/MATCHpositions.h>
class COLstring;

// use this if you have already found positions using MATCHsearch
COLstring MATCHgenerateMarkup(const COLstring& String, const MATCHpositions& Positions,
                              bool ConvertUrls, bool EscapeHtml, int Threshold = -1, int Maxlines = -1);

// returns number of matches
int MATCHsearchAndCreateMarkup(const COLstring& Filter, const COLstring& String, bool Exact, COLstring* pOut,
                               bool ConvertUrls, int Threshold, int Maxlines, bool EscapeHtml=true);

#endif  // end of defensive include
