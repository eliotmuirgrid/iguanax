//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRadd
//
// Description:
//
// Implementation
// 
// Author: Eliot Muir/Orkhan Amikishiyev
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <HASH/HASHutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKworker.h>
#include <USR/USRadd.h>
#include <USR/USRmap.h>
#include <USR/USRutils.h>
COL_LOG_MODULE;

static void USRcomplete(bool Success, COLstring Error, COLaddress Address){
   COL_FUNCTION(USRcomplete);
   COL_VAR2(Success, Error);
   Success ? COLrespondSuccess(Address) : COLrespondError(Address, Error);
}

bool USRaddUser(COLmap<COLstring, USRuser>*  pUserMap, USRuser& User, const COLstring& RawPassword, COLstring* pError) {
   COL_FUNCTION(USRaddUser);
   if (User.username() != COLremoveAllWhiteSpace(User.username())) { *pError = "Username cannot have whitespace in it";             return false; }
   if (User.username().isWhitespace())                             { *pError = "Username cannot be empty";                          return false; }
   // TODO user name size check should not include escape characters in the check see IX-1721
   if (User.username().size() > 40)                                { *pError = "Username cannot be more than 40 characters long.";  return false; }
   if (pUserMap->count(User.username()) > 0)                       { *pError = "Username exists already";                           return false; }
	if(!USRvalidPassword(RawPassword, pError))                      { return false; }
	User.setPassword (RawPassword);
   pUserMap->add(User.username(), User);
   return true;
}

static void USRaddImp(const COLaddress& Address, COLmap<COLstring, USRuser>*  pUserMap, USRuser& User,
                      const COLstring& RawPassword, SCKloop* pLoop){
   COL_FUNCTION(USRaddImp);
   COLstring Error;
   if (!USRaddUser(pUserMap, User, RawPassword, &Error)) { return COLrespondError(Address, Error); }
   COLvar Content = User.toVar(true);
   USRmodifyVarForSave(Content);
   COLstring JsonOut = Content.json(1);
   COLstring FileName = DIRconfigUserDir(User.username()) + "user.config";
   COL_VAR2(FileName, Content);
   SCKwriteFile(pLoop, FileName, JsonOut, COLnewClosure2(&USRcomplete, Address));
}

// /user/add
void USRadd(const COLwebRequest& Request, COLmap<COLstring, USRuser>*  pUserMap, SCKloop* pLoop){
   COL_FUNCTION(USRadd);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::EDIT_USERS, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_USERS));
   }
   const COLvar& RoleVar = COLvarRequiredObject(Request.Data, "roles");
   COLarray<COLstring> NewRoles;
   if(!HASHvarToArr(RoleVar, &NewRoles)) { return COLrespondError(Request.Address, HASHparseError()); }
   USRuser User;
   User.setUsername(COLvarRequiredString(Request.Data, "username"));
   User.setRoles(NewRoles);
   COL_VAR(User);
   USRaddImp(Request.Address, pUserMap, User, COLvarRequiredString(Request.Data, "password"), pLoop);
}