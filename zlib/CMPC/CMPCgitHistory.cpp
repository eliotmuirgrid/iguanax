// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCgitHistory
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Tuesday 30 July 2024 - 09:46AM
// ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigApi.h>
#include <CFG/CFGconfigSave.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCcommits.h>
#include <CMPC/CMPCgitHistory.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GITU/GITUfullCommitHistory.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKworker.h>
COL_LOG_MODULE;

void CMPCgitHistoryImpl(const COLvar& In, COLvar* pOut) {
   COL_FUNCTION(CMPCgitHistoryImpl);
   COLvar&   Out = *pOut;
   COLstring Error;
   COLstring ComponentId = In["component"];
   COLstring Path        = In["path"];
   bool Development    = In["development"];
   COLstring RepoDir     = DIRcomponentDevDir(ComponentId);
   COLstring Dir         = FILpathAppend(RepoDir, Path);
   COL_VAR(Dir);
   COLvar& Result = Out["commits"];
   Out["path"]    = Dir;
   GITUfullCommitHistory(Dir, Development, &Result, &Error);
   COL_VAR(Out.json(1));
}

// /component/git_history
void CMPCgitHistory(const COLwebRequest& Request, IWEBconfig* pWebConfig, SCKloop* pLoop) {
   COL_FUNCTION(CMPCgitHistory);
   COL_VAR(Request.Data.json(1));
   COLstring ComponentId, Error;
   const bool Development    = COLvarRequiredBool(Request.Data, "development");
   const COLstring Path      = COLvarGetString(Request.Data, "path", "");
   if (!CFGcomponentId(Request.Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", EDIT_SCRIPT, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(EDIT_SCRIPT));
   }
   COLvar NewData;
   NewData["component"] = ComponentId;
   NewData["development"] = Development;
   NewData["path"]      = Path;
   COL_VAR(NewData.json(1));
   SCKinvokeWorker(pLoop, CMPCgitHistoryImpl, NewData,
                   COLnewClosure1(CMPCcreationComplete, pWebConfig, Request.Address));
}
