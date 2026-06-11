//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNargRequest
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, November 24th, 2010 @ 01:19:34 PM
//---------------------------------------------------------------------------
#include <SFI/SFIhtmlCrop.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static unsigned int TRN_MAX_TITLE_SIZE = 100;

void TRNtitleFromHtmlString(const COLstring& HtmlString, COLstring& Title){
   // The "-1" is to make up for the length of "&hellip;".
   COL_VAR(HtmlString);
   SFIhtmlCrop(HtmlString.c_str(), HtmlString.size(), "&hellip;", TRN_MAX_TITLE_SIZE-1, Title);
}
