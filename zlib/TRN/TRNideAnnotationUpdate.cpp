// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNideAnnotationUpdate
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 25 April 2023 - 03:24PM
// ---------------------------------------------------------------------------

#include <TRN/TRNideAnnotationUpdate.h>
#include <TRN/TRNjobQueue.h>
#include <TRN/TRNjobRunner.h>
#include <TRN/TRNcreateJobQueue.h>

#include <PRM/PRMcallbacks.h>

#include <ROL/ROLid.h>

#include <SCK/SCKloop.h>

#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

// /ide/annotation/update
void TRNideAnnotationUpdate(const COLwebRequest& Request, SCKloop* pLoop, COLhashmap<COLstring, TRNjobQueue*>* pTranslatorJobQueues){
   COL_FUNCTION(TRNideAnnotationUpdate);
   const COLvar& Input = Request.Data;
   COL_VAR(Input.json(1));
   COLstring ComponentId = COLvarRequiredString(Input, "component");
   bool IsDev = COLvarRequiredBool(Input, "development");
   ROLid Id = IsDev ? ROLid::EDIT_SCRIPT : ROLid::VIEW_SCRIPT;
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, Id, &Error)){
      return COLrespondError(Request.Address, PRMsetError(Id));
   }
   TRNcreateJobQueue(ComponentId, pLoop, pTranslatorJobQueues);
   COL_TRC("Adding annotation update task to queue");
   TRNaddJob(COLnewClosure2(&TRNannotationUpdateTask, Input, Request.Address), (*pTranslatorJobQueues)[ComponentId], pLoop);
}