// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNtargetSet
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 06 June 2023 - 03:50PM
// ---------------------------------------------------------------------------

#include <TRN/TRNtargetSet.h>
#include <TRN/TRNfileFind.h>
#include <TRN/TRNfile.h>
#include <TRN/TRNfunction.h>
#include <TRN/TRNannotationState.h>


#include <LUACX/LUAXloops.h>

#include <COL/COLvarUtils.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// TODO - bad name for this function.
void TRNfunctionOnFunctionCalled(TRNfunction* pFunction, const COLstring& FunctionAddress){
   //COL_FUNCTION(TRNfunctionOnFunctionCalled);
   if (!pFunction->m_TargetCalleeAddress.is_null() && pFunction->m_TargetCalleeAddress == FunctionAddress){
      COL_TRC("Found callee");
      COL_VAR(*pFunction);
      LUAXscopeSetChildTargetsToCurrentIterations(&pFunction->m_Annotations);
      COL_VAR(pFunction->m_Annotations.m_CurrentIteration);
      pFunction->m_Annotations.m_TargetIteration = COL_MAX(1, pFunction->m_Annotations.m_CurrentIteration);
      COL_VAR(*pFunction);
   }
}

static void TRNpopulateTargetCalls(const COLvar& Input, LUAXscope& Scope){
   COL_FUNCTION(TRNpopulateTargetCalls);
   COL_VAR(Input.json(1));
   const COLvar& Array = Input["c"];
   int TargetIteration = Input.exists("i") ? Input["i"].asInt() : 1;
   COL_VAR(TargetIteration);
   Scope.m_TargetIteration = TargetIteration;
   int CountOfChildScope = Scope.m_Scopes.size();
   COL_VAR(CountOfChildScope);
   for (int i = 0; i < CountOfChildScope; i++){
      COL_VAR(i);
      TRNpopulateTargetCalls(Array[i], Scope.m_Scopes[i]);
   }
}

void TRNtargetSet(COLhashmap<COLstring, TRNfile*>& LuaFileList, const COLstring& ProjectPath, const COLstring& SourceFile, int StartLine, const COLvar& P){
   COL_FUNCTION(TRNtargetSet);
   COL_VAR2(SourceFile, StartLine);
   COL_VAR(LuaFileList);
   TRNfile* pFile = TRNfileFindOrAdd(LuaFileList, ProjectPath , SourceFile);
   COL_VAR(*pFile);
   TRNfunction* pFunction = TRNfindOrCreateFunction(pFile, StartLine);
   if (P.exists("iteration_tree")){
      COL_VAR(P["iteration_tree"]);
      TRNpopulateTargetCalls(P["iteration_tree"], pFunction->m_Annotations);   
   } else if (P.exists("target_callee")){
      pFunction->m_TargetCalleeAddress = P["target_callee"];
      LUAXscopeSetChildTargetsToUnknown(&pFunction->m_Annotations);
      pFunction->m_Annotations.m_TargetIteration = TRN_UNKNOWN_TARGET_ITERATION;
   }
   COL_VAR(pFunction->m_Annotations);
   COL_VAR(LuaFileList);
}

TRNfunction* TRNgetFunction(TRNannotationState* pState, const COLstring& FileName, int FunctionStartLine){
   COL_FUNCTION(TRNgetFunction);
   TRNfile* pFile = TRNfileFindOrAdd(pState->m_LuaFileList, pState->m_ProjectPath, FileName);
   TRNfunction* pFunction = TRNfindOrCreateFunction(pFile, FunctionStartLine);
   COL_VAR(*pFunction);
   return pFunction;
}

void TRNfunctionBlock(const COLvar& P, TRNannotationState* pState, const COLstring& FileName) {
   COL_FUNCTION(TRNfunctionBlock);
   COL_VAR(pState->m_LuaFileList);
   if (P.exists("function_block")) {
      COL_VAR(P["function_block"]);
      COLvar F = P["function_block"];
      TRNfunction* pFunction = TRNgetFunction(pState, FileName, COLvarRequiredInt(F, "start_line"));
      if (F.exists("target_callee")){
         COL_TRC("Setting target_callee");
         pFunction->m_TargetCalleeAddress = F["target_callee"];
         LUAXscopeSetChildTargetsToUnknown(&pFunction->m_Annotations);
         pFunction->m_Annotations.m_TargetIteration = TRN_UNKNOWN_TARGET_ITERATION;
      } else {
         COL_TRC("Setting target_callee");
         pFunction->m_Annotations.m_TargetIteration = COLvarRequiredInt(F, "iteration");
      }
   } else {
      COL_TRC("function_block parameter does not exist");
   }
   COL_VAR(pState->m_LuaFileList);
}
