// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTupgrade
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Sunday 23 July 2023 - 02:21PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DBD/DBDworld.h>
#include <DBDVER/DBDVERversion.h>
#include <DBDVER/DBDVERversionUtils.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GINI/GINIdefault.h>
#include <GIT/GITpath.h>
#include <INST/INSTdownload.h>
#include <INST/INSTupgrade.h>
#include <INST/INSTupgradeLinux.h>
#include <INST/INSTupgradeMacOs.h>
#include <INST/INSTupgradeRecord.h>
#include <INST/INSTupgradeWindows.h>
#include <PRM/PRMcallbacks.h>
#include <PRO/PROexecute.h>
#include <PRO/PROlaunch.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
#include <SDM/SDMmanage.h>
#include <SDM/SDMshutdown.h>
#include <SEC/SECaccess.h>
COL_LOG_MODULE;

void INSTlogUpgradeStartFailure(){
   COL_FUNCTION(INSTlogUpgradeStartFailure);
   SDBIlogService("Failed to start the IguanaX upgrade process.", "#error #upgrade");
}

void INSTlogUpgradeStartSuccess(){
   COL_FUNCTION(INSTlogUpgradeStartSuccess);
   SDBIlogService("Successfully started the IguanaX upgrade process.", "#upgrade");
}

static void INSTrestartWindows(DBDworld* pWorld){
   COL_FUNCTION(INSTrestartWindows);
#ifdef _WIN32
   COL_TRC("Windows detected, setting ReturnFailure to true");
   pWorld->ReturnFailure = true;
   SDMshutdown();
   // exit(EXIT_FAILURE);
#endif
}

bool INSTreplace(const COLstring& Version, COLstring* pError){
   COL_FUNCTION(INSTreplace);
   if (!INSTdownloaded(Version)){
      *pError = "This version was not downloaded.";
      return false;
   }
   if (!INSTcreateUpgradeRecord(pError)) {
      COL_TRC("Failed to create upgrade record file");
      return false;
   }
   COLstring IgnoreError;
   if(!GINIdefault(&IgnoreError, true)) {
      COL_TRC("Failed to setup .ignore before upgrade: " + IgnoreError);
      SDBIlogService("Continuing IguanaX upgrade even though config/.gitignore setup failed: " + IgnoreError, "#upgrade");
   }

   const COLstring CacheDir = GITcacheDir();
   COLstring       ClearError;
   FILremoveFullDirNewSafe(CacheDir, &ClearError);
   COL_VAR(ClearError);

#ifdef _WIN32
   return INSTupgradeWindows(Version, pError);
#elif __APPLE__
   return INSTupgradeMacOs(Version, pError);
#else
   return INSTupgradeLinux(Version, pError);
#endif
}

void INSTrunAt(const COLstring& Command){
   COL_FUNCTION(INSTrunAt);
   time_t Now = time(NULL);
   Now += 60;
   struct tm* localTime = localtime(&Now);
   // Format the time as HH:MM and print it
   char timeString[6]; // HH:MM\0
   strftime(timeString, sizeof(timeString), "%H:%M", localTime);
   COLstring Cmd = "echo \"" + Command + "\" | at " + timeString;
   COLstring Out,Err;
   COL_VAR(Cmd);
   int ExitCode = PROexecuteString(".", Cmd, &Out, &Err);
   COL_VAR3(ExitCode, Out, Err);
}

static void INSTupgradeRespondOnMain(bool Success, COLstring Error, DBDworld* pWorld, COLaddress Address){
   COL_FUNCTION(INSTupgradeRespondOnMain);
   COL_VAR2(Success, Error);
   if(!Success || !Error.is_null()) {
      COLrespondError(Address, Error);
   } else {
      INSTrestartWindows(pWorld);
      COLrespondSuccess(Address);
   }
}

static void INSTrunCallbackOnMain(COLauto<COLclosure2<bool, COLstring>> pCallback, bool Success, COLstring Error){
   COL_FUNCTION(INSTrunCallbackOnMain);
   pCallback->run(Success, Error);
}

static void INSTupgradeReplaceImpl(COLwebRequest Request, COLauto<COLclosure2<bool, COLstring>> pCallback, SCKloop* pLoop){
   COL_FUNCTION(INSTupgradeReplaceImpl);
   const COLstring Version = COLvarRequiredString(Request.Data, "version");
   COLstring Error;
   SDBIlogService("Starting the IguanaX upgrade process as requested by user '" + Request.User + "'.", "#upgrade");
   const bool Success = INSTreplace(Version, &Error);
   pLoop->post(COLnewClosure0(INSTrunCallbackOnMain, pCallback, Success, Error));
}

// /upgrade/replace
void INSTupgradeReplace(const COLwebRequest& Request, SCKloop* pLoop, DBDworld* pWorld) {
   COL_FUNCTION(INSTupgradeReplace);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::UPGRADE, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::UPGRADE));
   }
   COLstring Version = COLvarRequiredString(Request.Data, "version");
   COLauto<COLclosure2<bool, COLstring>> pCallback = COLnewClosure2(&INSTupgradeRespondOnMain, pWorld, Request.Address);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&INSTupgradeReplaceImpl, Request, pCallback, pLoop));
}
