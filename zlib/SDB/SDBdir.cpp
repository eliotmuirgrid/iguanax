// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBdir
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday 16 February 2023 - 03:49PM
// ---------------------------------------------------------------------------
#ifdef __linux__
#include <unistd.h>
#endif
#include <SDB/SDBdir.h>
#include <SDB/SDBepochTime.h>
#include <SDB/SDBserviceLog.h>

#include <DIR/DIRpath.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <FIL/FILdirEnumerator.h>

#include <PRO/PROexecute.h>

#include <COL/COLdateTime.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#define SDB_PLAINTEXT_LOG_DIR_NAME "logt"    // TODO: change back to "log" when we get the chance
#define SDB_ENCRYPTED_LOG_DIR_NAME "loge"

static COLstring SDBlogDir = SDB_PLAINTEXT_LOG_DIR_NAME;
static COLstring SDBlogDir2;
static bool s_SDBencryptLogs = false;

static void SDBdirPermissionLinux(const COLstring& Directory) {
   COL_FUNCTION(SDBdirPermissionLinux);
#ifdef __linux__
   char* Username = getlogin();
   COL_VAR(Username);
   COLstring Out, Err;
   int Exit = PROexecuteString(".", "sudo chown " + COLstring(Username) +  " " + Directory, &Out, &Err);
   COL_VAR3(Exit, Out, Err);
#endif
}

bool SDBreadLogConfig(COLvar* pConfig, COLstring* pError){
   COL_FUNCTION(SDBreadLogConfig);
   try {
      COLstring ConfigFile = DIRfileConfigLog();
      if (!FILfileExists(ConfigFile)) { return true; }
      COLstring Config;
      FILreadFile(ConfigFile, &Config);
      if (!pConfig->parse(Config)) {
         *pError = "Failed to parse log configuration file";
         return false;
      }
   } catch (const COLerror& Error) {
      *pError = Error.description();
      COL_ERR(*pError);
      return false;
   }
   return true;
}

COLstring SDBgetLogParentDir(const COLvar& LogConfig) {
   COL_FUNCTION(SDBgetLogParentDir);
   COLstring ParentDir = FILworkingDir();
   if (LogConfig.exists("log_dir")) {
      ParentDir = LogConfig["log_dir"];
   }
   return ParentDir;
}

static COLstring SDBgetAltLogParentDir(const COLvar& LogConfig) {
   COL_FUNCTION(SDBgetAltLogParentDir);
   COLstring ParentDir;
   if (LogConfig.exists("alt_dir")) {
      ParentDir = LogConfig["alt_dir"];
   }
   return ParentDir;
}

static COLstring SDBgetLogDirName(const COLvar& LogConfig) {
   COL_FUNCTION(SDBgetLogDirName);
   COLstring DirName = SDB_PLAINTEXT_LOG_DIR_NAME;
   if (LogConfig.exists("encryption") && LogConfig["encryption"].asBool()) {
      s_SDBencryptLogs = true;
      DirName = SDB_ENCRYPTED_LOG_DIR_NAME;
   }
   return DirName;
}

static void SDBbackupLogDir(const COLstring& LogDirPath) {
   COL_FUNCTION(SDBbackupLogDir);
   COLstring CurrentTime = COLdateTime::currentTime().format("%Y%m%dT%H%M%S");
   COLstring BackupPath = LogDirPath + COL_T("_") + CurrentTime;
   COL_VAR2(BackupPath, CurrentTime);
   int NumMoved = FILrenameDirectory(LogDirPath, BackupPath, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX, true);
   COL_VAR(NumMoved);
   COLcout << "Backed up the existing log directory to '" << BackupPath << "' to avoid corruptions." << newline;
}

static void SDBbackupLogDirIfSettingUpLogEncryption(const COLvar& LogConfig, const COLstring& LogDirPath) {
   COL_FUNCTION(SDBbackupLogDirIfSettingUpLogEncryption);
   COL_VAR2(LogConfig, LogDirPath);
   if (!LogConfig.exists("initial_setup")) { return; }
   COLASSERT(LogConfig["initial_setup"].asBool())
   if (FILfileExists(LogDirPath)) {
      SDBbackupLogDir(LogDirPath);
   }
}

