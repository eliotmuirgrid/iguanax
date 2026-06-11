// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCsetCommit
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Wednesday 15 March 2023 - 01:54PM
// ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCrunDir.h>
#include <CMPC/CMPCsetCommit.h>
#include <CMPC/CMPCutils.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GINS/GINSinitialize.h>
#include <GIT/GITbranch.h>
#include <GIT/GITpath.h>
#include <GIT/GITremote.h>
#include <GIT/GITutils.h>
#include <GITU/GITUrepository.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKworker.h>
COL_LOG_MODULE;

static void CMPCsetCommitCallback(COLvar Result, CFGmap* pMap, COLaddress Address) {
   COL_FUNCTION(CMPCsetCommitCallback);
   COL_VAR(Result.json(1));
   const COLstring& Component = Result["guid"];
   if(!Result["success"]) { return COLrespondError(Address, Result["error"]); }
   CFGconfig* pConfig = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Address, Component + " not found"); }
   pConfig->CoreConfig.Commit   = Result["run_commit"];
   pConfig->CoreConfig.Branch   = Result["run_branch"];
   pConfig->CoreConfig.Repo     = Result["run_repo"];
   pConfig->CoreConfig.Template = Result["template"];
   pConfig->CoreConfig.RunComp  = Result["run_comp"];
   COLrespondSuccess(Address, Result["warning"]);
}

static bool CMPCsetCommitJson(const COLstring& ComponentId, const COLstring& CommitId, const COLstring& RunComp,
                              COLvar* pOut) {
   COL_FUNCTION(CMPCsetCommitJson);
   const bool      IsDevelopment  = CMPCisDevelopment(CommitId);
   const COLstring DevDir         = DIRcomponentDevDir(ComponentId);
   const COLstring RunDir         = DIRcomponentRunDir(ComponentId);
   const COLstring ConfigFile     = DIRcomponentConfig(ComponentId);
   const bool      IsPlainFolder  = GITisPlainFolder(ComponentId);
   const COLstring RunCommitId    = IsDevelopment ? "" : CommitId;
   const COLstring RunComponentId = IsDevelopment || RunComp == ComponentId ? "" : RunComp;
   COLstring Branch = "";
   COLvar&   Out = *pOut;
   try {
      COLstring Data, Url, Error;
      FILreadFile(ConfigFile, &Data);
      if(!Out.parse(Data)) return CMPCsetErrorVar(&Out, "Failed to parse config: " + Data);
      if(IsDevelopment && Out["template"])
         return CMPCsetErrorVar(&Out, "Cannot set commit to development for template component.");
      if(IsDevelopment && FILfileExists(RunDir) && !FILremoveFullDirNewSafe(RunDir, &Error)) {
         return CMPCsetErrorVar(&Out, "Failed to remove " + RunDir + ". " + Error);
      }
      if(!IsPlainFolder && !IsDevelopment) { GITUrepository(DevDir, "origin", &Url); }
      if(!IsPlainFolder && !IsDevelopment) { Branch = GITbranchCurrentDev(DevDir); }
      Out["run_commit"] = RunCommitId;
      Out["run_repo"]   = Url;
      Out["run_branch"] = Branch;
      Out["run_comp"]   = RunComponentId;
      FILwriteFile(ConfigFile, Out.json(1));
      Out["success"] = true;
   } catch(const COLerror& E) { return CMPCsetErrorVar(&Out, E.description()); }
   return true;
}

static void CMPCsetCommitImpl(const COLvar& In, COLvar* pOut) {
   COL_FUNCTION(CMPCsetCommitImpl);
   const COLstring ComponentId = In["component"];
   const COLstring CommitId    = In["run_commit"];
   const COLstring RunComp     = In["run_comp"];
   const COLstring User        = In["user"];
   if(!CMPCsetCommitJson(ComponentId, CommitId, RunComp, pOut)) {
      COL_TRC("Error setting commit");
      return;
   }
   if(CMPCisDevelopment(CommitId)) { return; }
   COLstring Error;
   if(!CMPCrunDir(ComponentId, (*pOut)["run_comp"], CommitId, &Error)) {
      COL_TRC("Error creating run dir");
      CMPCsetErrorVar(pOut, Error);
      return;
   }
   if(!Error.is_null()) { (*pOut)["warning"] = Error; }
}

// /component/set_commit
void CMPCsetCommit(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(CMPCsetCommit);
   COL_VAR(Request.Data.json(1));
   const COLstring Component = COLvarRequiredString(Request.Data, "component");
   const COLstring RunComp   = COLvarGetString(Request.Data, "run_comp", "");
   CFGconfig*      pConfig   = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Request.Address, "No component exists with name or id " + Component); }
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pConfig->CoreConfig.Guid, ROLid::EDIT_COMMIT, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_COMMIT));
   }
   if(!RunComp.is_null()) {
      CFGconfig* pRunConfig = CFGmapNode(pMap, RunComp);
      if(!pRunConfig) { return COLrespondError(Request.Address, "No component exists with name or id " + RunComp); }
      if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pRunConfig->CoreConfig.Guid, ROLid::VIEW_SCRIPT,
                             &Error)) {
         return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_COMMIT));
      }
   }
   COLvar Data;
   Data["component"]  = pConfig->CoreConfig.Guid;
   Data["run_commit"] = COLvarRequiredString(Request.Data, "commit");
   Data["run_comp"]   = RunComp;
   Data["user"]       = Request.User;
   SCKinvokeWorker(pLoop, CMPCsetCommitImpl, Data, COLnewClosure1(CMPCsetCommitCallback, pMap, Request.Address));
}
