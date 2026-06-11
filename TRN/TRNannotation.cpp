//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNannotation
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, November 16th, 2010 @ 02:58:17 PM
//---------------------------------------------------------------------------
#include <TRN/TRNannotation.h>
#include <TRN/TRNannotationJob.h>
#include <TRN/TRNcallMain.h>
#include <TRN/TRNconnectionCheck.h>
#include <TRN/TRNfile.h>

#include <TRN/TRNhook.h>
#include <TRN/TRNfileFind.h>
#include <TRN/TRNwriteFileInfo.h>
#include <TRN/TRNwriteScriptError.h>
#include <TRN/TRNtargetSet.h>

#include <TRN/TRNargumentRequestParse.h>
#include <TRN/TRNargumentResponse.h>

#include <BRO/BROargumentPopulate.h>

#include <FIL/FILpathUtils.h>
#include <LND/LNDdebug.h>
#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


// /ide/annotation
void TRNannotation(const COLvar& Params, TRNannotationJob* pTranslator, COLvar* pResult) {
   COL_FUNCTION(TRNannotation);
   COLvar& Result = *pResult;
   COL_VAR(Params.json(1));
   COLstring FileName = COLvarRequiredString(Params, "filename");
   bool Development   = COLvarRequiredBool(Params, "development");

   COL_VAR(FileName);
   if (FILpathExtLast(FileName, true) != ".lua") { 
      COL_TRC("We do this so that we can see the annotations change if we edit sample data or EDI grammars etc.");
      FileName = "main.lua";
   } // stops iguana crashing if executed script isn't lua
   if (Params.exists("disabled")) {
      COL_TRC("The parameter 'disabled' exists, annotations disabled");
      Result = "Annotation disabled";
      return;
   }
   COL_TRC("Check for full argument requests");
   if (Params.exists("full_argument_requests")) {
      COL_TRC("The parameter 'full_argument_requests' exists");
      TRNargumentRequestParse(Params["full_argument_requests"], &pTranslator->m_AnnotationState.m_FullArgumentRequests);
   }
   COL_TRC("Run script");
   COL_VAR(pTranslator->m_AnnotationState.m_LuaFileList.find("main.lua")->second->m_Source);
   COLstring ScriptError;

   bool ScriptSuccess = false;

   if (pTranslator->L != 0) {
      COL_TRC("We have an interpreter so run the program.");
      LNDregisterNodeCallback(pTranslator->L, &pTranslator->m_AnnotationState);
      TRNconnectionCheckTimeRegister(pTranslator->L);
      TRNhookAnnotationSet(pTranslator->L);
      TRNresetAnnotationState(&pTranslator->m_AnnotationState);
      TRNfunctionBlock(Params, &pTranslator->m_AnnotationState, FileName);
      ScriptSuccess = TRNcallFunctionWithSampleData(pTranslator->L, pTranslator->m_AnnotationState.m_SampleFunction, pTranslator->m_AnnotationState.m_SampleData, &ScriptError);
      //TODO should unit test this?
      if (ScriptSuccess && TRNprogramCheckUnreachableBlocks(pTranslator->m_AnnotationState.m_LuaFileList)) {
         COL_TRC("Re-run program since we have unreachable annotation blocks.");
         TRNresetAnnotationState(&pTranslator->m_AnnotationState);
         ScriptSuccess = TRNcallFunctionWithSampleData(pTranslator->L, pTranslator->m_AnnotationState.m_SampleFunction, pTranslator->m_AnnotationState.m_SampleData, &ScriptError);
      }
      TRNhookAnnotationUnset(pTranslator->L);
   }
   if (!ScriptSuccess) {
      TRNwriteScriptError(&Result, ScriptError, pTranslator->m_AnnotationState, false);
      COL_TRC("Code has error = " << Result);
   }
   // generate report
   TRNfile* pFile   = TRNfileFindOrAdd(pTranslator->m_AnnotationState.m_LuaFileList, pTranslator->m_AnnotationState.m_ProjectPath, FileName);
   TRNcalcMaxLineLength(pFile);
   TRNwriteFileInfo(pTranslator->m_AnnotationState, pFile, &Result);  // this generates BlockData
   TRNargumentWriteResponses(pTranslator->m_AnnotationState.m_FullArgumentRequests, &Result);
   #ifdef BRO_ENABLED
   BROwriteResponses(pTranslator->m_AnnotationState.m_FullArgumentRequests, &Result);  // CONNECTOR BRO code.
   #endif
   COL_VAR(Result.json(1));
}
