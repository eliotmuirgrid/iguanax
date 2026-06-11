// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNideAnnotationWindow
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 25 April 2023 - 03:24PM
// ---------------------------------------------------------------------------

#include <TRN/TRNideAnnotationWindow.h>
#include <TRN/TRNjobQueue.h>
#include <TRN/TRNjobRunner.h>
#include <TRN/TRNcreateJobQueue.h>

#include <PRM/PRMcallbacks.h>

#include <ROL/ROLid.h>

#include <SCK/SCKloop.h>

#include <COL/COLclosure.h>
#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

// /ide/annotation/window
void TRNideAnnotationWindow(const COLwebRequest& Request, SCKloop* pLoop, COLhashmap<COLstring, TRNjobQueue*>* pTranslatorJobQueues){
   COL_FUNCTION(TRNideAnnotationWindow);
   const COLvar& Input = Request.Data;
   COLstring ComponentId = COLvarRequiredString(Input, "component");
   bool IsDev = COLvarRequiredBool(Input, "development");
   ROLid Id = IsDev ? ROLid::EDIT_SCRIPT : ROLid::VIEW_SCRIPT;
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, Id, &Error)){
      return COLrespondError(Request.Address, PRMsetError(Id));
   }
   TRNcreateJobQueue(ComponentId, pLoop, pTranslatorJobQueues); // creates a new job queue if needed
   COL_TRC("Adding annotation window task to queue");
   TRNaddJob(COLnewClosure2(&TRNannotationWindowTask, Input, Request.Address), (*pTranslatorJobQueues)[ComponentId], pLoop);
}