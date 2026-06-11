//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOuser.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <DRX/DRXutils.h>
#include <FIL/FILutils.h>
#include <SCK/SCKloop.h>
#include <SSO/SSOclientVerify.h>
#include <SSO/SSOconfig.h>
#include <SSO/SSOuser.h>
#include <USR/USRmap.h>
COL_LOG_MODULE;

void SSOrunOnMain(COLauto<COLclosure3<bool, COLstring, const USRuser&>> pCallback, const bool Success,
                  const COLstring Message, const USRuser User) {
   COL_FUNCTION(SSOrunOnMain);
   pCallback->run(Success, Message, User);
}

void SSOfinishWithError(SCKloop* pLoop, COLauto<COLclosure3<bool, COLstring, const USRuser&>> pCallback,
                        const COLstring& Error, const USRuser& User) {
   COL_FUNCTION(SSOFinishWithError);
   pLoop->post(COLnewClosure0(&SSOrunOnMain, pCallback, false, Error, User));
}

static void SSOwriteUser(const USRuser User, const SSOprovider Provider, SCKloop* pLoop,
                         COLauto<COLclosure3<bool, COLstring, const USRuser&>> pCallback) {
   COL_FUNCTION(SSOwriteUser);
   COLstring       Error;
   const COLvar    Content  = User.toVar(false);
   const COLstring FileName = DIRconfigUserDir(User.username()) + "user.config";
   COL_TRC("Creating new " << SSOproviderToString(Provider) << " user: " << User.displayName() << " -> " << FileName);
   try {
      FILcreateDirectoryTreeToFile(FileName, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
      FILwriteFile(FileName, Content.json(1));
      COL_TRC("Successfully saved user config file: " << FileName);
      pLoop->post(COLnewClosure0(&SSOrunOnMain, pCallback, true, Error, User));

   } catch(const COLerror& e) {
      Error = "Failed to save user. " + e.description();
      COL_TRC("Failed to save user " + User.username() + " - " + e.description());
      return SSOfinishWithError(pLoop, pCallback, Error, User);
   }
}

static bool SSOhandleExistingUser(const USRuser& UpdatedUser, COLmap<COLstring, USRuser>* pUserMap, COLstring& Error) {
   COL_FUNCTION(SSOhandleExistingUser);
   const USRuser User = (*pUserMap)[UpdatedUser.username()];
   COL_VAR2(User.username(), UpdatedUser.username());
   if(User.username() != UpdatedUser.username()) {
      COL_TRC("User mismatch - ProviderId: " << UpdatedUser.username() << " mapped to username: " << User.username());
      Error = "Sign-in failed. Please use a different method or contact support.";
      return false;
   }
   return true;
}

bool SSOsetupUser(const USRuser& User, const SSOprovider Provider, COLmap<COLstring, USRuser>* pUserMap,
                  COLauto<COLclosure3<bool, COLstring, const USRuser&>> pCallback, SCKloop* pLoop, COLstring& Error) {
   if(pUserMap->count(User.username())) {
      COL_TRC("Found existing user with ID: " + User.username());
      if(!SSOhandleExistingUser(User, pUserMap, Error)) { return false; }
   }
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&SSOwriteUser, User, Provider, pLoop, pCallback));
   return true;
}