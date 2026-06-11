//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNformatUntruncatedString
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Saturday, November 20th, 2010 @ 02:38:37 PM
//---------------------------------------------------------------------------
#include "TRNformatUntruncatedString.h"
#include <THTM/THTMprinter.h>

#include <COL/COLostream.h>

void TRNformatUntruncatedString(const char* pData, int Size, COLostream& Stream){
   Stream << COL_T("<span class=\'st\'>\'");
   THTMprintEscapedLiteral(Stream, pData, Size);
   Stream << COL_T("\'</span>");
}
