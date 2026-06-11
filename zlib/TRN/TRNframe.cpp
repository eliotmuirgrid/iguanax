
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNframe
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Wednesday 03 May 2023 - 01:45PM
// ---------------------------------------------------------------------------
#include <TRN/TRNframe.h>
#include <TRN/TRNscope.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

TRNframe::TRNframe() : m_InternalLevel(0), m_FirstLine(0), m_CurrentLine(0), m_CountOfParam(0), m_ReceivedParams(false), m_pSource(NULL){
   COL_METHOD(TRNframe::TRNframe);
}

void TRNframeInit(TRNframe* pFrame, int InternalLevel, int FirstLine, const COLstring& FunctionName, TRNfunction* pFunction, int CountOfParamGuess, const char* pSource){
   COL_FUNCTION(TRNframeInit);
   pFrame->m_InternalLevel = InternalLevel;
   pFrame->m_FirstLine = FirstLine;
   pFrame->m_CurrentLine = FirstLine;
   pFrame->m_Current.m_pFunction = pFunction;
   pFrame->m_Current.m_Call = 0;
   if (pFunction) {
      pFrame->m_Current.m_Call = LUAXscopeIncIteration(&pFunction->m_Annotations);
   }
   pFrame->m_Current.m_Name = FunctionName;
   pFrame->m_CountOfParam = CountOfParamGuess;
   pFrame->m_pSource = pSource;
}
