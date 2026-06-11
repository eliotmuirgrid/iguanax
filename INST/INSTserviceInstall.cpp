// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTserviceInstall
//
// Description:
//
// Implementation
//
// Author: Matthew Sobkowski
// Date:   Monday 19 June 2023 - 11:17AM
// ---------------------------------------------------------------------------
#include <INST/INSTserviceInstall.h>
#include <INST/INSTinstall.h>
#include <INST/INSTshortcut.h>
#include <INST/INSTpermission.h>
#include <INST/INSTworkingDir.h>

#include <SDM/SDMmanage.h>
#include <SDM/SDMname.h>

#include <PRO/PROexecute.h>

#include <FIL/FILutils.h>

#include <DIR/DIRpath.h>

#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool INSTtakeOwnership(const COLstring& User, const COLstring& Dir, COLstring* pError){
   COL_FUNCTION(INSTtakeOwnership);
#ifndef _WIN32
   return true; // windows only
#endif
   COLstring DirWithoutTrailing = Dir;
   FILremovePathSeparator(DirWithoutTrailing);
   COLstring TakeownCmd = "takeown /f \"" + DirWithoutTrailing + "\" /r /d Y";
   COLstring IcalcsCmd  = "icacls \"" + DirWithoutTrailing + "\" /setowner \"" + User + "\" /t";
   COLstring Out, Err;
   int Exit = PROexecuteString("", TakeownCmd, &Out, &Err);
   if (Exit) {
      *pError = "Failed to take ownership of " + DirWithoutTrailing;
      return false;
   }
   Exit = PROexecuteString("", IcalcsCmd, &Out, &Err);
   if (Exit) {
      *pError = "Failed to grant ownership of " + DirWithoutTrailing + " to user account";
      return false;
   }
   return true;
}

// make sure User can access the working directory
static bool INSTgrantUserAccessPermissions(const COLstring& User, const COLstring& Exe, COLstring* pError) {
   COL_FUNCTION(INSTgrantUserAccessPermissions);
#ifndef _WIN32
   return true;   // windows only for now
#endif
   if (User.is_null()) {
      COL_TRC("Default user 'LocalSystem' already has full access to Iguana working dir.");
      return true;
   }
   bool SetPermissions = INSTgrantUserFullDirectoryControl(INSTgetWorkingDir(Exe), User, pError);
   if (!INSTtakeOwnership(User, INSTgetWorkingDir(Exe), pError)){
      COL_TRC("Failed to transfer ownership to " + User);
      return false;
   }
   return SetPermissions;
}

// /install/server/install
void INSTserviceInstall(const COLwebRequest& Request) {
   COL_FUNCTION(INSTserviceInstall);
   COLstring User, Password, Exe;
   // It doesn't matter if username or password are left empty for the SDMserverInstall API.
   User     = COLvarGetString(Request.Data, "username", "");
   Password = COLvarGetString(Request.Data, "password", "");
   Exe      = COLvarGetString(Request.Data, "exe",      "");
   COL_VAR3(User, Password, Exe);
   if (Exe.is_null()){
      COL_TRC("Setting EXE to default");
      Exe = FILexecutableDir() + FILexecutableFileName();
   }
   COL_VAR(Exe);
   COLstring Error;
   INSTcreateDesktopShortcut(Exe);
   if (!INSTgrantUserAccessPermissions(User, Exe, &Error)) {
      COL_ERR(Error);
      return COLrespondError(Request.Address, Error);
   }
   if (!INSTinstall(Exe, User, Password, &Error)) {
      COL_ERR(Error);
      return COLrespondError(Request.Address, Error);
   }
   #ifndef __APPLE__
   // The apple SDMserverInstall starts the service, but Windows and Linux does not.
   if (!SDMserverStart(SDMserviceName(), &Error)){
      return COLrespondError(Request.Address, Error);
   }
   #endif
   COLrespondSuccess(Request.Address);
}
