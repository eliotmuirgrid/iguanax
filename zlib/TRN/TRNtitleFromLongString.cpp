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
#include <SFI/SFIhtmlEscape.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static unsigned int TRN_MAX_TITLE_SIZE = 100;

void TRNtitleFromLongString(const COLstring& LongString, COLstring& Title){
   bool Truncate = (LongString.size() > TRN_MAX_TITLE_SIZE);
   // The "-1" is to make up for the length of "&hellip;".
   SFIhtmlEscapeNoNewline(LongString.c_str(), (Truncate ? TRN_MAX_TITLE_SIZE-1 : LongString.size()), Title);
   if (Truncate){
      Title.append("&hellip;", 8);
   }
}
