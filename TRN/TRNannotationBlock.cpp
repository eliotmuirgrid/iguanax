//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNannotationBlock
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, November 16th, 2010 @ 02:58:17 PM
//---------------------------------------------------------------------------
#include <TRN/TRNannotationBlock.h>
#include <TRN/TRNannotationJob.h>
#include <TRN/TRNcallMain.h>
#include <TRN/TRNconnectionCheck.h>
#include <TRN/TRNblockReportVar.h>
#include <TRN/TRNhook.h>
#include <TRN/TRNtargetSet.h>
#include <TRN/TRNfileFind.h>
#include <TRN/TRNwriteScriptError.h>

#include <LND/LNDdebug.h>

#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// /ide/annotation/update
void TRNannotationBlock(const COLvar& Params, TRNannotationJob* pTranslator, COLvar* pResult) {
   COL_FUNCTION(TRNannotationBlock);
   COLvar&   Result = *pResult;
   COLstring ScriptError;
   COL_VAR(Params.json(1));
   COLstring FileName = COLvarRequiredString(Params, "filename");
   bool Development   = COLvarRequiredBool  (Params, "development");
   int StartLine      = COLvarRequiredInt(Params, "start_line");
   pTranslator->m_AnnotationState.m_CurrentFile = FileName;
   COL_VAR2(StartLine, FileName);
   COL_VAR(pTranslator->m_AnnotationState.m_LuaFileList.size()); 
   COL_VAR(pTranslator->m_AnnotationState.m_LuaFileList[pTranslator->m_AnnotationState.m_CurrentFile]);
   TRNconnectionCheckTimeRegister(pTranslator->L);
   TRNhookAnnotationSet(pTranslator->L);
   TRNresetAnnotationState(&pTranslator->m_AnnotationState);
   // Important that TRNtargetSet happens after TRNclearCalls.
   TRNtargetSet(pTranslator->m_AnnotationState.m_LuaFileList, pTranslator->m_AnnotationState.m_ProjectPath, pTranslator->m_AnnotationState.m_CurrentFile, StartLine, Params);
   bool Success = TRNcallFunctionWithSampleData(pTranslator->L, pTranslator->m_AnnotationState.m_SampleFunction, pTranslator->m_AnnotationState.m_SampleData, &ScriptError);
   if (!Success){
      TRNwriteScriptError(&Result, ScriptError, pTranslator->m_AnnotationState, false);
      COL_TRC("Code has error = " << Result);
   }
   COL_VAR(pTranslator->m_AnnotationState.m_LuaFileList);
   
   TRNblockReportVar(pTranslator->m_AnnotationState, &Result["BlockData"], pTranslator->m_AnnotationState.m_LuaFileList, pTranslator->m_AnnotationState.m_CurrentFile, StartLine);
   TRNhookAnnotationUnset(pTranslator->L);
   COL_VAR(pResult->json(1));
}