// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNideCompileSet
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Tuesday 09 May 2023 - 01:17PM
// ---------------------------------------------------------------------------

#include <TRN/TRNideCompileSet.h>
#include <TRN/TRNcreateJobQueue.h>
#include <TRN/TRNjobQueue.h>
#include <TRN/TRNannotationJob.h>

#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

// /ide/compile/set
void TRNideCompileSet(const COLwebRequest& Request, SCKloop* pLoop, COLhashmap<COLstring, TRNjobQueue*>* pTranslatorJobQueues){
   COL_FUNCTION(TRNideCompileSet);
   COL_VAR(Request.Data.json(1));
   const COLstring ComponentId = COLvarRequiredString(Request.Data, "component");
   TRNcreateJobQueue(ComponentId, pLoop, pTranslatorJobQueues);
   (*pTranslatorJobQueues)[ComponentId]->pJob->m_CompileNeeded = true;
   return COLrespondSuccess(Request.Address);
}
