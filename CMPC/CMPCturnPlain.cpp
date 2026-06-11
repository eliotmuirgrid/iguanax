// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCturnPlain
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Friday 25 October 2024 - 12:38PM
// ---------------------------------------------------------------------------
#include <BLUA/BLUAmanager.h>
#include <CFG/CFGconfig.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCturnPlain.h>
#include <CMPC/CMPCutils.h>
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GINS/GINSinitialize.h>
#include <GIT/GITadd.h>
#include <GIT/GITremoveTracking.h>
#include <GIT/GITutils.h>
#include <PRM/PRMcallbacks.h>
#include <PRM/PRMpermissionUtils.h>
#include <PRM/PRMroleCheck.h>
#include <SCK/SCKworker.h>
COL_LOG_MODULE;

static void CMPCturnPlainCallback(COLvar Result, CFGmap* pMap, COLaddress Address) {
   COL_FUNCTION("CMPCturnPlainCallback");
   COL_VAR(Result.json(1));
   if(!Result["success"]) { return COLrespondError(Address, Result["error"]); }
   const COLstring& Component = Result["component"];
   CFGconfig*       pConfig   = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Address, Component + " not found"); }  // should never happen
   pConfig->CoreConfig.Commit = "DEVELOPMENT";
   pConfig->CoreConfig.Repo   = "";
   COLrespondSuccess(Address, Result["warning"]);
}

void CMPCremoveTracking(const COLstring& Component) {
   const COLstring ConfigDir = DIRconfigDir();
   const COLstring Folder    = DIRcomponentDevDir(Component);
   COLstring       Error;
   COL_VAR2(ConfigDir, Folder);
   COLstring RelFile = Folder;
   if(COLstringHasPrefix(Folder, ConfigDir)) RelFile = COLstringCutPrefix(Folder, ConfigDir);
   COL_VAR(RelFile);
   bool Success = GITremoveTracking(ConfigDir, RelFile, &Error);
   COL_VAR2(Success, Error);
   if(!Success) {
      COL_TRC("Error removing tracking for " << Component << ": " << Error);
      return;
   }
   GITadd(ConfigDir, RelFile, true, &Error);
}

static void CMPCturnPlainWorker(const COLvar& In, COLvar* pOut) {
   COL_FUNCTION("CMPCturnPlainWorker");
   const COLstring Component  = In["component"];
   const COLstring DevDir     = DIRcomponentDevDir(Component);
   const COLstring RunDir     = DIRcomponentRunDir(Component);
   const COLstring ConfigFile = DIRcomponentConfig(Component);
   try {
      if(FILfileExists(RunDir)) {
         COL_TRC("Removing run dir: " << RunDir);
         FILremoveFullDirNew(RunDir);
      }
      COLvar    Json;
      COLstring Data;
      FILreadFile(ConfigFile, &Data);
      Json.parse(Data);
      Json["run_commit"] = "";
      Json["run_branch"] = "";
      Json["run_repo"]   = "";
      FILwriteFile(ConfigFile, Json.json(1));
      GITcleanupAllGitDirs(DevDir);
      (*pOut)["success"]   = true;
      (*pOut)["component"] = Component;
      CMPCremoveTracking(Component);
      GINSremoveComponent(Component);
   } catch(const COLerror& E) {
      COL_TRC("Error: " << E.description());
      CMPCsetErrorVar(pOut, E.description());
   }
}

// component/turn_plain
void CMPCturnPlain(const COLwebRequest& Request, BLUAinstanceManager* pBluaManager, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(CMPCturnPlain);
   const COLstring ComponentId = COLvarRequiredString(Request.Data, "component");
   CFGconfig*      pConfig     = CFGmapNode(pMap, ComponentId);
   if(!pConfig) { return COLrespondError(Request.Address, ComponentId); }
   COLstring Error;
   if(BLUAinstanceRunning(*pBluaManager, ComponentId)) {
      return COLrespondError(Request.Address, "You must stop the component before making modifications.");
   }
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pConfig->CoreConfig.Guid, ROLid::EDIT_UPSTREAM,
                          &Error)) {
      COL_TRC("You don't have permission");
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_UPSTREAM));
   }
   COLvar Data;
   Data["component"] = ComponentId;
   SCKinvokeWorker(pLoop, CMPCturnPlainWorker, Data, COLnewClosure1(CMPCturnPlainCallback, pMap, Request.Address));
}
