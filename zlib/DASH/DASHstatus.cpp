//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DASHstatus.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   19/06/25 2:19 PM
//  ---------------------------------------------------------------------------
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLweb.h>
#include <DASH/DASHstatus.h>
#include <DIR/DIRpath.h>
#include <GIT/GITfetch.h>
#include <GIT/GITstatusChanges.h>
#include <SCK/SCKloop.h>

#include <memory>
COL_LOG_MODULE;

struct DASHstatusTracker {
   explicit DASHstatusTracker(const int Pending) : m_Pending(Pending) {
      m_ResultVar.setMapType();
   }
   int m_Pending = 0;
   COLvar m_ResultVar;
};

static void DASHstatusCallbackMain(const COLstring Component, const COLvar Result,
                                   COLauto<COLclosure2<COLstring, COLvar>> pCallback) {
   COL_FUNCTION(DASHstatusCallbackMain);
   try {
      pCallback->run(Component, Result);
   } catch(const COLerror& E) { COL_ERR("DASHstatusCallbackMain encountered an error: " << E.description()); }
}

static void DASHstatusCallback(const COLstring Component, const COLvar Result,
                               std::shared_ptr<DASHstatusTracker> pTracker, const COLaddress Address) {
   COL_FUNCTION(DASHstatusCallback);
   pTracker->m_Pending--;
   pTracker->m_ResultVar[Component] = Result;
   if(pTracker->m_Pending == 0) { COLrespondSuccess(Address, pTracker->m_ResultVar); }
}

static void DASHstatusWorker(const COLstring User, const COLstring Component, SCKloop* pLoop,
                             COLauto<COLclosure2<COLstring, COLvar>> pCallback) {
   COL_FUNCTION(DASHstatusWorker);
   const COLstring DevDir = DIRcomponentDevDir(Component);
   COLstring       Output, Error;
   COLvar Result;
   if(!GITfetch(User, DevDir, "", &Output, &Error)) {
      COL_TRC("Fetch failed for " << Component << " \n" << Output + "\n" << Error);
      Result["error"] = "Failed to fetch.";
   }
   Output.clear();
   Error.clear();
   if(GITstatusChanges(DevDir, &Output, &Error)) { Result["has_changes"] = !Output.is_null(); }
   pLoop->post(COLnewClosure0(&DASHstatusCallbackMain, Component, Result, pCallback));
}

void DASHstatus(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(DASHstatus);
   auto pTracker = std::make_shared<DASHstatusTracker>(pMap->ConfigMap.size());
   for(const auto& it : pMap->ConfigMap) {
      COLauto<COLclosure2<COLstring, COLvar>> pCallback = COLnewClosure2(DASHstatusCallback, pTracker, Request.Address);
      pLoop->threadPool()->scheduleTask(COLnewClosure0(&DASHstatusWorker, Request.User, it.first, pLoop, pCallback));
   }
}