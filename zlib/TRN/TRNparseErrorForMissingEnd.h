#ifndef __TRN_PARSE_ERROR_FOR_MISSING_END_H__
#define __TRN_PARSE_ERROR_FOR_MISSING_END_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNparseErrorForMissingEnd
//
// Description:
//
// TRNscriptError class.
//
// Author: Kevin Senn
// Date:   Friday, November 26th, 2010 @ 12:57:05 PM
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLvar.h>

class TRNannotationState;
struct lua_State;

// Special case to catch error messages for missing 'end' in a function, and to overwrite the 'LineNumber' value
// in the error COLvar from the last line of the file to the line of the function which has the problem.
void TRNparseErrorForMissingEnd(COLvar* pErrorDetails);

#endif // end of defensive include
