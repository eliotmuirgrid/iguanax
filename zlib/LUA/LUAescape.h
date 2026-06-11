#ifndef __LUA_ESCAPE_H__
#define __LUA_ESCAPE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAescape
//
// Description:
//
// LUAstringEscape() will escape a lua string.  Will not put quotes around
// the string - that is up to the caller.
//
// Author: Kevin Senn
// Date:   Monday, April 18th, 2011 @ 03:29:20 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

COLstring LUAstringEscape(const char* UnescapedString, int Size);

#endif // end of defensive include
