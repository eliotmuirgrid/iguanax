#ifndef __TRN_FUNCTION_H__
#define __TRN_FUNCTION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNfunction
//
// Description:
//
// TRNfunction - data for a lua function
//
// Author: Eliot Muir
// Date:   Friday, November 19th, 2010 @ 10:34:19 AM
//---------------------------------------------------------------------------

#include <TRN/TRNscope.h>

#include <LUACX/LUAXloops.h>

class TRNfile;
class TRNfunction{
public:
   TRNfunction(TRNfile* pFile, int Start);
   ~TRNfunction();

   LUAXscope  m_Annotations;
   TRNfile* m_pFile;
   int m_Start, m_End;
   int m_MaxLine;   
   COLstring m_TargetCallName;
   COLstring m_TargetCalleeAddress;
   LUAXscope* m_pCurrentScope;

private:
   TRNfunction(const TRNfunction& Orig); // not allowed
   TRNfunction& operator=(const TRNfunction& Orig); // not allowed
};

COLostream& operator<<(COLostream& Stream, const TRNfunction& Function);
// Load the original source code for this function and parse it into
// a tree of scopes, based on the locations of loops in the function body.
// This should be called when the target call for this function is reached.
void TRNfunctionLoadSource(TRNfunction* pFunction);
// Returns the TRNline* associated with the specified line number, if the
// line is in the scope request.  Will adjust iteration numbers for this
// and its child scopes accordingly.
TRNline* TRNfunctionOnLine(TRNfunction* pFunction,int Line);

// Executed when this function calls another function (only if this function is on its target call).
void TRNfunctionOnFunctionCalled(TRNfunction* pFunction, const COLstring& FunctionAddress);
void TRNfunctionOnCurrentLoopEnd(TRNfunction* pFunction);
class TRNfunctionCall{
public:
   TRNfunctionCall() : m_pFunction(0), m_Call(0) {}

   TRNfunction* m_pFunction;
   int          m_Call;
   COLstring    m_Name; // to display the function name as it was called
};

bool TRNfunctionCallIsTargetCall(const TRNfunctionCall& Call);

COLostream& operator<<(COLostream& Stream, const TRNfunctionCall& Call);
#endif // end of defensive include