//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPLlibraryInitializeAll.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   27/03/25 10:27 AM
//  ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCutils.h>
#include <CMPC/CMPCinitializeLibraries.h>
#include <CMPL/CMPLlibraryInitializeAll.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void CMPLlibraryInitializeAllImpl(COLvar Data, COLstring Username, COLaddress Address,
                                         SCKloop* pLoop, COLclosure0* pCallback) {
   COL_FUNCTION(CMPLlibraryInitializeAllImpl);
   const COLstring ComponentId = COLvarRequiredString(Data, "component");
   const COLstring CommitId    = COLvarGetString(Data, "commit_id", "");
   COLstring Error;
   const bool Success = CMPCinitializeAllLibraries(ComponentId, CommitId, Username, &Error);
   COL_VAR2(Success, Error);
   Success ? COLrespondSuccess(Address) : COLrespondError(Address, Error);
   pLoop->post(pCallback);
}

// /component/library/initialize/all
void CMPLlibraryInitializeAll(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(CMPLlibraryInitializeAll);
   COL_VAR(Request.Data.json(1));
   COLvar Data = Request.Data;
   const COLstring Component   = COLvarRequiredString(Data, "component");
   bool            Development = COLvarRequiredBool(Data, "development");
   if(!Development) { return COLrespondError(Request.Address, "Cannot modify file structure of a run repository."); }
   CFGconfig* pConfig = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Request.Address, Component); }
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pConfig->CoreConfig.Guid, ROLid::EDIT_SCRIPT, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&CMPLlibraryInitializeAllImpl, Data,
                                                    Request.User, Request.Address, pLoop,
                                                    CMPCcallback("/component/library/initialize/all")));
}
