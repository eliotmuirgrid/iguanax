// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBAlogClear
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday 09 May 2023 - 04:26PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SDB/SDBcapi.h>
#include <SDB/SDBpurgeManual.h>
#include <SDB/SDBserviceLog.h>
#include <SDB/SDBsystem.h>
#include <SDBA/SDBAlogClear.h>
COL_LOG_MODULE;

static void SDBAlogCleared(COLstring Time, COLstring AltTime, COLaddress Address) {
   COL_FUNCTION(SDBAlogCleared);
   COLvar Result;
   Result["before_time"] = Time;
   if (!AltTime.is_null()) {
      // before_time for the Backup log folder
      Result["alt_before_time"] = AltTime;
   }
   COL_VAR(Result);
   COLrespondSuccess(Address, Result);
}

// log/component/clear
void SDBAlogClear(const COLwebRequest& Request, SDBsystem* pSystem){
   COL_FUNCTION(SDBAlogClear);
   COLstring ComponentId = COLvarGetString(Request.Data, "component", SDB_SERVICE_COMPONENT_ID);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::DELETE_LOGS, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::DELETE_LOGS));
   }
   if (ComponentId.is_null()) { ComponentId = SDB_SERVICE_COMPONENT_ID; }
   if (ComponentId != SDB_SERVICE_COMPONENT_ID && pSystem->ConfigMap.ConfigMap.count(ComponentId) == 0) {
      return COLrespondError(Request.Address, ComponentId + " does not exist");
   }
   COL_TRC("Clearing logs for " << ComponentId);
   SDBpurgeLog(pSystem, ComponentId, COLnewClosure2(&SDBAlogCleared, Request.Address));
   SDBIlogService("User '" + Request.User + "' manually started log purging for component '" + ComponentId + "'.", "#purge");
}