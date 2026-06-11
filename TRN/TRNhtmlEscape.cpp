//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNhtmlEscape
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, December 20th, 2010 @ 02:27:26 PM
//---------------------------------------------------------------------------
#include "TRNhtmlEscape.h"

#include <SFI/SFIhtmlEscape.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TRNhtmlEscape(const COLstring& Source, COLstring& Destination){
   SFIhtmlEscapeNoNewline(Source.c_str(), Source.size(), Destination);
}
