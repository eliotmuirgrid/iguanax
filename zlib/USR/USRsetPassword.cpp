//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: USRsetPassword.cpp
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
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKworker.h>
#include <USR/USRsetPassword.h>
#include <USR/USRutils.h>
COL_LOG_MODULE;

static void USRsetPasswordCallback(COLvar Result, COLaddress Address) {
	COL_FUNCTION(USRsetPasswordCallback);
	Result["success"].asBool() ? COLrespondSuccess(Address) : COLrespondError(Address, Result["error"].asString());
}

static void USRsetPasswordWorker(const COLvar& In, COLvar* pOut) {
	COL_FUNCTION(USRsetPasswordWorker);
	COLstring Error;
	const bool Success = FILsave(In["file"].asString(), In["content"], &Error);
	(*pOut)["success"] = Success;
	if(!Success) { (*pOut)["error"] = Error; }
}

static void USRsetPasswordImpl(const COLstring& RequestUser, const COLstring& Username, const COLstring& OldPassword, const COLstring& NewPassword,
                               bool HasEditPerm, COLmap<COLstring, USRuser>* pUserMap, SCKloop* pLoop, const COLaddress& Address) {
   COL_FUNCTION(USRsetPasswordImpl);
   USRuser& User = (*pUserMap)[Username];
   COL_VAR2(HasEditPerm, User);
   COLstring Err;
   if(!USRvalidPassword(NewPassword, &Err))            { return COLrespondError(Address, Err); }
   if(!HasEditPerm && !User.authenticate(OldPassword)) { return COLrespondError(Address, "Incorrect password"); }
   User.setPassword(NewPassword);
   COLvar Content = User.toVar(true);
   USRmodifyVarForSave(Content);
	COLvar Data;
	Data["user"]			 = RequestUser;
	Data["modified_user"] = Username;
	Data["file"]			 = DIRconfigUserDir(User.username()) + "user.config";
	Data["content"]		 = Content.json(1);
	SCKinvokeWorker(pLoop, USRsetPasswordWorker, Data, COLnewClosure1(USRsetPasswordCallback, Address));
}

// /user/set/password
void USRsetPassword(const COLwebRequest& Request, COLmap<COLstring, USRuser>*  pUserMap, SCKloop* pLoop) {
   COL_FUNCTION(USRsetPassword);
   COL_VAR(Request.Data.json(1));
   const COLstring& Username    = COLvarRequiredString(Request.Data, "username");
   const COLstring& Password    = COLvarRequiredString(Request.Data, "current_password");
   const COLstring& NewPassword = COLvarRequiredString(Request.Data, "password");
   if (pUserMap->count(Username) == 0)         { return COLrespondError(Request.Address, "User does not exist"); }
   if ((*pUserMap)[Username].isExternalUser()) { return COLrespondError(Request.Address, "Cannot modify password of external user"); }
   COLstring Err;
   bool HasEditPerm = PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::EDIT_USERS, &Err);
   if (!HasEditPerm && Username != Request.User) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_USERS));
   }
   USRsetPasswordImpl(Request.User, Username, Password, NewPassword, HasEditPerm, pUserMap, pLoop, Request.Address);
}
