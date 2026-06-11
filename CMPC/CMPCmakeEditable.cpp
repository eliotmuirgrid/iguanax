// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCmakeRemoteSsh
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday 29 September 2023 - 04:47PM
// ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigSave.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCinitializeLibraries.h>
#include <CMPC/CMPCmakeEditable.h>
#include <CMPC/CMPCutils.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <GINS/GINSinitialize.h>
#include <GIT/GITclean.h>
#include <GIT/GITpull.h>
#include <GITU/GITUconfig.h>
#include <GITU/GITUremoveRemote.h>
#include <GITU/GITUurl.h>
#include <INI/INIdata.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKworker.h>
COL_LOG_MODULE;

static void CMPCmakeEditableCallback(COLvar Result, COLaddress Address) {
   COL_FUNCTION(CMPCmakeEditableCallback);
   COL_VAR(Result.json(1));
   Result["success"].asBool() ? COLrespondSuccess(Address, Result["data"])
                              : COLrespondError(Address, Result["error"]);
}

bool CMPCsaveConfigVar(const COLstring& Component, const COLvar& ConfigVar, COLstring* pErr){
   COL_FUNCTION(CMPCsaveConfigVar);
   const COLstring File    = DIRcomponentConfig(Component);
   const bool      Success = FILsave(File, ConfigVar.json(1), pErr);
   return Success;
}

static void CMPChttpsToSsh(const COLstring& RepoDir, INIdata* pConfig, COLvar* pOut) {
   COL_FUNCTION(CMPChttpsToSsh);
   const COLstring RemoteUrl = GITUconfigRemoteOriginUrl(*pConfig);
   COLstring       SshRemoteUrl;
   GITUsshUrl(RemoteUrl, &SshRemoteUrl);
   COL_VAR2(RemoteUrl, SshRemoteUrl);
   GITUconfigRemoteOriginUrlSet(pConfig, SshRemoteUrl);
   COL_TRC("Save config");
   GITUconfigSave(RepoDir, *pConfig);
   (*pOut)["data"]["new_git"] = SshRemoteUrl;
}

static void CMPCresetSubmodules(const COLstring& Component, const COLstring& User, const COLstring& RepoDir) {
   COL_FUNCTION(CMPCresetSubmodules);
   COLstring Out, Err;
   if(!GITpull(User, RepoDir, &Out, &Err)) { COL_TRC("Git pull failed: " << Err); }
   if(!CMPCinitializeAllLibraries(Component, User, "", &Err)) { COL_TRC("Library initialization failed: " << Err); };
   if(!GITclean(RepoDir, &Out, &Err)) { COL_TRC("Git clean failed: " << Err); };
}

static void CMPCmakeEditableImpl(const COLvar& In, COLvar* pOut) {
   COL_FUNCTION(CMPCmakeEditableImpl);
   COL_VAR(In.json(1));
   const COLstring& User      = In["user"];
   const COLstring& Component = In["component"];
   const bool       EditOrig  = In["edit_original"];
   const COLstring  RepoDir   = DIRcomponentDevDir(Component);
   CMPCresetSubmodules(Component, User, RepoDir);
   COLstring Err;
   INIdata Config;
   if(!GITUconfigLoad(RepoDir, &Config, &Err)) {
      COL_TRC("config load failed " << Err);
      CMPCsetErrorVar(pOut, Err);
      return;
   }
   if(EditOrig) { CMPChttpsToSsh(RepoDir, &Config, pOut); }
   else         { 
      (*pOut)["data"]["old_git"] = GITUremoveRemote(RepoDir); 
      
   }

   GINSaddComponent(Component);
   if(!CMPCsaveConfigVar(Component, In["config"], &Err)) { COL_TRC("Component config failed to save: " << Err); }
   (*pOut)["success"] = true;
}

//  /component/make_editable
void CMPCmakeEditable(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(CMPCmakeEditable);
   const COLstring Component = COLvarRequiredString(Request.Data, "component");
   const bool      EditOrig  = COLvarGetBool(Request.Data, "edit_original", false);
   CFGconfig*      pConfig   = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Request.Address, "No component exists with name or id " + Component); }
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pConfig->CoreConfig.Guid, EDIT_UPSTREAM, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(EDIT_UPSTREAM));
   }
   pConfig->CoreConfig.Template = false;
   COLvar Input;
   Input["component"]     = pConfig->CoreConfig.Guid;
   Input["edit_original"] = EditOrig;
   Input["config"]        = CFGconfigSerialize(*pConfig);
   Input["user"]          = Request.User;
   SCKinvokeWorker(pLoop, CMPCmakeEditableImpl, Input, COLnewClosure1(CMPCmakeEditableCallback, Request.Address));
}