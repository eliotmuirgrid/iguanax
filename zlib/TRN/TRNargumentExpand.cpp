//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNargumentExpand
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
#include <TRN/TRNargumentRequestParse.h>
#include <TRN/TRNargumentResponse.h>
#include <TRN/TRNargumentInCache.h>
#include <TRN/TRNhook.h>

#include <LND/LNDdebug.h>
#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool TRNargumentHaveCachedData(const COLarray<TRNargRequest>& FullArgumentRequests, const COLstring& Id){
   COL_FUNCTION(TRNargumentHaveCachedData);
   for (int i = 0; i < FullArgumentRequests.size(); i++){
      if (FullArgumentRequests[i].m_Id == Id) { 
         COL_TRC("Already have data for this argument - no need to run script.");
         return true;
      }
   }
   return false;
}

static void TRNargumentRunScript(const COLstring& Id, const COLstring& FileName, TRNannotationJob* pTranslator, COLvar* pResult){
   COL_FUNCTION(TRNargumentRunScript);
   COLvar Args;
   COLvar Item;
   Item["id"] = Id;
   Args.push_back(Item);

   TRNargumentRequestParse(Args, &pTranslator->m_AnnotationState.m_FullArgumentRequests);
   COLstring ScriptError;
   pTranslator->m_AnnotationState.m_CurrentFile = FileName;
   COL_TRC("Run script");
   TRNconnectionCheckTimeRegister(pTranslator->L);
   TRNhookAnnotationSet(pTranslator->L);
   bool Success = TRNcallFunctionWithSampleData(pTranslator->L, pTranslator->m_AnnotationState.m_SampleFunction, pTranslator->m_AnnotationState.m_SampleData, &ScriptError);
   if (!Success) {
      COL_TRC("Ran into script error.");
      (*pResult)["ErrorMessage"] = ScriptError;
      // No early return. There could be some traceable tables before the script error, if so we have the data for those. 
   }
   TRNhookAnnotationUnset(pTranslator->L);
}

void TRNargumentExpand(const COLvar& Input, TRNannotationJob* pTranslator, COLvar* pResult) {
   COL_FUNCTION(TRNargumentExpand);
   COLvar& Result = *pResult;
   COL_VAR(Input);
   // Could already have the argument data from a previous run of the script, or we may need to do a run to get data.
   // If we have the data already, it would be in the annotation state's m_FullArgumentRequests member. Check by looking for a matching id.
   COLstring  Id = COLvarRequiredString(Input, "id");
   bool FoundArg = TRNargumentHaveCachedData(pTranslator->m_AnnotationState.m_FullArgumentRequests, Id);
   if (!FoundArg){
      COLstring FileName = COLvarRequiredString(Input, "filename");
      COL_TRC("Didn't find data for window, need to re-run script.");
      TRNargumentRunScript(Id, FileName, pTranslator, pResult);
   }
   // Now can get get more of an argument already being displayed (i.e. a tree
   // branch is expanded, or the user has scrolled and we need more chunk(s) of text).
   TRNargRequest ArgRequest;
   TRNargumentPopulate(Input, &ArgRequest);
   COLhashmapPlace ArgumentPlace = TRNargumentInCache(pTranslator->m_AnnotationState.m_FullArgumentCache, ArgRequest);
   if (ArgumentPlace){
      COL_TRC("Found argument in cache");
      TRNarg& Arg = pTranslator->m_AnnotationState.m_FullArgumentCache[ArgumentPlace];
      TRNargumentPopulateResponse(Arg, ArgRequest);
      Result = ArgRequest.m_VarResult;
      COL_VAR(Result);
   } else { 
      COL_TRC("Argument not found in cache.");
      (*pResult)["ErrorMessage"] = "Argument not found in cache.";
   }
}
