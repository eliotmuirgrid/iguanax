// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNideTree
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Friday 29 November 2024 - 11:59AM
// ---------------------------------------------------------------------------

#include <TRN/TRNideGlobalFunctionList.h>
#include <TRN/TRNcreateJobQueue.h>
#include <TRN/TRNjobRunner.h>

#include <ROL/ROLid.h>
#include <PRM/PRMcallbacks.h>

#include <COL/COLweb.h>
#include <COL/COLvarUtils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// /ide/function/list
void TRNideGlobalFunctionList(const COLwebRequest& Request, SCKloop* pLoop, COLhashmap<COLstring, TRNjobQueue*>* pTranslatorJobQueues){
   COL_FUNCTION(TRNideTree);
   const COLvar& Input = Request.Data;
   COLstring ComponentId = COLvarRequiredString(Input, "component");
   bool IsDev = COLvarRequiredBool(Input, "development");
   ROLid Id = IsDev ? ROLid::EDIT_SCRIPT : ROLid::VIEW_SCRIPT;
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, Id, &Error)){
      return COLrespondError(Request.Address, PRMsetError(Id));
   }
   TRNcreateJobQueue(ComponentId, pLoop, pTranslatorJobQueues); // creates a new job queue if needed
   COL_TRC("Adding function list task to queue");
   TRNaddJob(COLnewClosure2(&TRNannotationFunctionListTask, Input, Request.Address), (*pTranslatorJobQueues)[ComponentId], pLoop);
}
