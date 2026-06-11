#ifndef __TRN_INTELLISENSE_CONTEXT_H__
#define __TRN_INTELLISENSE_CONTEXT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNintellisenseContext
//
// Description:
//
// TRNintellisenseContext - class refactored from NWB intellisense code.
//
// Author: Eliot Muir and Kevin Senn
// Date:   Monday, December 20th, 2010 @ 01:25:57 PM
//---------------------------------------------------------------------------

#include <COL/COLlist.h>
#include <COL/COLvar.h>

#include "TRNintellisenseOptions.h"
#include "TRNautoCompleteInfo.h"
#include <LUACX/LUAXgrabOpenExpression.h>

struct lua_State;

class TRNintellisenseContext{
public:
   TRNintellisenseContext() : LineNumber(0), TargetReached(false) { }



   COLstring m_AddRequest;

   int LineNumber;
   COLstring HelpSummary;

   COLstring CurrentFile;
   bool      TargetReached;
   COLstring HelpFile;        // The absolute filepath to the help file when a single help function is resolved

   COLlist<COLstring> Upvalues;
   TRNsortedOptionsList Options;

   LUAXautocompletionContext ParseContext;

   TRNautoCompleteInfo NewCompleteInfo;
};

TRNintellisenseContext* TRNintellisenseContextGet(lua_State* L);
void TRNintellisenseContextSet(lua_State* L, TRNintellisenseContext* pContext);

COLostream& operator<<(COLostream& Stream, const TRNintellisenseContext& Object);

#endif // end of defensive include
