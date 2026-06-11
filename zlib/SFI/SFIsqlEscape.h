#ifndef __SFI_SQL_ESCAPE_H__
#define __SFI_SQL_ESCAPE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIsqlEscape
//
// Description:
//
// SFIsqlEscape class.
//
// Author: Eliot Muir
// Date:   Tuesday, August 31st, 2010 @ 03:31:47 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

// Only escape if we need to.  Useful for languages like Lua - better if we can
// avoid the overhead of re-creating the Lua string.
int SFIsqlEscapedSize(const COLstring& Value);

void SFIsqlEscapeString(const COLstring& Orig, COLstring* pEscaped, int NewSize);

#endif // end of defensive include
