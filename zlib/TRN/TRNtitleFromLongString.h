#ifndef __TRN_TITLE_FROM_LONG_STRING_H__
#define __TRN_TITLE_FROM_LONG_STRING_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNtitleFromLongString
//
// Description:
//
// A helper function - crop and HTML escape the string
//
// Author: Eliot Muir
// Date:   Wednesday, November 24th, 2010 @ 01:19:34 PM
//
//---------------------------------------------------------------------------

class COLstring;

// Will crop and HTML escape the string.
void TRNtitleFromLongString(const COLstring& LongString, COLstring& Title);

#endif // end of defensive include
