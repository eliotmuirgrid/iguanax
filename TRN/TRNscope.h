#ifndef __TRN_SCOPE_H__
#define __TRN_SCOPE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAXscope
//
// Description:
//
// LUAXscope class.  Represent the structure, and annotations, of a function.
//
// Author: Kevin Senn
// Date:   Tuesday, October 4th, 2011 @ 03:45:01 PM
//---------------------------------------------------------------------------

#include <LUACX/LUAXloops.h>

class LUAXscope;

void TRNlineAddReturn(TRNline* pLine, const COLstring& Data);
void TRNlineAddAnnotation(TRNline* pLine, const COLstring& Data);


void LUAXscopeOutputAnnotations(const LUAXscope& Scope, COLostream& Stream);

bool LUAXscopeLineInRange(const LUAXscope& Scope, int Line);

// Gets the TRNline* associated with the specified line number, if the
// line is the currently requested iteration.  Will adjust iteration
// numbers for this and its child scopes accordingly.
// Also returns a pointer to the scope containing the TRNline, via ppScope.
TRNline* LUAXscopeOnLine(LUAXscope* pScope, const int Line, bool CurrentIterationSoFar, bool IsRootFunctionCall, LUAXscope** ppScope);


void LUAXscopeReset(LUAXscope* pScope);

void LUAXscopeSetChildTargetsToUnknown(LUAXscope* pScope);
void LUAXscopeSetChildTargetsToCurrentIterations(LUAXscope* pScope);

int LUAXscopeIncIteration(LUAXscope* pScope);

void LUAXscopeResetIterations(LUAXscope* pScope);

void LUAXscopeInit(LUAXscope* pTrnScope);

#endif // end of defensive include
