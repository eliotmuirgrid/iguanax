//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNfunction
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday, November 19th, 2010 @ 10:34:19 AM
//---------------------------------------------------------------------------
#include <TRN/TRNfunction.h>
#include <TRN/TRNfile.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

TRNfunction::TRNfunction(TRNfile* pFile, int Start) : m_Start(Start), m_End(Start), m_MaxLine(0), m_pFile(pFile), m_pCurrentScope(0) {
   COL_METHOD(TRNfunction::TRNfunction);
}

TRNfunction::~TRNfunction(){
   COL_METHOD(TRNfunction::~TRNfunction);
}

TRNline* TRNfunctionOnLine(TRNfunction* pFunction, int Line){
   COL_FUNCTION(TRNfunctionOnLine);
   if (Line < pFunction->m_Start || Line > pFunction->m_End){
      COL_TRC("Out of range of function - shouldn't be possible");
      return 0;
   }
   // TODO this seems odd at this stage?
   if (!pFunction->m_Annotations.m_Initialized){
      COL_TRC("Loading source");
      TRNfunctionLoadSource(pFunction);
   }
   return LUAXscopeOnLine(&pFunction->m_Annotations, Line, true, true, &pFunction->m_pCurrentScope);
}

// TODO - why do we need the if statement?
void TRNfunctionOnCurrentLoopEnd(TRNfunction* pFunction){
   COL_FUNCTION(TRNfunctionOnCurrentLoopEnd);
   if (pFunction->m_pCurrentScope){
      pFunction->m_pCurrentScope->m_IterationOver = true;
   }
}

static int TRNgetPosition(const COLstring& LuaCode, int LineNumber){
   int CurrentLineNumber = 1;
   COLindex CurrentLineBegin = 0;
   while (CurrentLineNumber < LineNumber){
      CurrentLineBegin = LuaCode.find('\n', CurrentLineBegin);
      if (CurrentLineBegin == npos){
         return LuaCode.size();
      }
      CurrentLineNumber++;
      CurrentLineBegin++;
   }
   return CurrentLineBegin;
}

void TRNfunctionLoadSource(TRNfunction* pFunction){
   COL_FUNCTION(TRNfunctionLoadSource);
   const COLstring& FileSource = pFunction->m_pFile->m_Source;

   int FunctionBegin = TRNgetPosition(FileSource, pFunction->m_Start);
   int FunctionEnd = TRNgetPosition(FileSource, pFunction->m_End+1);

   LUAXfindLoops(FileSource, FunctionBegin, FunctionEnd, pFunction->m_Annotations);
   COL_TRC("Calling init() for TRNfunction " << ((void*)pFunction));
   LUAXscopeInit(&pFunction->m_Annotations);
}

bool TRNfunctionCallIsTargetCall(const TRNfunctionCall& Call) {
   COL_FUNCTION(TRNfunctionCallIsTargetCall); 
   COL_VAR(Call.m_pFunction);
   if (!Call.m_pFunction) return false;

   int ScopeTargetIteration = LUAXscopeTargetIteration(Call.m_pFunction->m_Annotations);
   COL_VAR2(Call.m_Call, ScopeTargetIteration);

   bool Result = Call.m_pFunction && Call.m_Call == ScopeTargetIteration; 
   COL_VAR(Result);
   return Result;
}

COLostream& operator<<(COLostream& Stream, const TRNfunctionCall& Call){
   Stream << "Name = " << Call.m_Name << ", iteration = " << Call.m_Call << ", file=" << Call.m_pFunction->m_pFile->m_Label;
   return Stream;
}

COLostream& operator<<(COLostream& Stream, const TRNfunction& Function){
   Stream << "Function range(" << Function.m_Start << "," << Function.m_End << ")"; 
   if (!Function.m_TargetCalleeAddress.is_null()){
      Stream << ", targetcallee = " << Function.m_TargetCalleeAddress;
   }
   if (!Function.m_TargetCallName.is_null()){
      Stream << ", targetcallname = " << Function.m_TargetCallName;    
   }
   Stream << indent << newline << Function.m_Annotations << unindent;
   return Stream; 
}