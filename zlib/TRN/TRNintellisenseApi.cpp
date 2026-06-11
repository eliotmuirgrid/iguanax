//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNintellisenseApi
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, November 16th, 2010 @ 02:58:17 PM
//---------------------------------------------------------------------------
#include "TRNintellisenseApi.h"
#include "TRNannotationJob.h"
#include "TRNintellisenseMain.h"

#include <TRN/TRNintellisenseContext.h>

#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// Returns the position in LuaCode at which line LineNumber starts.
static int TRNlineToPosition(const COLstring& LuaCode, int LineNumber){
   COL_FUNCTION(TRNlineToPosition);
   int CurrentLineNumber = 1;
   COLindex CurrentLineBegin = 0;
   while (CurrentLineNumber < LineNumber){
      CurrentLineBegin = LuaCode.find('\n', CurrentLineBegin);
      if (CurrentLineBegin == npos){
         COL_ERROR_STREAM_PLAIN("Invalid line number: " << LineNumber, 0);
      }
      CurrentLineNumber++;
      CurrentLineBegin++;
   }
   COL_VAR(CurrentLineBegin);
   return CurrentLineBegin;
}

// Core of intellisense API call.
void TRNintellisenseApi(const COLvar& Params, TRNannotationJob* pTranslator, COLvar* pResult) {
   COL_FUNCTION(TRNintellisenseApi);
   COLvar& Result = *pResult;
   COL_VAR(Params.json(true));
   TRNintellisenseContext P;
   P.CurrentFile           = COLvarRequiredString(Params, "filename");
   P.ParseContext.m_Code   = COLvarRequiredString(Params, "code");
   P.LineNumber            = atoi(COLvarRequiredString(Params, "line_number").c_str());
   P.ParseContext.m_Cursor = atoi(COLvarRequiredString(Params, "cursor_position").c_str());
   P.ParseContext.m_BeginLine = TRNlineToPosition(P.ParseContext.m_Code, P.LineNumber);

   pTranslator->m_AnnotationState.m_IsIntellisense = true;
   if (Params.exists("addition_request_expression")){
      P.ParseContext.FullExpression = Params["addition_request_expression"];
      P.ParseContext.TrimmedExpression = P.ParseContext.FullExpression;
      COL_TRC("Got additional request expression " << P.m_AddRequest);
   }
   COL_VAR(P);
   COL_TRC("Now compiling and running intellisense");
   TRNintellisenseMain(&P, &Result, pTranslator);
   COL_VAR(Result.json(1));
}
