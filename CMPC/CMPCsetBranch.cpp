//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPCsetBranch.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCsetBranch.h>
#include <CMPC/CMPCutils.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLclosure6.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <GINS/GINSinitialize.h>
#include <GIT/GITbranch.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>

COL_LOG_MODULE;

static void CMPCrespondAndPost(const COLaddress Address, const bool Success, const COLstring& Error,
                               SCKloop* pLoop, COLclosure0* pCallback){
   COL_FUNCTION(CMPCrespondAndPost);
   if(Success) { COLrespondSuccess(Address); }
   else        { COLrespondError(Address, Error); }
   pLoop->post(pCallback);
}

static void CMPCrespondAndPostData(const COLaddress Address, const COLvar& Data, SCKloop* pLoop, COLclosure0* pCallback){
   COL_FUNCTION(CMPCrespondAndPostData);
   COLrespondSuccess(Address, Data);
   pLoop->post(pCallback);
}

static COLstring CMPCconfirmSwitchMessage(const COLstring& BranchName){
   COL_FUNCTION(CMPCconfirmSwitchMessage);
   return "Branch '" + BranchName + "' already exists.\n"
          "Switching source branches replaces this component's contents with that branch's contents.\n"
          "This does not merge or promote changes between branches.\n";
}

static void CMPCsetBranchRespondSuccess(const COLaddress Address, const COLstring& OldBranch, const COLstring& BranchName,
                                       const bool Created, SCKloop* pLoop, COLclosure0* pCallback){
   COL_FUNCTION(CMPCsetBranchRespondSuccess);
   COLvar Result;
   Result["old_branch"] = OldBranch;
   Result["branch"]     = BranchName;
   Result["created"]    = Created;
   CMPCrespondAndPostData(Address, Result, pLoop, pCallback);
}

static bool CMPCsetBranchValidate(const COLstring& Dir, const COLstring& CurrentBranch, const COLstring& BranchName,
                                 COLaddress Address, SCKloop* pLoop, COLclosure0* pCallback){
   COL_FUNCTION(CMPCsetBranchValidate);
   COLstring Error;
   if(CurrentBranch == BranchName)
      Error = "Already on branch: " + BranchName;
   else if(!GITbranchValidateName(Dir, BranchName, &Error))
      ;
   if(!Error.is_null()) {
      CMPCrespondAndPost(Address, false, Error, pLoop, pCallback);
      return false;
   }
   return true;
}

static bool CMPCsetBranchRejectIfExistsNotAllowed(const COLstring& Dir, const COLstring& Username, const COLstring& BranchName, const bool AllowExisting,
                                                 COLaddress Address, SCKloop* pLoop, COLclosure0* pCallback){
   COL_FUNCTION(CMPCsetBranchRejectIfExistsNotAllowed);
   if(AllowExisting) { return true; }
   bool Exists = false;
   COLstring Error;
   if(!GITbranchExistsLocal(Dir, BranchName, Username, &Exists, &Error)) {
      CMPCrespondAndPost(Address, false, Error, pLoop, pCallback);
      return false;
   }
   if(Exists) {
      CMPCrespondAndPost(Address, false, CMPCconfirmSwitchMessage(BranchName), pLoop, pCallback);
      return false;
   }
   return true;
}

static void CMPCsetBranchImpl(COLvar Data, COLaddress Address, SCKloop* pLoop, COLclosure0* pCallback) {
   COL_FUNCTION(CMPCsetBranchImpl);
   const COLstring Username      = Data["username"];
   const COLstring Component     = Data["component"]; 
   const COLstring BranchName    = Data["branch"];
   const bool      AllowExisting = (bool)Data["allow_existing"];
   const COLstring Dir           = DIRcomponentDevDir(Component);
   const COLstring CurrentBranch = GITbranchCurrent(Dir);
   COL_VAR3(Dir, CurrentBranch, AllowExisting);

   if(!CMPCsetBranchValidate(Dir, CurrentBranch, BranchName, Address, pLoop, pCallback))
      return;

   if(!CMPCsetBranchRejectIfExistsNotAllowed(Dir, Username, BranchName, AllowExisting, Address, pLoop, pCallback))
      return;

   COLstring Error;
   bool Created = false;
   if(CMPCcheckoutOrCreateBranch(Username, Dir, BranchName, &Created, &Error)) {
      if(!AllowExisting && !Created)
         return CMPCrespondAndPost(Address, false, CMPCconfirmSwitchMessage(BranchName), pLoop, pCallback);
      GINSaddComponent(Component);
      return CMPCsetBranchRespondSuccess(Address, CurrentBranch, BranchName, Created, pLoop, pCallback);
   }
   // if(!AllowExisting) {
   //    bool Exists = false;
   //    COLstring ExistsErr;
   //    if(GITbranchExistsLocal(Dir, BranchName, &Exists, &ExistsErr) && Exists)
   //       return CMPCrespondAndPost(Address, false, CMPCconfirmSwitchMessage(BranchName), pLoop, pCallback);
   // }
   return CMPCrespondAndPost(Address, false, Error, pLoop, pCallback);
}

// /component/set_branch
void CMPCsetBranch(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop){
   COL_FUNCTION(CMPCsetBranch);
   COL_VAR(Request.Data.json(1));
   const COLstring  Component      = COLvarRequiredString(Request.Data, "component");
   const COLstring  BranchName     = COLvarRequiredString(Request.Data, "branch");
   const bool       Development    = COLvarRequiredBool(Request.Data, "development");
   const bool       AllowExisting  = COLvarGetBool(Request.Data, "allow_existing", false);
   const CFGconfig* pConfig        = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Request.Address, "Component not found: " + Component); }
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pConfig->CoreConfig.Guid, EDIT_UPSTREAM, &Error)){
      return COLrespondError(Request.Address, PRMsetError(EDIT_UPSTREAM));
   }
   if(!Development) { return COLrespondError(Request.Address, "Cannot modify branch of a run repository."); }
   COLvar Data;
   Data["username"]       = Request.User;
   Data["component"]      = Component;
   Data["branch"]         = BranchName;
   Data["allow_existing"] = AllowExisting;
   pLoop->threadPool()->scheduleTask(COLnewClosure0( CMPCsetBranchImpl, Data, Request.Address, pLoop, CMPCcallback("/component/set_branch")));
}
