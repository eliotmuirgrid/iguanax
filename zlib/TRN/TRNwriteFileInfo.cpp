//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNwriteFileInfo
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, November 22nd, 2010 @ 10:58:18 AM
//
//---------------------------------------------------------------------------
#include "TRNfile.h"
#include "TRNfunction.h"
#include "TRNannotationState.h"

#include <COL/COLsinkString.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void LUAXscopeVar(const TRNannotationState& State, const TRNfile* pFile, const LUAXscope& Scope, COLvar* pScopeTank){
   COL_FUNCTION(LUAXscopeVar);
   COLstring AnnBlock;
   COLostream AnnBlockStream(AnnBlock);
   AnnBlockStream.setNewLine("<br>");
   LUAXscopeOutputAnnotations(Scope,AnnBlockStream);

   (*pScopeTank)["d"] = AnnBlock;
   (*pScopeTank)["l"] = Scope.m_CurrentIteration;
   // We have the special case where the target iteration may be set to -1 because of searching for a error where the target callee is set to
   // the imaginary target 'last'
   (*pScopeTank)["p"] = Scope.m_TargetIteration == TRN_UNKNOWN_TARGET_ITERATION ? 1 : Scope.m_TargetIteration; 
   (*pScopeTank)["s"] = Scope.m_Start;
   (*pScopeTank)["e"] = Scope.m_End;
   COLvar Children;
   Children.setArrayType();
   for (int i = 0, n = Scope.m_Scopes.size(); i < n; i++){
      COLvar ChildScope;
      LUAXscopeVar(State, pFile, Scope.m_Scopes[i], &ChildScope);
      Children.push_back(ChildScope);
   }
   (*pScopeTank)["c"].swap(&Children);
}

void TRNblockVar(const TRNannotationState& State, const TRNfunction* pFunc, COLvar* pResult) {
   COL_FUNCTION(TRNblockVar);
   // Tooltip for thumbnail view
   COLvar& Result = *pResult;
   //if (pFunc->m_Annotations.m_CurrentIteration > 1) {
   //   COL_TRC("More that one call"); 
      //Result["t"] = pFunc->m_TargetCallName + " " + COLint32ToString(pFunc->m_Annotations.m_CurrentIteration) + " iterations"; 
   //} else { 
   //   COL_TRC("One call only");
      //Result["t"] = pFunc->m_TargetCallName; 
   //}
   //COL_VAR(Result["t"]);
   Result["c"] = pFunc->m_MaxLine;
   COLvar Scope;
   LUAXscopeVar(State, pFunc->m_pFile,  pFunc->m_Annotations, &Scope);
   Result["scope"].swap(&Scope);
   COL_VAR(Result["scope"].json(1));
}

void TRNwriteFileInfo(const TRNannotationState& State, TRNfile* pFile, COLvar* pResult){   
   COL_FUNCTION(TRNwriteFileInfo);
   COLvar BlockData;
   BlockData.setArrayType();
   COL_VAR(pFile->m_FunctionList.size());
   for (auto i=pFile->m_FunctionList.begin(); i != pFile->m_FunctionList.end(); i++){
      COLvar OneFunc;
      COL_VAR2(i->first, i->second);
      const TRNfunction* pFunc = i->second;
      COL_VAR(pFunc->m_Annotations.m_CurrentIteration);
      if (pFunc->m_Annotations.m_CurrentIteration > 0){
         COL_VAR2(pFunc->m_TargetCallName,pFunc->m_Annotations.m_CurrentIteration);
         TRNblockVar(State, pFunc, &OneFunc);
         BlockData.push_back(OneFunc);
      }
   }
   COL_VAR(BlockData);
   (*pResult)["BlockData"].swap(&BlockData);
}
