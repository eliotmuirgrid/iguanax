//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: USRsetRoles.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   27/02/24 11:50 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <HASH/HASHutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKworker.h>
#include <USR/USRsetRoles.h>
COL_LOG_MODULE;

static void USRsetRolesCallback(COLvar Result, const COLaddress Address) {
   COL_FUNCTION(USRsetRolesCallback);
   Result["success"].asBool() ? COLrespondSuccess(Address) : COLrespondError(Address, Result["error"].asString());
}

static void USRsetRolesWorker(const COLvar& In, COLvar* pOut) {
   COL_FUNCTION(USRsetRolesWorker);
   COLstring  Error;
   const bool Success = FILsave(In["file"].asString(), In["content"].json(1), &Error);
   (*pOut)["success"] = Success;
   if(!Success) { (*pOut)["error"] = Error; }
}

static void USRsetRolesImpl(const COLstring& RequestUser, const COLstring& Username, const COLvar& RoleVar,
                            COLmap<COLstring, USRuser>* pUserMap, SCKloop* pLoop, const COLaddress& Address) {
   COL_FUNCTION(USRsetRolesImpl);
   USRuser&            UserObj = (*pUserMap)[Username];
   COLarray<COLstring> NewRoles;
   if(!HASHvarToArr(RoleVar, &NewRoles)) { return COLrespondError(Address, HASHparseError()); }
   UserObj.setRoles(NewRoles);
   COLvar Content = UserObj.toVar(true);
   COL_DBG("Content=\n" << Content);
   USRmodifyVarForSave(Content);
   COLvar Data;
   Data["user"]          = RequestUser;
   Data["modified_user"] = Username;
   Data["file"]          = DIRconfigUserDir(UserObj.username()) + "user.config";
   Data["content"]       = Content;
   COL_TRC("Launched save");
   SCKinvokeWorker(pLoop, USRsetRolesWorker, Data, COLnewClosure1(USRsetRolesCallback, Address));
}

// /user/set/roles
void USRsetRoles(const COLwebRequest& Request, COLmap<COLstring, USRuser>* pUserMap, SCKloop* pLoop) {
   COL_FUNCTION(USRsetRoles);
   COL_VAR(Request.Data.json(1));
   const COLstring& Username = COLvarRequiredString(Request.Data, "username");
   const COLvar&    RoleVar  = COLvarRequiredObject(Request.Data, "roles");
   if(pUserMap->count(Username) == 0) { return COLrespondError(Request.Address, "User does not exist"); }
   if((*pUserMap)[Username].isExternalUser()) {
      return COLrespondError(Request.Address, "Cannot modify roles of external user");
   }
   COLstring Err;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", EDIT_USERS, &Err)) {
      return COLrespondError(Request.Address, PRMsetError(EDIT_USERS));
   }
   USRsetRolesImpl(Request.User, Username, RoleVar, pUserMap, pLoop, Request.Address);
}