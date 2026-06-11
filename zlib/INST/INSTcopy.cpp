// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTcopy
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Wednesday 09 August 2023 - 12:36PM
// ---------------------------------------------------------------------------

#include <INST/INSTcopy.h>
#include <INST/INSTworkingDir.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <DIR/DIRpath.h>

#include <COL/COLstring.h>
#include <COL/COLvarUtils.h>
#include <COL/COLutils.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool INSTdeleteFile(const COLstring& FilePath){
   COL_FUNCTION(INSTdeleteFile);
   COL_VAR(FilePath);
   for (int i=1; i < 30; i++){
      // we may need to retry
      COL_TRC("Try " << i);
      int Result = FILremove(FilePath, true);
      COL_VAR2(Result, COLerrorString(Result));
      if (!FILfileExists(FilePath)){
         COL_TRC("Successfully deleted file.");
         return true;
      }
      COL_TRC("Failed to delete file - try again!");
      COLsleep(1000);
   }
   return false;
}

// /install/copy
void INSTcopy(const COLwebRequest& Request){
   COL_FUNCTION(INSTcopy);
   const COLvar& Data = Request.Data;
   COLstring AppDir  = COLvarRequiredString(Data, "app_dir");
   COLstring DataDir = COLvarRequiredString(Data, "data_dir");
   COL_VAR2(AppDir, DataDir);
   AppDir  = FILpathAppend(AppDir, "IguanaX") + FIL_DIR_SEPARATOR;
   const COLstring IguanaExe = "iguana.exe";
   const COLstring IguanaIni = "iguana.ini";
   COLstring Path = FILpathAppend(AppDir, IguanaExe);
   COLstring IniPath = FILpathAppend(AppDir, IguanaIni);
   FILcorrectPathSeparators(Path);
   FILaddPathSeparator(DataDir);
   Path = FILexpand(Path);
   const COLstring CurrentExePath = FILexecutableDir() + FILexecutableFileName();
   COL_VAR2(CurrentExePath, Path);
   if(!FILfileExists(AppDir)){
      COL_TRC("Creating parent directory for the destination: " << AppDir);
      FILmakeFullDir(AppDir, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   }
   COLstring WorkingDir = INSTgetWorkingDir(Path);
   if (DataDir != WorkingDir) {
      COL_TRC("Set custom IguanaX working directory");
      INSTsaveWorkingDir(IniPath, DataDir);
      WorkingDir = DataDir;
   }
   // IMPORTANT: Set working dir so that the installer configures the correct iguana working dir
   // e.g. user, git, server name, etc.
   FILmakeFullDir(WorkingDir, FIL_USER_RWX);
   FILsetWorkingDir(WorkingDir);
   if (CurrentExePath == Path) { // in case we are doing an reinstall of the same iguana executable
      COLvar Result;
      Result["path"] = Path;
      return COLrespondSuccess(Request.Address, Result);
   }
   if(FILfileExists(AppDir) && !FILisDirectory(AppDir)){
      COLstring Err = "Destination directory provided is a file not a folder.";
      COL_TRC(Err);
      return COLrespondError(Request.Address, Err);
   }
   COL_VAR(Path);
   if (FILfileExists(Path)){
      if (!INSTdeleteFile(Path)){
         return COLrespondError(Request.Address, "Unable to delete " + Path);
      }
   }
   FILcopyFile(CurrentExePath, Path, true);
   COLvar Result;
   Result["path"] = Path;
   return COLrespondSuccess(Request.Address, Result);
}