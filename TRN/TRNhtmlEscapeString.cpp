//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNhtmlEscapeString
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, December 20th, 2010 @ 02:27:26 PM
//---------------------------------------------------------------------------
#include "TRNhtmlEscapeString.h"
#include "TRNhtmlEscape.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring TRNhtmlEscapeString(const COLstring& UnescapedString){
   COLstring EscapedString;
   TRNhtmlEscape(UnescapedString, EscapedString);
   return EscapedString;
}
