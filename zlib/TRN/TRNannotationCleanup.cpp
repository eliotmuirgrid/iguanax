//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNprogram
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday, November 19th, 2010 @ 10:34:00 AM
//---------------------------------------------------------------------------
#include "TRNannotationCleanup.h"
#include "TRNfile.h"
#include "TRNcfunction.h"
#include "TRNfunction.h"
#include "TRNconstant.h"
#include <TRN/TRNannotationState.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLvar.h>


#include <COL/COLlog.h>
COL_LOG_MODULE;

void TRNcleanUpFiles(COLhashmap<COLstring, TRNfile*>& FileList){
   COL_FUNCTION(TRNcleanUpFiles);
   for (auto i=FileList.begin(); i; ++i){
      delete i->second;
   }
}

void TRNfunctionReset(TRNfunction* pFunction){ 
   COL_FUNCTION(TRNfunctionReset);
   pFunction->m_pCurrentScope = 0;
   LUAXscopeReset(&pFunction->m_Annotations);
   pFunction->m_TargetCallName.clear();
   pFunction->m_TargetCalleeAddress.clear();
}

void TRNresetFunctionCalls(TRNfile* pFile){
   COL_FUNCTION(TRNresetFunctionCalls);
   for (auto i = pFile->m_FunctionList.begin(); i; ++i){
      TRNfunctionReset(i->second);
   }
}

void TRNresetFunctions(COLhashmap<COLstring, TRNfile*>& LuaFileList){
   COL_FUNCTION(TRNresetFunctions);
   for(auto i = LuaFileList.cbegin(); i; ++i){
      TRNresetFunctionCalls(i->second);
   }
}

void TRNresetCFunctions(COLhashmap<COLstring, TRNcfunction*>& CFunctionList){
   COL_FUNCTION(TRNresetCFunctions);
   for (auto i = CFunctionList.cbegin(); i; ++i){
      i->second->m_Calls = 0;
   }
}

void TRNresetAnnotationState(TRNannotationState* pState){
   COL_FUNCTION(TRNresetAnnotationState);
   pState->m_Stack.clear();
   pState->m_HashStack.clear();
   pState->m_MostRecentReturnInfo.clear();
   pState->m_CurrentAddress.clear();
   pState->m_ErrorFile.clear();
   pState->m_ErrorLine = -1;
   COL_VAR(pState->m_LuaFileList);
   TRNresetFunctions(pState->m_LuaFileList);
   TRNresetCFunctions(pState->m_CFunctionList);
   COL_VAR(pState->m_LuaFileList);
}