static void SDBcleanUpLogConfig(COLvar& LogConfig) {
   COL_FUNCTION(SDBcleanUpLogConfig);
   if (!FILfileExists(DIRfileConfigLog())) { return; }
   LogConfig.erase("initial_setup");   // or iguana will backup the log dir on every start
   COLstring ConfigFile = DIRfileConfigLog();
   try {
      FILwriteFile(ConfigFile, LogConfig.json(1));
   } catch (const COLerror& Error) {
      COL_ERR(Error);
   }
}

static void SDBcreateLogDirIfNeededAndCorrectLinuxPermissions(const COLstring& LogDirPath) {
   COL_FUNCTION(SDBcreateLogDirIfNeededAndCorrectLinuxPermissions);
   if (!FILfileExists(LogDirPath)) {
      FILmakeFullDir(LogDirPath, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   }
   // not sure why we
   SDBdirPermissionLinux(LogDirPath);
}

void SDBsetUpMainLogDirectory(COLstring* pLogDirPath, const COLvar& LogConfig){
   COL_FUNCTION(SDBsetUpMainLogDirectory);
   COLstring& LogDirPath = *pLogDirPath;
   COLstring LogParentDir = SDBgetLogParentDir(LogConfig);
   COLstring LogDirName   = SDBgetLogDirName(LogConfig);
   LogDirPath = FILpathAppend(LogParentDir, LogDirName);
   SDBbackupLogDirIfSettingUpLogEncryption(LogConfig, LogDirPath);
   SDBcreateLogDirIfNeededAndCorrectLinuxPermissions(LogDirPath);
   SDBlogDir = FILpathSepAdd(LogDirPath);
}

void SDBsetUpAltLogDirectory(COLstring* pAltLogDirPath, const COLvar& LogConfig){
   COL_FUNCTION(SDBsetUpAltLogDirectory);
   COLstring& AltLogDirPath = *pAltLogDirPath;
   COLstring AltLogParentDir = SDBgetAltLogParentDir(LogConfig);
   if (AltLogParentDir.is_null()) {
      COL_TRC("No configured secondary log directory");
      return;
   }
   if (AltLogParentDir == SDBgetLogParentDir(LogConfig)) {
      COL_TRC("Secondary log directory is the same as the main one - do nothing");
      return;
   }
   COLstring LogDirName = SDBgetLogDirName(LogConfig);
   AltLogDirPath = FILpathAppend(AltLogParentDir, LogDirName);
   SDBbackupLogDirIfSettingUpLogEncryption(LogConfig, AltLogDirPath);
   SDBcreateLogDirIfNeededAndCorrectLinuxPermissions(AltLogDirPath);
   SDBlogDir2 = FILpathSepAdd(AltLogDirPath);
}

bool SDBsetUpLogDirectory(COLstring* pLogDirPath, bool AllowAltDir, COLstring* pError) {
   COL_FUNCTION(SDBsetUpLogDirectory);
   COLstring& LogDirPath = *pLogDirPath;
   COLstring AltLogDirPath;
   try {
      COLvar LogConfig;
      if (!SDBreadLogConfig(&LogConfig, pError)) { return false; }
      SDBsetUpMainLogDirectory(pLogDirPath, LogConfig);
      if (AllowAltDir){ SDBsetUpAltLogDirectory(&AltLogDirPath, LogConfig); } // AllowAltDir is true for enterprise licenses
      SDBcleanUpLogConfig(LogConfig);
      COL_VAR(LogDirPath);
      COL_VAR(AltLogDirPath);
      return true;
   } catch(const COLerror& Error) {
      *pError = Error.description();
      COL_ERR(*pError);
   }
   return false;
}

bool SDBlogEncryptionEnabled() {
   return s_SDBencryptLogs;
}

COLstring SDBroot(){
   COL_FUNCTION(SDBroot);
   return SDBlogDir;
}

COLstring SDBroot2(){
   COL_FUNCTION(SDBroot2);
   return SDBlogDir2;
}

COLstring SDBdir(const COLstring& ComponentId, const COLstring& RootDir){
   COLASSERT(!ComponentId.is_null());
   return FILpathAppend(RootDir, ComponentId) + FIL_DIR_SEPARATOR;
}

COLstring SDBlogFile(const COLstring& ComponentId, COLuint64 Timestamp, const COLstring& RootDir){
   return SDBdir(ComponentId, RootDir) + SDBepochTimePath(Timestamp/1000);
}

COLstring SDBoldJournalFile(const COLstring& RootDir){
   // COL_FUNCTION(SDBoldJournalFile); // Too chatty
   COLstring Result = FILpathAppend(RootDir, "journal.old.log");
   return Result;
}

COLstring SDBjournalFile(const COLstring& RootDir){
   //COL_FUNCTION(SDBjournalFile);  // Too chatty
   COLstring Result = FILpathAppend(RootDir, "journal.log");
   return Result;
}

static COLstring SDBnextDir(const COLstring& DirPath, const COLstring& ComponentId) {
   COL_FUNCTION(SDBnextDir);
   COL_VAR(DirPath);
   // COLASSERT(FILisDirectory(DirPath));
   COLstring ParentDir, DirName, Name;
   DirPath.rsplit(ParentDir, DirName, FIL_DIR_SEPARATOR);
   if (DirName == ComponentId) {
      COL_TRC("Failed to find the next log file");
      return "";
   }
   COLASSERT(!DirName.is_null());
   FILdirEnumerator E(FILpathAppend(ParentDir, "*"), true);
   while (E.getFile(Name)) {
      if (Name <= DirName) { continue; }
      COLstring Path = FILpathAppend(ParentDir, Name);
      if (FILisDirectoryEmpty(Path)) {
         COL_TRC("Remove empty dir " << Path);
         COLstring Error;
         if (!FILremoveFullDirNewSafe(Path, &Error)) { COL_WRN(Error); }
      } else {
         COL_TRC("Found the next dir " << Path);
         return Path;
      }
   }
   COL_VAR2(Name, DirName);
   // either DirName is the last dir in current dir, or dirs after DirName are all empty
   COLASSERT(Name.is_null() || Name >= DirName);
   COL_TRC("The next dir is not in current dir. Check parent dir!");
   return SDBnextDir(ParentDir, ComponentId);
}

static COLstring SDBfirstFileInDir(const COLstring& DirPath) {
   COL_FUNCTION(SDBfirstFileInDir);
   COL_VAR(DirPath);
   COLASSERT(FILisDirectory(DirPath));
   COLstring Name;
   FILdirEnumerator E(FILpathAppend(DirPath, "*"));
   while (E.getFile(Name)) {
      Name = FILpathAppend(DirPath, Name);
      if (FILisDirectory(Name)) {
         COL_TRC("Need to check the first sub-dir!");
         return SDBfirstFileInDir(Name);
      } else {
         COL_TRC("Found the next log file!");
         COL_VAR(Name);
         return Name;
      }
   }
   return "";  // not found
}

// keep the log dir pristine. Garbage (non-log) files/dirs break this logic
COLstring SDBfindNextLogFile(const COLstring& CurrentFile, const COLstring& ComponentId) {
   COL_FUNCTION(SDBfindNextLogFile);
   COL_VAR(CurrentFile);
   COLstring ParentDir, Filename, Name;
   CurrentFile.rsplit(ParentDir, Filename, FIL_DIR_SEPARATOR);
   FILdirEnumerator E(FILpathAppend(ParentDir, "*.log"));
   while (E.getFile(Name)) {
      if (Name > Filename) {
         COL_TRC("Found the next log file!");
         return FILpathAppend(ParentDir, Name);
      }
   }
   COL_TRC("Need to check other log dirs");
   COLstring NextDir = SDBnextDir(ParentDir, ComponentId);
   if (NextDir.is_null()) { return ""; }
   return SDBfirstFileInDir(NextDir);
}

COLstring SDBfindPreviousLogFile(const COLstring& CurrentFile, const COLstring& ComponentId, const COLstring& RootDir){
   COL_FUNCTION(SDBfindPreviousLogFile);
   COL_VAR2(CurrentFile, ComponentId);
   if(!FILfileExists(CurrentFile)){
      COL_TRC("Could not find current file, cannot determine previous file");
      return "";
   }
   time_t Time = SDBlogFileTime(CurrentFile, ComponentId, RootDir);
   COLuint64 TimeStamp = (Time - 1) * 1000;
   COL_VAR(Time);
   return SDBfindLogFile(ComponentId, TimeStamp, RootDir);
}

static COLstring SDBprevDir(const COLstring& DirPath, const COLstring& ComponentId) {
   COL_FUNCTION(SDBprevDir);
   COL_VAR(DirPath);
   COLstring ParentDir, DirName, PrevDir, Name;
   DirPath.rsplit(ParentDir, DirName, FIL_DIR_SEPARATOR);
   if (DirName == ComponentId) {
      COL_TRC("Failed to find the previous log file");
      return "";
   }
   COLASSERT(!DirName.is_null());
   FILdirEnumerator E(FILpathAppend(ParentDir, "*"), true);
   while (E.getFile(Name)) {
      COLstring PrevPath = FILpathAppend(ParentDir, Name);
      if (FILisDirectoryEmpty(PrevPath)) { // this assumes that folders only contain log files
         COL_TRC("Remove empty dir " << PrevPath);
         COLstring Error;
         if (!FILremoveFullDirNewSafe(PrevPath, &Error)) { COL_WRN(Error); }
      } else if (Name < DirName) {
         PrevDir = PrevPath;
      } else if (Name >= DirName) {
         break;
      }
   }
   return PrevDir.is_null() ? SDBprevDir(ParentDir, ComponentId) : PrevDir;
}

static COLstring SDBlastFileInDir(const COLstring& DirPath) {
   COL_FUNCTION(SDBlastFileInDir);
   COL_VAR(DirPath);
   COLASSERT(FILisDirectory(DirPath));
   if (FILisDirectoryEmpty(DirPath)) { return ""; }
   COLstring Name;
   FILdirEnumerator E(FILpathAppend(DirPath, "*"));
   while (E.getFile(Name)) {}    // go to the last file or sub-dir
   Name = FILpathAppend(DirPath, Name);
   if (FILisDirectory(Name)) {
      COL_TRC("Need to check the last sub-dir!");
      return SDBlastFileInDir(Name);
   }
   COL_VAR(Name);
   return (FILpathExtLast(Name, false) == "log") ? Name : "";
}

COLstring SDBfindLogFile(const COLstring& ComponentId, COLuint64 Timestamp, const COLstring& RootDir) {
   COL_FUNCTION(SDBfindLogFile);
   COL_VAR(Timestamp);
   COLstring LogFile = SDBlogFile(ComponentId, Timestamp, RootDir);
   COL_VAR(LogFile);
   if (FILfileExists(LogFile)) {
      COL_TRC("File exists with the timestamp as the filename");
      return LogFile;
   }
   COL_TRC("No file exists with the timestamp as the filename, looking for the next smaller log file time");
   COLstring ParentDir, RequestedMessageTime, LastFileTime, CurrentFileTime;
   LogFile.rsplit(ParentDir, RequestedMessageTime, FIL_DIR_SEPARATOR);
   FILdirEnumerator E(FILpathAppend(ParentDir, "*.log"));
   while (E.getFile(CurrentFileTime)) {
      if (RequestedMessageTime < CurrentFileTime) {
         COL_TRC("Found a file with a larger time stamp");
         break;
      }
      LastFileTime = CurrentFileTime;
   }
   if(!LastFileTime.is_null()){
      COL_TRC("Found next smallest file: " << LastFileTime);
      return FILpathAppend(ParentDir, LastFileTime);
   }
   COL_TRC("Directory is empty or only contains files greater than the requested time");
   COLstring PrevDir = SDBprevDir(FILparentDir(LogFile), ComponentId);
   if (PrevDir.is_null()) {
      COL_TRC("There are no more directories to check");
      return "";
   }
   COLstring MatchingLogFile = SDBlastFileInDir(PrevDir);
   if(MatchingLogFile.is_null()){
      COL_TRC("There was no previous log file.");
   }
   return MatchingLogFile;
}

time_t SDBlogFileTime(const COLstring& LogFilePath, const COLstring& ComponentId, const COLstring& RootDir) {
   COL_FUNCTION(SDBlogFileTime);
   COL_VAR2(LogFilePath, RootDir);
   COLASSERT(LogFilePath.find(RootDir) != npos);
   COLstring EpochPath = LogFilePath.substr(SDBdir(ComponentId, RootDir).size());
   COL_VAR(EpochPath);
   return SDBepochTime(EpochPath);
}

COLstring SDBfindOldestSubDir(const COLstring& ParentDir){
   COL_FUNCTION(SDBfindOldestSubDir);
   COLstring Match(ParentDir);
   FILaddPathSeparator(Match);
   FILdirEnumerator Enum(Match + FIL_ANYFILE);
   COLstring File;
   if (Enum.getFile(File)){
      COL_TRC("Found: " << File);
      return Match + File;
   }
   COL_TRC("Didn't find a file");
   return File;
}

COLstring SDBfindNewestSubDir(const COLstring& ParentDir){
   COL_FUNCTION(SDBfindNewestSubDir);
   COLstring Match(ParentDir);
   FILaddPathSeparator(Match);
   FILdirEnumerator Enum(Match + FIL_ANYFILE);
   COLstring File;
   while (Enum.getFile(File)){
      COL_TRC("Found: " << File);
   }
   if (File.is_null()){
      return File;
   }
   return Match + File;
}

COLstring SDBfindOldestImpl(const COLstring& ParentDir){
   COL_FUNCTION(SDBfindOldestImpl);
   COLstring Result = SDBfindOldestSubDir(ParentDir);
   COLstring ResultDir;
   COL_VAR(Result);

   if (Result.is_null()){ return Result;}
   ResultDir = Result;
   Result = SDBfindOldestSubDir(Result + FIL_DIR_SEPARATOR);

   if (Result.is_null()){ return ResultDir;}
   ResultDir = Result;
   Result = SDBfindOldestSubDir(Result + FIL_DIR_SEPARATOR);

   if (Result.is_null()){ return ResultDir;}
   ResultDir = Result;
   Result = SDBfindOldestSubDir(Result + FIL_DIR_SEPARATOR);

   if (Result.is_null()){ return ResultDir;}
   return Result;
}

COLstring SDBfindNewestImpl(const COLstring& ParentDir){
   COL_FUNCTION(SDBfindNewest);
   COLstring Result = SDBfindNewestSubDir(ParentDir);
   COLstring ResultDir;
   COL_VAR(Result);

   if (Result.is_null()){ return Result;}
   ResultDir = Result;
   Result = SDBfindNewestSubDir(Result + FIL_DIR_SEPARATOR);

   if (Result.is_null()){ return ResultDir;}
   ResultDir = Result;
   Result = SDBfindNewestSubDir(Result + FIL_DIR_SEPARATOR);

   if (Result.is_null()){ return ResultDir;}
   ResultDir = Result;
   Result = SDBfindNewestSubDir(Result + FIL_DIR_SEPARATOR);

   if (Result.is_null()){ return ResultDir;}
   COL_VAR(Result);
   return Result;
}

COLstring SDBfindOldest(const COLstring& ParentDir){
   COL_FUNCTION(SDBfindOldest);
   COLstring Result = SDBfindOldestImpl(ParentDir);
   if(Result.is_null() || !FILisDirectory(Result)) return Result;
   if(FILisDirectoryEmpty(Result)){
      COL_TRC("Removing empty directory: " + Result);
      COLstring Error;
      if (!FILremoveFullDirNewSafe(Result, &Error)) { COL_WRN(Error); }
   }
   return SDBfindOldest(ParentDir);
}


COLstring SDBfindNewest(const COLstring& ParentDir){
   COL_FUNCTION(SDBfindNewest);
   COLstring Result = SDBfindNewestImpl(ParentDir);
   if(Result.is_null() || !FILisDirectory(Result)) return Result;
   if(FILisDirectoryEmpty(Result)){
      COL_TRC("Removing empty directory: " + Result);
      COLstring Error;
      if (!FILremoveFullDirNewSafe(Result, &Error)) { COL_WRN(Error); }
   }
   return SDBfindNewest(ParentDir);
}
