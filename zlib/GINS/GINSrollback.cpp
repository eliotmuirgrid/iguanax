//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINSrollback.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   19/09/24 11:46 AM
//  ---------------------------------------------------------------------------
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GINS/GINSrollback.h>
#include <GINS/GINSutils.h>
#include <GIT/GITexecute.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKworker.h>
COL_LOG_MODULE;

static COLstring GINSbackup = "config_backup/";

static void GINSrollbackCallback(COLvar Result, COLaddress Address) {
   COL_FUNCTION(GINSrollbackCallback);
   Result["success"].asBool() ? COLrespondSuccess(Address) : COLrespondError(Address, Result["error"]);
}

static bool GINSrollbackCommand(const COLstring& Dir, const COLstring& CommitHash, COLstring* pOut, COLstring* pErr) {
   COL_FUNCTION(GINSrollbackCommand);
   pOut->clear();
   pErr->clear();
   COLstring Command  = "git reset --hard " + CommitHash;
   int       ExitCode = GITexecuteString(Dir, Command, pOut, pErr);
   if(ExitCode != 0) { return false; }
   Command  = "git clean -fd";  // Clean any untracked files
   ExitCode = GITexecuteString(Dir, Command, pOut, pErr);
   COL_VAR2(*pOut, *pErr);
   return ExitCode == 0;
}

static void GINSrollbackFailed(const COLstring& Out, const COLstring& Err, COLvar* pOut) {
   COL_FUNCTION(GINSrollbackFailed);
   const auto      Mode   = FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX;
   const COLstring Backup = FILpathAppend(FILworkingDir(), GINSbackup);
   COLstring       RemoveErr;
   FILremoveFullDirNewSafe(Backup, &RemoveErr);
   const COLstring Message = Out + "\n" + Err;
   GINSsetError("Revert failed: " + Message, pOut);
}

static COLstring GINSrollbackCreateBackup(const COLstring& Backup) {
   COL_FUNCTION(GINSrollbackCreateBackup);
   const COLstring ConfigDir = DIRconfigDir();
   const auto      Mode      = FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX;
   FILcopyDirectoryContents(ConfigDir, Backup, Mode);
   return Backup;
}

static bool GINSrollbackPreChecks(const COLstring& BackupDir, const COLstring& CommitHash, COLvar* pOut) {
   COL_FUNCTION(GINSrollbackPreChecks);
   COLstring Out, Err;
   if(FILfileExists(BackupDir) && !FILremoveFullDirNewSafe(BackupDir, &Err)) {
      GINSsetError("Could not remove pre-existing backup directory: " + Err, pOut);
      return false;
   }
   GINSrollbackCreateBackup(BackupDir);
   if(!GINSstatusClean(BackupDir, &Out, &Err)) {
      const COLstring GitResponse = Out.is_null() ? Err : Out + "\n" + Err;
      GINSsetError(
          "Uncommitted changes present. Please commit them manually or by using the button below.\n" + GitResponse,
          pOut);
      (*pOut)["clean"] = false;
      return false;
   }
   if(!GINShashExists(BackupDir, CommitHash, &Out, &Err)) {
      GINSsetError(CommitHash + " not a valid commit hash", pOut);
      return false;
   }
   return true;
}

void GINSrollbackWorker(const COLvar& In, COLvar* pOut) {
   COL_FUNCTION(GINSrollbackWorker);
   const COLstring Hash      = In["hash"];
   const COLstring Username  = In["username"];
   const COLstring ConfigDir = DIRconfigDir();
   const COLstring BackupDir = FILpathAppend(FILworkingDir(), GINSbackup);  // we use this to attempt the rollback
   COL_VAR3(ConfigDir, Hash, BackupDir);
   (*pOut)["clean"] = true;
   if(!GINSrollbackPreChecks(BackupDir, Hash, pOut)) { return GINSrollbackFailed("", (*pOut)["error"], pOut); }
   COLstring Out, Err;
   if(!GINSrollbackCommand(BackupDir, Hash, &Out, &Err)) { return GINSrollbackFailed(Out, Err, pOut); }
   GINSinitializeSubmodules(BackupDir, &Err);
   if(!FILremoveFullDirNewSafe(ConfigDir, &Err)) { return GINSrollbackFailed("", Err, pOut); }
   FILrenameDirectory(BackupDir, ConfigDir, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX, true);
   (*pOut)["success"] = true;
   COL_VAR(pOut->json(1));
}

// instance/rollback
void GINSrollback(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(GINSrollback);
   COL_VAR(Request.Data.json(1));
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::MANAGE_INSTANCE, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::MANAGE_INSTANCE));
   }
   COLvar Data;
   Data["username"] = Request.User;
   Data["hash"]     = COLvarRequiredString(Request.Data, "hash");
   SCKinvokeWorker(pLoop, GINSrollbackWorker, Data, COLnewClosure1(GINSrollbackCallback, Request.Address));
}
