#ifndef __TRN_PARSE_ERROR_H__
#define __TRN_PARSE_ERROR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNparseError
//
// Description:
//
// TRNscriptError class.
//
// Author: Kevin Senn
// Date:   Friday, November 26th, 2010 @ 12:57:05 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLvar.h>

class TRNannotationState;
struct lua_State;

void TRNparseError(const COLstring& ErrorMessage, COLstring* pDescription, int* pLine, COLstring* pFile);

#endif // end of defensive include
