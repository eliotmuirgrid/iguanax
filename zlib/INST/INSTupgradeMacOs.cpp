// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTupgradeMacOs
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Monday 28 August 2023 - 10:11AM
// ---------------------------------------------------------------------------
#include <INST/INSTupgradeMacOs.h>
#include <INST/INSTupgrade.h>
#include <INST/INSTdownload.h>
#include <INST/INSTunzip.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <APP/APPmain.h>

#include <PRO/PROlaunch.h>
#include <SDM/SDMmanage.h>
#include <SDM/SDMname.h>
#include <SDM/SDMshutdown.h>

#include <COL/COLsplit.h>
#include <COL/COLutils.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


#define INST_IGUANX_OLD "___IguanaX_old_temp"
// This function returns IguanaX.app from the below
// /Applications/IguanaX.app/Contents/MacOS/
//               ^^^^^^^^^^^
COLstring INSTmyPackageName(){
   COL_FUNCTION(INSTmyPackageName);
   COLstring CurrentDir = FILexecutableDir();
   COLarray<COLstring> Dirs;
   COLsplit(&Dirs, CurrentDir, "/");
   COL_VAR2(CurrentDir, Dirs.size());
   if (Dirs.size() < 3){
      COL_TRC("Couldn't find package");
      return "IguanaX.app";
   }
   COL_VAR(Dirs[Dirs.size()-2]);
   if (Dirs[Dirs.size()-2] != "MacOS"){
      COL_TRC("iguana exe is not part of Mac Bundle");
      return "IguanaX.app";
   }
   COL_VAR(Dirs[Dirs.size()-4]);
   return Dirs[Dirs.size()-4];
}

static void INSTappleRenameOld(const COLstring& AppDir){
   COL_FUNCTION(INSTappleRenameOld);
   COLstring FileName = AppDir + INSTmyPackageName();
   COLstring OldFileName = AppDir + INST_IGUANX_OLD + ".app";
   COL_VAR2(FileName, OldFileName);
   if (FILfileExists(FileName)){
      COL_TRC("Renaming existing IguanaX to " << INST_IGUANX_OLD);
      FILrenameFile(FileName, OldFileName);
   } else {
      COL_TRC("No old app to rename.");
   }
}


static void INSTappleDeleteOld(const COLstring& AppDir){
   COL_FUNCTION(INSTappleDeleteOld);
   COLstring OldFileName = AppDir + INST_IGUANX_OLD + ".app";
   COL_VAR(OldFileName);
   if (FILfileExists(OldFileName)){
      COL_TRC(INST_IGUANX_OLD << " exists - deleting");
      COLstring Error;
      if (!FILremoveFullDirNewSafe(OldFileName, &Error)){ 
         COL_TRC("Failed to delete old app. " + Error);
      }
   } else {
      COL_TRC("No old app to delete.");
   }
}

static COLstring INSTappleAppDir(){
   COL_FUNCTION(INSTappleAppDir);
   COLstring Result, ResultErr;
   FILpathSimplify(FILexecutableDir() + "../../../", &Result, &ResultErr);
   COL_VAR(Result);
   FILaddPathSeparator(Result);  // Just in case.
   COL_VAR(Result);
   return Result;
}

bool INSTrestartMacIguana(const COLstring& AppDir){
   COL_FUNCTION(INSTrestartMacIguana);
   COLstring ServiceUser;
   COLstring Error;
   bool Success = SDMserverUser(SDMserviceName(), &ServiceUser, &Error);
   if (!Success){
      // Not that critical.
      COL_ERR("Failed to get service user:" << ServiceUser);
      ServiceUser.clear();
   }
   COLstring Cmd = "iguana --service install";
   COLstring Dir = AppDir + "Contents/MacOS/";
   if (!ServiceUser.is_null()) {
      Cmd.append(" --username " + ServiceUser);
   }
   COL_VAR2(Dir, Cmd);
   int ProcessId = PROlaunch(Dir, Cmd);
   COL_VAR(ProcessId);
   return 0 != ProcessId;
}



bool INSTlaunchNewController(const COLstring& AppDir){
   COL_FUNCTION(INSTlaunchNewController);
   COLstring Cmd = AppDir + "Contents/MacOS/iguana --controller";
   COLstring Dir = ".";
   COL_VAR2(Dir, Cmd);
   int ProcessId = PROlaunch(Dir, Cmd);
   COL_VAR(ProcessId);
   return 0 != ProcessId;
}

bool INSTupgradeMacOs(const COLstring& Version, COLstring* pError){
   COL_FUNCTION(INSTupgradeMacOs);
   COLstring UnzipLocation = INSTdownloadDir() + FILpathNameWithoutLastExt(INSTdownloadFilePath(Version));
   if (!INSTunzip(INSTdownloadFilePath(Version), UnzipLocation)) {
      COL_TRC("We failed to extract");
      INSTlogUpgradeStartFailure();
      *pError = "Failed to extract " + INSTdownloadFilePath(Version);
      return false;
   }
   COLstring AppDir = INSTappleAppDir();
   COL_VAR(AppDir);
   COLstring Source = UnzipLocation + "/IguanaX.app";
   COLstring NewApp = AppDir + "IguanaX_new.app";
   COLstring AppLocation = AppDir + INSTmyPackageName();
   COL_TRC("Copying from " << Source << " to " << NewApp);
   int Count = FILcopyDirectoryContents(Source, NewApp, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   COL_VAR(Count);
   if (Count < 2) {
      COL_TRC("Count far too small - failed to copy everything");
      INSTlogUpgradeStartFailure();
      *pError = "Failed to copy the files from the dmg.";
      return false;
   }
   COL_TRC("Remove: " << UnzipLocation);
   COLstring RemoveError;
   FILremoveFullDirNewSafe(UnzipLocation, &RemoveError); // clean up extracted files. Not the end of the world if we fail to remove.
   COL_TRC("If old iguana instance exists, delete it. Then rename the running IguanaX to " << INST_IGUANX_OLD);
   INSTappleDeleteOld(AppDir);
   INSTappleRenameOld(AppDir);
   COL_TRC("Rename new install from IguanaX_new to IguanaX and give executable permission");
   FILrenameFile(NewApp, AppLocation);
   COLstring IguanaExe = AppLocation + "/Contents/MacOS/iguana";
   COL_TRC("Setting permissions for IguanaExe");
   FILsetPermission(IguanaExe, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   INSTappleDeleteOld(AppDir);
   COL_TRC("Install new iguana.");
   APPkillMacController();
   //INSTlaunchNewController(AppDir + INSTmyPackageName() + "/");
   //COLsleep(2000);
   //INSTrestartMacIguana(AppDir + INSTmyPackageName() + "/");
   INSTlogUpgradeStartSuccess();
   SDMshutdown();
   return true;
}