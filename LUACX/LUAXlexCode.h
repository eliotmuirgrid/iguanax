#ifndef __LUAX_LEX_CODE_H__
#define __LUAX_LEX_CODE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAXlexCode
//
// Description:
//
// Code to parse Lua for auto-completion?
//
// Author: Kevin Senn
// Date:   Tuesday, May 17th, 2011 @ 03:11:12 PM
//---------------------------------------------------------------------------

#include "LUAXtoken.h"
#include <COL/COLauto.h>
#include "lua.h"



// Returns a pointer to the last token.
//
// IncompleteKey is valid iff the "StopPosition" occurs in a (possibly) incomplete key.
// If this is the case, it will be the value of the incomplete key.
// Also if this is the case, IncompleteKeyIsStringLiteral will be set to true
// if the incomplete key is a string literal (e.g. 'foo rather than foo), or
// false otherwise.
//
// If Reverse is true, the token list returned will be in reverse order, starting from StopPosition.
COLauto<LUAXtoken> LUAXlexCode(
   luax_State* L,
   const COLstring& Code,
   const char* pChunkName,
   size_t StopPosition,
   bool Reverse,
   COLstring& IncompleteKey, // output
   bool& IncompleteKeyIsStringLiteral // output
);

#endif // end of defensive include
