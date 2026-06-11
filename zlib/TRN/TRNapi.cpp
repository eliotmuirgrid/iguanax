// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNapi
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday 24 April 2023 - 11:13AM
// ---------------------------------------------------------------------------

#include <TRN/TRNapi.h>
#include <TRN/TRNideDirectories.h>
#include <TRN/TRNideFileDownload.h>
#include <TRN/TRNideFileLoad.h>
#include <TRN/TRNideFilePdfView.h>
#include <TRN/TRNideFileSave.h>
#include <TRN/TRNideIntellisense.h>
#include <TRN/TRNideAnnotation.h>
#include <TRN/TRNideAnnotationUpdate.h>
#include <TRN/TRNideAnnotationWindow.h>
#include <TRN/TRNideCompileSet.h>
#include <TRN/TRNideGlobalFunctionList.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TRNapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, COLhashmap<COLstring, TRNjobQueue*>* pTranslatorJobQueues){
   COL_FUNCTION(TRNapi);
   ApiMap.add("/ide/compile/set",      COLnewClosure1(&TRNideCompileSet,       pLoop, pTranslatorJobQueues));
   ApiMap.add("/ide/intellisense",     COLnewClosure1(&TRNideIntellisense,     pLoop));
   ApiMap.add("/ide/annotation",       COLnewClosure1(&TRNideAnnotation,       pLoop, pTranslatorJobQueues));
   ApiMap.add("/ide/annotation/update",COLnewClosure1(&TRNideAnnotationUpdate, pLoop, pTranslatorJobQueues));
   ApiMap.add("/ide/annotation/window",COLnewClosure1(&TRNideAnnotationWindow, pLoop, pTranslatorJobQueues));
   ApiMap.add("/ide/function/list",    COLnewClosure1(&TRNideGlobalFunctionList,pLoop, pTranslatorJobQueues));
}

void TRNworker(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap){
   COL_FUNCTION(TRNworker);
   ApiMap.add("/directories",   &TRNideDirectories);
   ApiMap.add("/file/save",     &TRNideFileSave);
   ApiMap.add("/file/load",     &TRNideFileLoad);
   ApiMap.add("/file/download", &TRNideFileDownload);
   ApiMap.add("/file/pdf_view", &TRNideFilePdfView);
}