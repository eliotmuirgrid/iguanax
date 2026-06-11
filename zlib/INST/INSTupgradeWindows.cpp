// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTupgradeWindows
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Monday 28 August 2023 - 10:35AM
// ---------------------------------------------------------------------------
#include <INST/INSTupgradeWindows.h>
#include <INST/INSTupgrade.h>
#include <INST/INSTdownload.h>
#include <INST/INSTunzip.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <SERV/SERVutils.h>

#include <SDM/SDMshutdown.h>

#include <PRO/PROlaunch.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool INSTwindowsRenameBinary(const COLstring& CurrentBinary, COLstring* pError){
   COL_FUNCTION(INSTwindowsRenameBinary);
   int RetryCount = 0;
   while (RetryCount < 5){
      try {
         FILrenameFile(CurrentBinary, FILpathAppend(FILexecutableDir(), SERV_OLD_TEMP));
         return true;
      } catch (const COLerror& Error) {
         COL_TRC("Failed to rename, maybe file is locked? Error: " << Error.code() << " " << Error.description());
      }
      RetryCount++;
   }
   *pError = "Could not alter the IguanaX binary. Ensure you have closed all other applications viewing IguanaX in particular the Event Viewer and the Services App";
   return false;
}

static bool INSTwindowsCopyBinary(const COLstring& CurrentBinary, const COLstring& DownloadFile, COLstring* pError){
   COL_FUNCTION(INSTwindowsCopyBinary);
   int RetryCount = 0;
   while (RetryCount < 5){
      try {
         FILcopyFile(DownloadFile, CurrentBinary);
         return true;
      } catch (const COLerror& Error){
         COL_TRC("Failed to copy. Error: " << Error.code() << " " << Error.description());
      }
      RetryCount++;
   }
   *pError = "Could not alter the IguanaX binary. Ensure you have closed all other applications viewing IguanaX in particular the Event Viewer and the Services App";
   return false;
}

static bool INSTwindowsDeleteOld(COLstring* pError){
   COL_FUNCTION(INSTwindowsDeleteOld);
   if (FILfileExists(FILexecutableDir() + SERV_OLD_TEMP))  {
      int Return = FILremove(FILexecutableDir() + SERV_OLD_TEMP);
      if (Return) {
         *pError = "Could not remove " + FILexecutableDir() + SERV_OLD_TEMP + ". Make sure you have permissions to do this, or remove this file manually and try again.";
         return false;
      }
   }
   return true;
}

bool INSTwindowsReplace(const COLstring& CurrentBinary, const COLstring& Version, COLstring* pError){
   COL_FUNCTION(INSTwindowsReplace);
   COLstring UnzipLocation = INSTdownloadDir() + FILpathNameWithoutLastExt(INSTdownloadFilePath(Version));
   if (!INSTunzip(INSTdownloadFilePath(Version), UnzipLocation)) {
      COL_TRC("We failed to extract");
      *pError = "Failed to extract " + INSTdownloadFilePath(Version);
      return false;
   }
   if (!INSTwindowsDeleteOld(pError))                   { return false; }
   if (!INSTwindowsRenameBinary(CurrentBinary, pError)) { return false; }
   COL_TRC("Sucessfully renamed running binary");
   COL_TRC("Copying " << UnzipLocation + "/iguana_setup.exe" << " to " << CurrentBinary);
   if (!INSTwindowsCopyBinary(CurrentBinary, FILpathAppend(UnzipLocation, "iguana_setup.exe"), pError)) { return false; }
   COL_TRC("Sucessfully copied over extracted file");
   COLstring Error;
   if (!FILremoveFullDirNewSafe(UnzipLocation, &Error)){ // clean up extracted files
      COL_TRC("Failed to cleanup " << UnzipLocation);
   }
   return true;
}

static bool INSTwindowsReplaceByInvokingServiceRestart(const COLstring& CurrentBinary, const COLstring& Version, COLstring* pError){
   COL_FUNCTION(INSTwindowsReplaceByInvokingServiceRestart);
   COLstring UnzipLocation = INSTdownloadDir() + FILpathNameWithoutLastExt(INSTdownloadFilePath(Version));
   if (!INSTunzip(INSTdownloadFilePath(Version), UnzipLocation)) {
      COL_TRC("We failed to extract");
      INSTlogUpgradeStartFailure();
      *pError = "Failed to extract " + INSTdownloadFilePath(Version);
      return false;
   }
   if (!INSTwindowsDeleteOld(pError))                   { INSTlogUpgradeStartFailure(); return false; }
   if (!INSTwindowsRenameBinary(CurrentBinary, pError)) { INSTlogUpgradeStartFailure(); return false; }
   COL_TRC("Sucessfully renamed running binary");
   COL_TRC("Copying " << UnzipLocation + "/iguana_setup.exe" << " to " << CurrentBinary);
   if (!INSTwindowsCopyBinary(CurrentBinary, FILpathAppend(UnzipLocation, "iguana_setup.exe"), pError)) { INSTlogUpgradeStartFailure(); return false; }
   COL_TRC("Sucessfully copied over extracted file");
   COLstring Error;
   if (!FILremoveFullDirNewSafe(UnzipLocation, &Error)){ // clean up extracted files
      COL_TRC("Failed to cleanup " << UnzipLocation);
   }
   COLstring Cmd = CurrentBinary + " --service restart";
   COL_VAR2(FILexecutableDir(), Cmd);
   int ProcessId = PROlaunch(FILexecutableDir(), Cmd);
   COL_VAR(ProcessId);
   bool Success = (0 != ProcessId);
   Success ? INSTlogUpgradeStartSuccess() : INSTlogUpgradeStartFailure();
   return Success;
}

bool INSTupgradeWindows(const COLstring& Version, COLstring* pError){
   COL_FUNCTION(INSTupgradeWindows);
   // On windows, we rename the current exe then copy over the downloaded one as iguana.exe and run iguana.exe --service install
   COLstring CurrentBinary = FILexecutableDir() + FILexecutableFileName();
  // return INSTwindowsReplace(CurrentBinary, Version, pError);
   return INSTwindowsReplaceByInvokingServiceRestart(CurrentBinary, Version, pError);
}