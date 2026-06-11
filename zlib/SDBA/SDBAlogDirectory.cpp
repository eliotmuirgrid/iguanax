// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBAlogDirectory
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday 09 May 2023 - 04:26PM
// ---------------------------------------------------------------------------
#include <SDBU/SDBUencryptionKey.h>
#include <COL/COLioCheck.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <DRX/DRXutils.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <SDB/SDBcapi.h>
#include <SDB/SDBdir.h>
#include <SDB/SDBsize.h>
#include <SDBA/SDBAlogDirectory.h>
COL_LOG_MODULE;

static bool SDBAcanWriteToDirectory(const COLstring& DirPath, COLstring* pErr) {
   COL_FUNCTION(SDBAcanWriteToDirectory);
   const COLstring TestFile = FILpathAppend(DirPath, "_testFile.txt");
   try {
      if(FILfileExists(TestFile)) { FILremove(TestFile); }
      FILwriteFile(TestFile, "");
      FILremove(TestFile);
      return true;
   } catch(const COLerror& E) {
      COL_TRC(E.description());
      *pErr = E.description();
      return false;
   }
}

void SDBAdirCheck(const COLstring& Dir, COLvar& Result) {
   COL_FUNCTION(SDBAdirCheck);
   Result.setMapType();
   if(Dir.is_null()) { return; }
   COLstring      Error;
   const COLstring LogSuffix = SDBUencryptionKeyGlobal().is_null() ? "logt/" : "loge/";
   const COLstring LogPath   = FILpathAppend(Dir, LogSuffix);
   Result["log_path"]        = LogPath;
   Result["path"]            = Dir;
   Result["dir_size"]        = COLint64ToString(SDBsize(LogPath));
   const bool Writable       = SDBAcanWriteToDirectory(Dir, &Error);
   Result["writable"]        = Writable;
   if(!Writable) { Result["error"] = Error; }
}

static void SDBAreadConfig(COLstring& LogParentDir, COLvar& Result) {
   COL_FUNCTION(SDBAreadConfig);
   COLvar    LogConfig;
   COLstring Error;
   if(!SDBreadLogConfig(&LogConfig, &Error)) {
      COL_TRC("Failed to read log config: " << Error);
      return;
   }
   LogParentDir      = SDBgetLogParentDir(LogConfig);
   COLstring AltDir  = LogConfig["alt_dir"].asString();
   if(!AltDir.is_null()) {
      FILaddPathSeparator(AltDir);
      SDBAdirCheck(AltDir, Result["alt_dir"]);
   }
}

static void SDBAgetStats(SDBlogStats Stats, SDBlogStats Stats2, COLvar Result, const COLaddress Address) {
   COL_FUNCTION(SDBAgetStats);
   Result["log_dir"]["disk_size"]  = COLuint64ToString(Stats.Disk.Size);
   Result["log_dir"]["disk_used"]  = COLuint64ToString(Stats.Disk.Used);
   Result["encrypted"]             = !SDBUencryptionKeyGlobal().is_null();
   Result["alt_enabled"] = DRXhighAvailabilityEnabled();
   if (Stats2.TotalSize) {
      Result["alt_dir"]["disk_size"]  = COLuint64ToString(Stats2.Disk.Size);
      Result["alt_dir"]["disk_used"]  = COLuint64ToString(Stats2.Disk.Used);
   }
   COLrespondSuccess(Address, Result);
}

// /log/directory
void SDBAlogDirectory(const COLwebRequest& Request) {
   COL_FUNCTION(SDBAlogDirectory);
   COLvar    Result;
   COLstring LogParentDir = FILworkingDir();
   if(FILfileExists(DIRfileConfigLog())) { SDBAreadConfig(LogParentDir, Result); }
   FILaddPathSeparator(LogParentDir);
   SDBAdirCheck(LogParentDir, Result["log_dir"]);
   SDBIgetLogStats(COLnewClosure2(&SDBAgetStats, Result, Request.Address));
}
