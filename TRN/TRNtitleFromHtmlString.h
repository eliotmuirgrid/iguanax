#ifndef __TRN_TITLE_FROM_HTML_STRING_H__
#define __TRN_TITLE_FROM_HTML_STRING_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNtitleFromHtmlString
//
// Description:
//
// A helper function - crop an already HTML-formatted string
//
// Author: Eliot Muir
// Date:   Wednesday, November 24th, 2010 @ 01:19:34 PM
//
//---------------------------------------------------------------------------

class COLstring;

// Will crop an already HTML-formatted string.
void TRNtitleFromHtmlString(const COLstring& HtmlString, COLstring& Title);

#endif // end of defensive include
