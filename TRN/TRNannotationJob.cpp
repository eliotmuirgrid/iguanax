//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNannotationJob
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, November 16th, 2010 @ 02:58:17 PM
//---------------------------------------------------------------------------
#include "TRNannotationJob.h"

#include <TRN/TRNconstant.h>
#include <TRN/TRNfile.h>
#include <TRN/TRNfileFind.h>
#include <TRN/TRNsampleData.h>

#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>

#include <TRN/TRNcompile.h>
#include <TRN/TRNindexFunc.h>

#include <CFG/CFGconfigApi.h>

#include <LHLP/LHLPindex.h>

#include <LUA/LUAopen.h>
#include <LUA/LUAtest.h>

#ifdef _WIN32
#include <LUACOM/LUACOMcreateDestroy.h>
#endif
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool TRNcompileAnnotationProgram(TRNannotationJob* pJob, COLstring* pCompileError){
   COL_FUNCTION(TRNcompileAnnotationProgram);
   lua_State** pL = &pJob->L;
   COL_VAR(pL);
   #ifdef _WIN32
   LUACOMdestroyLuacomWrapper(*pL, pJob->m_AnnotationState.m_UsesLuacom);
   #endif
   TRNdestroyLua(pL);
   *pL = LUAopen(LUA_ANNOTATION_MODE);
   LUAsetIdAndRoot(*pL, pJob->m_AnnotationState.m_TranslatorGuid, pJob->m_AnnotationState.m_ProjectPath);
   LUALmarkEnvironmentAsTest(*pL);
   TRNfile* pMainFile = TRNfileFindOrAdd(pJob->m_AnnotationState.m_LuaFileList, pJob->m_AnnotationState.m_ProjectPath, TRN_MAIN_FILE_NAME);

   TRNcompile(pL, &pJob->m_AnnotationState, pCompileError);
   if (*pL == NULL) {
      COL_TRC("We have a compile error");
      return false;
   }
   COL_TRC("Compile OK");
   TRNindexFuncGlobal(*pL, pJob->m_AnnotationState.m_CFunctionList, pJob->m_AnnotationState.m_LFunctionNameMap);
   // Important - this must happen after
   LHLPinit(&pJob->m_AnnotationState.m_HelpMap, *pL);
   LHLPregisterCustomHelp(*pL, &pJob->m_AnnotationState.m_HelpMap, pJob->m_AnnotationState.m_ProjectPath);

   return true;
}

void TRNideBoilerPlate(COLvar P, TRNannotationJob* pJob){
   COL_FUNCTION(TRNideBoilerPlate);
   COLstring ComponentId = COLvarRequiredString(P, "component");
   bool      Development = COLvarRequiredBool  (P, "development");
   COL_TRC(Development);
   COL_TRC("Resolved input params");
   COLstring Path = CFGresolveRepoFile(ComponentId, "", Development);
   COL_TRC("About to set input params to job object");
   pJob->m_AnnotationState.m_ProjectPath    = Path;
   pJob->m_AnnotationState.m_TranslatorGuid = ComponentId;
   if (pJob->m_AnnotationState.m_DevMode != Development){
      pJob->m_CompileNeeded = true;
   }
   pJob->m_AnnotationState.m_DevMode        = Development;
   COL_VAR(pJob->m_AnnotationState.m_DevMode);
   COL_TRC("About to populate sample data");
   TRNsampleDataPopulate(P, ComponentId, &pJob->m_AnnotationState.m_SampleData, &pJob->m_AnnotationState.m_SampleFunction);
}
