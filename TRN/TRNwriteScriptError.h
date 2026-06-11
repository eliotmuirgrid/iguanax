#ifndef __TRN_WRITE_SCRIPT_ERROR_H__
#define __TRN_WRITE_SCRIPT_ERROR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNwriteScriptError
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

// LineNumber is to be used as a backup in we are unable to extract the line number
// from the error message.  -1 means we don't know.
void TRNwriteScriptError(COLvar* pErrorTank, const COLstring& ErrorMessage, const TRNannotationState& EventHandler, bool CompileError);

#endif // end of defensive include
