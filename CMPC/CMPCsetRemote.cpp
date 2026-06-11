// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCsetRemote
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Thursday 23 March 2023 - 12:16PM
// ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCsetRemote.h>
#include <CMPC/CMPCutils.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <GINS/GINSinitialize.h>
#include <GIT/GITbranch.h>
#include <GIT/GITpush.h>
#include <GITU/GITUaddRemote.h>
#include <GITU/GITUurl.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void CMPCsetRemoteImpl(COLvar Data, COLaddress Address, SCKloop* pLoop, COLclosure0* pCallback) {
   COL_FUNCTION(CMPCsetRemoteImpl);
   const COLstring Username  = Data["username"];
   const COLstring Component = Data["component"];
   const COLstring GitUrl    = Data["url"];
   bool            Push      = Data["push"];
   COLvar          Result;
   COLstring       Url;
   if(!GITUsshUrl(GitUrl, &Url)) {
      COLrespondError(Address, "Failed to parse url into ssh form");
      return pLoop->post(pCallback);
   }
   COL_VAR(Url);
   COLstring Error;
   COLstring ComponentDir = DIRcomponentDevDir(Component);
   if(!GITUaddRemote(ComponentDir, Url, &Error)) {
      Error = "Could not set remote repository: " + Error;
      COL_TRC(Error);
      COLrespondError(Address, Error);
      return pLoop->post(pCallback);
   }
   Result["git"] = GitUrl;
   COLstring Out;
   GINSaddComponent(Component);
   COLstring Branch        = GITbranchCurrent(ComponentDir);
   COLstring DefaultBranch = GITrepositoryDefaultBranch(ComponentDir);
   COL_VAR2(Branch, DefaultBranch);
   COLstring Flags = "-u origin " + DefaultBranch;
   if(Push && DefaultBranch != Branch && !GITpush(Username, ComponentDir, Flags, &Out, &Error)){
      COL_VAR2(Out, Error);
      COLrespondError(Address, Error);
   }
   if(Push && !GITpush(Username, ComponentDir, "", &Out, &Error)) {
      COL_VAR2(Out, Error);
      COLrespondError(Address, Error);
   } else {
      COLrespondSuccess(Address, Result);
   }
   pLoop->post(pCallback);
}

// /component/set_upstream
void CMPCsetRemote(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(CMPCsetRemote);
   COL_VAR(Request.Data.json(1));
   COLstring  Component   = COLvarRequiredString(Request.Data, "component");
   bool       Development = COLvarRequiredBool(Request.Data, "development");
   COLstring  GitUrl      = COLvarRequiredString(Request.Data, "git");
   bool       Push        = COLvarGetBool(Request.Data, "push", false);
   CFGconfig* pConfig     = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Request.Address, Component); }
   COLstring User = Request.User;
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, User, pConfig->CoreConfig.Guid, ROLid::EDIT_UPSTREAM, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_UPSTREAM));
   }
   if(!Development) { return COLrespondError(Request.Address, "Cannot modify upstream of a run repository."); }
   COLvar Data;
   Data["username"]  = User;
   Data["component"] = Component;
   Data["url"]       = GitUrl;
   Data["push"]      = Push;
   pLoop->threadPool()->scheduleTask(
       COLnewClosure0(CMPCsetRemoteImpl, Data, Request.Address, pLoop, CMPCcallback("/component/set_upstream")));
}
