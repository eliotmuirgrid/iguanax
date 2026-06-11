//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2026 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPCpromoteChanges.cpp
//
//  Description: Implementation
//  ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCpromoteChanges.h>
#include <CMPC/CMPCutils.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GINS/GINSinitialize.h>
#include <GIT/GITbranch.h>
#include <GIT/GITexecute.h>
#include <GIT/GITfetch.h>
#include <GIT/GITpush.h>
#include <GIT/GITstatusChanges.h>
#include <GITU/GITUrepository.h>
#include <GSV/GSVlink.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>

COL_LOG_MODULE;

static const COLstring CMPCPROMOTION_STATE_PENDING_COMMIT = "pending_commit";
static const COLstring CMPCPROMOTION_STATE_PENDING_PUSH   = "pending_push";

static COLstring CMPCpromotionStateFile(const COLstring& Dir){
   COL_FUNCTION(CMPCpromotionStateFile);
   return FILpathAppend(FILpathAppend(Dir, ".git"), "IGUANA_PROMOTION");
}

static bool CMPCpromotionStateWrite(const COLstring& Dir, const COLstring& OriginalSourceBranch,
                                    const COLstring& TargetBranch, const COLstring& StateName, COLstring* pError){
   COL_FUNCTION(CMPCpromotionStateWrite);
   try {
      COLvar State;
      State["original_source_branch"] = OriginalSourceBranch;
      State["target_branch"]          = TargetBranch;
      State["state"]                  = StateName;
      FILwriteFile(CMPCpromotionStateFile(Dir), State.json(1));
      return true;
   } catch(const COLerror& E) {
      if(pError) { *pError = "Could not write promotion state file. " + E.description(); }
      return false;
   }
}

static void CMPCpromotionStateClear(const COLstring& Dir){
   COL_FUNCTION(CMPCpromotionStateClear);
   const COLstring FileName = CMPCpromotionStateFile(Dir);
   if(FILfileExists(FileName)) { FILremove(FileName); }
}

static bool CMPCpromotionStateRead(const COLstring& Dir, COLvar* pOut, COLstring* pError){
   COL_FUNCTION(CMPCpromotionStateRead);
   const COLstring FileName = CMPCpromotionStateFile(Dir);
   if(!FILfileExists(FileName)) { return false; }
   try {
      COLstring Data;
      FILreadFile(FileName, &Data);
      COLvar Parsed;
      if(!Parsed.parse(Data)) {
         if(pError) { *pError = "Promotion state file is invalid JSON."; }
         return false;
      }
      if(pOut) { *pOut = Parsed; }
      return true;
   } catch(const COLerror& E) {
      if(pError) { *pError = "Could not read promotion state file. " + E.description(); }
      return false;
   }
}

static bool CMPCpromotionStateAdvance(const COLstring& Dir, const COLstring& StateName, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(CMPCpromotionStateAdvance);
   COLvar State;
   if(!CMPCpromotionStateRead(Dir, &State, pError)) {
      if(pResult) { (*pResult)["active"] = false; }
      return true;
   }
   const COLstring OriginalSourceBranch = COLvarGetString(State, "original_source_branch", "");
   const COLstring TargetBranch         = COLvarGetString(State, "target_branch", "");
   if(!CMPCpromotionStateWrite(Dir, OriginalSourceBranch, TargetBranch, StateName, pError)) { return false; }
   if(pResult) {
      (*pResult)["active"]                 = true;
      (*pResult)["original_source_branch"] = OriginalSourceBranch;
      (*pResult)["target_branch"]          = TargetBranch;
      (*pResult)["state"]                  = StateName;
   }
   return true;
}

static bool CMPCpromotionHasConflictMarkers(const COLstring& Dir){
   COL_FUNCTION(CMPCpromotionHasConflictMarkers);
   COLstring Out, Err;
   const int ExitCode = GITexecuteString(Dir, "git grep -n \"<<<<<<< HEAD\" -- .", &Out, &Err);
   return ExitCode == 0;
}

static bool CMPCpromotionActiveForStart(const COLstring& Dir, bool* pActive, COLvar* pState, COLstring* pError){
   COL_FUNCTION(CMPCpromotionActiveForStart);
   if(pActive) { *pActive = false; }
   if(!FILfileExists(CMPCpromotionStateFile(Dir))) { return true; }

   COLvar State;
   COLstring ReadError;
   if(!CMPCpromotionStateRead(Dir, &State, &ReadError)) {
      CMPCpromotionStateClear(Dir);
      return true;
   }

   COLstring Status, StatusError;
   if(!GITstatusChanges(Dir, &Status, &StatusError)) {
      if(pError) { *pError = StatusError.isWhitespace() ? "Could not check repository status." : StatusError; }
      return false;
   }

   const COLstring OriginalSourceBranch = COLvarGetString(State, "original_source_branch", "");
   if(Status.isWhitespace()) {
      const COLstring CurrentBranch = GITbranchCurrent(Dir);
      if(!OriginalSourceBranch.isWhitespace() && CurrentBranch == OriginalSourceBranch) {
         CMPCpromotionStateClear(Dir);
         return true;
      }
   }

   if(pActive) { *pActive = true; }
   if(pState)  { *pState = State; }
   return true;
}

static bool CMPCpromotionRejectActiveForStart(const COLstring& Dir, COLstring* pError){
   COL_FUNCTION(CMPCpromotionRejectActiveForStart);
   bool Active = false;
   COLvar State;
   if(!CMPCpromotionActiveForStart(Dir, &Active, &State, pError)) { return false; }
   if(!Active) { return true; }

   if(pError) {
      const COLstring OriginalSourceBranch = COLvarGetString(State, "original_source_branch", "");
      const COLstring TargetBranch         = COLvarGetString(State, "target_branch", "");
      const COLstring StateName            = COLvarGetString(State, "state", CMPCPROMOTION_STATE_PENDING_COMMIT);
      *pError = "A promotion is already active";
      if(!OriginalSourceBranch.isWhitespace() || !TargetBranch.isWhitespace()) {
         pError->append(" from '" + OriginalSourceBranch + "' to '" + TargetBranch + "'");
      }
      pError->append(" (" + StateName + "). Finish, exit, or abort that promotion before starting another.");
   }
   return false;
}

static bool CMPCpromoteBranchArgSafe(const COLstring& BranchName, COLstring* pError){
   COL_FUNCTION(CMPCpromoteBranchArgSafe);
   if(BranchName.isWhitespace() || BranchName[0] == '-') {
      if(pError) { *pError = "Branch name is not valid for promotion: " + BranchName; }
      return false;
   }
   for(int i = 0; i < BranchName.size(); ++i) {
      const char c = BranchName[i];
      const bool Ok = (c >= 'a' && c <= 'z') ||
                      (c >= 'A' && c <= 'Z') ||
                      (c >= '0' && c <= '9') ||
                      c == '/' || c == '-' || c == '_' || c == '.';
      if(!Ok) {
         if(pError) {
            *pError = "Promote Changes only supports branch names containing letters, numbers, '/', '-', '_' and '.'.";
         }
         return false;
      }
   }
   return true;
}

static bool CMPCpromoteRunGit(const COLstring& Dir, const COLstring& Command, COLstring* pOut, COLstring* pError){
   COL_FUNCTION(CMPCpromoteRunGit);
   COLstring Out, Err;
   const int ExitCode = GITexecuteString(Dir, Command, &Out, &Err);
   if(pOut) { *pOut = Out; }
   if(ExitCode == 0) { return true; }
   if(pError) { *pError = Err.isWhitespace() ? Out : Err; }
   return false;
}

static bool CMPCpromoteRepoClean(const COLstring& Dir, COLstring* pError){
   COL_FUNCTION(CMPCpromoteRepoClean);
   COLstring Status, Error;
   if(!GITstatusChanges(Dir, &Status, &Error)) {
      if(pError) { *pError = Error.isWhitespace() ? "Could not check repository status." : Error; }
      return false;
   }
   if(!Status.isWhitespace()) {
      if(pError) {
         *pError = "Cannot promote changes while the component has uncommitted files.\n"
                   "Commit or discard these changes first:\n" + Status;
      }
      return false;
   }
   return true;
}

static bool CMPCpromoteRefExists(const COLstring& Dir, const COLstring& Ref){
   COL_FUNCTION(CMPCpromoteRefExists);
   COLstring Out, Error;
   const int ExitCode = GITexecuteString(Dir, "git show-ref --verify --quiet " + Ref, &Out, &Error);
   return ExitCode == 0;
}

static bool CMPCpromoteSourceRef(const COLstring& Dir, const COLstring& SourceBranch, COLstring* pSourceRef, COLstring* pError){
   COL_FUNCTION(CMPCpromoteSourceRef);
   const COLstring LocalRef = "refs/heads/" + SourceBranch;
   if(CMPCpromoteRefExists(Dir, LocalRef)) {
      if(pSourceRef) { *pSourceRef = SourceBranch; }
      return true;
   }
   const COLstring RemoteRef = "refs/remotes/origin/" + SourceBranch;
   if(CMPCpromoteRefExists(Dir, RemoteRef)) {
      if(pSourceRef) { *pSourceRef = "origin/" + SourceBranch; }
      return true;
   }
   if(pError) { *pError = "Source branch does not exist locally or on origin: " + SourceBranch; }
   return false;
}

static bool CMPCpromoteDiffStaged(const COLstring& Dir, bool* pHasChanges, COLstring* pError){
   COL_FUNCTION(CMPCpromoteDiffStaged);
   COLstring Out, Err;
   const int ExitCode = GITexecuteString(Dir, "git diff --staged --quiet", &Out, &Err);
   if(ExitCode == 0) { if(pHasChanges) { *pHasChanges = false; } return true; }
   if(ExitCode == 1) { if(pHasChanges) { *pHasChanges = true; } return true; }
   if(pError) { *pError = Err.isWhitespace() ? Out : Err; }
   return false;
}

static bool CMPCpromoteResetMerge(const COLstring& Dir, COLstring* pError){
   COL_FUNCTION(CMPCpromoteResetMerge);
   COLstring Out;
   return CMPCpromoteRunGit(Dir, "git reset --merge HEAD", &Out, pError);
}

static bool CMPCpromoteRestoreBranch(const COLstring& Dir, const COLstring& OriginalBranch, COLstring* pError){
   COL_FUNCTION(CMPCpromoteRestoreBranch);
   if(OriginalBranch.isWhitespace() || OriginalBranch == "HEAD") { return true; }
   if(GITbranchCurrent(Dir) == OriginalBranch) { return true; }
   return GITbranchCheckoutExistingOrTrack(Dir, OriginalBranch, pError);
}

static bool CMPCpromoteAbortAndRestore(const COLstring& Dir, const COLstring& OriginalBranch, COLstring* pError){
   COL_FUNCTION(CMPCpromoteAbortAndRestore);
   COLstring ResetError, RestoreError;
   const bool ResetOk = CMPCpromoteResetMerge(Dir, &ResetError);
   const bool RestoreOk = CMPCpromoteRestoreBranch(Dir, OriginalBranch, &RestoreError);
   if(ResetOk && RestoreOk) { return true; }
   if(pError) {
      *pError = "";
      if(!ResetOk)   { pError->append("Could not reset the failed merge: " + ResetError + "\n"); }
      if(!RestoreOk) { pError->append("Could not switch back to " + OriginalBranch + ": " + RestoreError + "\n"); }
   }
   return false;
}

static bool CMPCpromoteValidateBranches(const COLstring& Dir, const COLstring& SourceBranch, const COLstring& TargetBranch, COLstring* pError){
   COL_FUNCTION(CMPCpromoteValidateBranches);
   if(SourceBranch == TargetBranch) {
      if(pError) { *pError = "Source and target branches must be different."; }
      return false;
   }
   if(!CMPCpromoteBranchArgSafe(SourceBranch, pError)) { return false; }
   if(!CMPCpromoteBranchArgSafe(TargetBranch, pError)) { return false; }
   if(!GITbranchValidateName(Dir, SourceBranch, pError)) { return false; }
   if(!GITbranchValidateName(Dir, TargetBranch, pError)) { return false; }
   return true;
}

static bool CMPCpromoteValidateType(const COLstring& PromotionType, COLstring* pError){
   COL_FUNCTION(CMPCpromoteValidateType);
   if(PromotionType == "squash" || PromotionType == "standard") { return true; }
   if(pError) { *pError = "Promotion type must be 'squash' or 'standard'."; }
   return false;
}

static bool CMPCpromoteAbortRun(const COLvar& Data, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(CMPCpromoteAbortRun);
   const COLstring Component = Data["component"];
   const COLstring Dir       = DIRcomponentDevDir(Component);

   if(!FILfileExists(CMPCpromotionStateFile(Dir))) {
      (*pResult)["active"]  = false;
      (*pResult)["aborted"] = false;
      return true;
   }

   COLvar State;
   if(!CMPCpromotionStateRead(Dir, &State, pError)) { return false; }
   const COLstring OriginalBranch = COLvarGetString(State, "original_source_branch", "");

   COLstring Out, Error;
   const int AbortCode = GITexecuteString(Dir, "git merge --abort", &Out, &Error);
   if(AbortCode != 0) {
      const int ResetCode = GITexecuteString(Dir, "git reset --hard HEAD", &Out, &Error);
      if(ResetCode != 0) {
         if(pError) {
            *pError = "Could not abort promotion merge state.\n" + (Error.isWhitespace() ? Out : Error);
         }
         return false;
      }
   }

   if(!OriginalBranch.isWhitespace()) {
      if(!GITbranchValidateName(Dir, OriginalBranch, pError)) { return false; }
      if(!GITbranchCheckoutExistingOrTrack(Dir, OriginalBranch, pError)) { return false; }
   }
   CMPCpromotionStateClear(Dir);

   (*pResult)["aborted"]                = true;
   (*pResult)["original_source_branch"] = OriginalBranch;
   (*pResult)["current_branch"]         = GITbranchCurrent(Dir);
   return true;
}

static COLstring CMPCpromoteMergeCommand(const COLstring& PromotionType, const COLstring& SourceRef){
   COL_FUNCTION(CMPCpromoteMergeCommand);
   if(PromotionType == "standard") { return "git merge --no-ff --no-commit " + SourceRef; }
   return "git merge --squash " + SourceRef;
}

static COLstring CMPCpromoteMergeLabel(const COLstring& PromotionType){
   COL_FUNCTION(CMPCpromoteMergeLabel);
   return PromotionType == "standard" ? "standard merge" : "squash merge";
}

static bool CMPCpromoteIsMergeConflict(const COLstring& ErrorText){
   COL_FUNCTION(CMPCpromoteIsMergeConflict);
   if(ErrorText.is_null()) { return false; }
   return ErrorText.find("CONFLICT") != -1 ||
          ErrorText.find("Automatic merge failed") != -1 ||
          ErrorText.find("could not apply") != -1;
}

static COLvar CMPCpromoteConflictFiles(const COLstring& Dir){
   COL_FUNCTION(CMPCpromoteConflictFiles);
   COLvar OutFiles;
   COLstring Out, Err;
   const int ExitCode = GITexecuteString(Dir, "git diff --name-only --diff-filter=U", &Out, &Err);
   if(ExitCode != 0 && ExitCode != 1) { return OutFiles; }
   COLvar Lines;
   COLsplit(&Lines, Out, "\n");
   for(const auto& it : Lines.array()) {
      COLstring FileName = it.asString().strip(COLstring::Both);
      FileName = FileName.strip(COLstring::Trailing, '\r');
      if(!FileName.isWhitespace()) { OutFiles.push_back(FileName); }
   }
   return OutFiles;
}

static bool CMPCpromoteFetchIfRemote(const COLstring& Username, const COLstring& Dir, COLstring* pError){
   COL_FUNCTION(CMPCpromoteFetchIfRemote);
   const COLstring Url = GITUrepositoryRemoteUrl(Dir);
   if(Url.is_null() || Url == "") { return true; }
   COLstring Out, Error;
   if(GITfetch(Username, Dir, "", &Out, &Error)) { return true; }
   if(pError) { *pError = "Could not fetch latest branches from origin.\n" + (Error.isWhitespace() ? Out : Error); }
   return false;
}

static bool CMPCpromoteChangesRun(const COLvar& Data, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(CMPCpromoteChangesRun);
   const COLstring Username     = Data["username"];
   const COLstring Component    = Data["component"];
   const COLstring SourceBranch = Data["source_branch"];
   const COLstring TargetBranch = Data["target_branch"];
   const COLstring PromotionType = Data["promotion_type"];
   const COLstring Dir           = DIRcomponentDevDir(Component);
   const COLstring Original      = GITbranchCurrent(Dir);
   COL_VAR4(Dir, Original, SourceBranch, TargetBranch);
   COL_VAR(PromotionType);

   if(!CMPCpromotionRejectActiveForStart(Dir, pError)) { return false; }
   if(!CMPCpromoteValidateBranches(Dir, SourceBranch, TargetBranch, pError)) { return false; }
   if(!CMPCpromoteValidateType(PromotionType, pError)) { return false; }
   if(!CMPCpromoteRepoClean(Dir, pError)) { return false; }
   if(!CMPCpromoteFetchIfRemote(Username, Dir, pError)) { return false; }

   COLstring SourceRef;
   if(!CMPCpromoteSourceRef(Dir, SourceBranch, &SourceRef, pError)) { return false; }
   bool TargetCreated = false;
   if(!CMPCcheckoutOrCreateBranch(Username, Dir, TargetBranch, &TargetCreated, pError)) { return false; }
   if(!CMPCpromoteRepoClean(Dir, pError)) {
      COLstring RestoreError;
      CMPCpromoteRestoreBranch(Dir, Original, &RestoreError);
      return false;
   }
   if(!CMPCpromotionStateWrite(Dir, Original, TargetBranch, CMPCPROMOTION_STATE_PENDING_COMMIT, pError)) { return false; }

   COLstring MergeOut, MergeError;
   if(!CMPCpromoteRunGit(Dir, CMPCpromoteMergeCommand(PromotionType, SourceRef), &MergeOut, &MergeError)) {
      const COLstring MergeDetails = MergeError.isWhitespace() ? MergeOut : (MergeError + "\n" + MergeOut);
      if(CMPCpromoteIsMergeConflict(MergeDetails)) {
         (*pResult)["source_branch"]   = SourceBranch;
         (*pResult)["target_branch"]   = TargetBranch;
         (*pResult)["original_branch"] = Original;
         (*pResult)["original_source_branch"] = Original;
         (*pResult)["current_branch"]  = TargetBranch;
         (*pResult)["promotion_type"]  = PromotionType;
         (*pResult)["merge_staged"]    = false;
         (*pResult)["merge_conflicts"] = true;
         (*pResult)["target_branch_created"] = TargetCreated;
         (*pResult)["committed"]       = false;
         (*pResult)["pushed"]          = false;
         (*pResult)["no_changes"]      = false;
         (*pResult)["conflict_output"] = MergeDetails;
         (*pResult)["conflict_files"]  = CMPCpromoteConflictFiles(Dir);
         return true;
      }
      COLstring CleanupError;
      CMPCpromoteAbortAndRestore(Dir, Original, &CleanupError);
      CMPCpromotionStateClear(Dir);
      if(pError) {
         *pError = "Could not stage the " + CMPCpromoteMergeLabel(PromotionType) + ".\n" + MergeError;
         if(!CleanupError.isWhitespace()) { pError->append("\n" + CleanupError); }
      }
      return false;
   }

   bool HasChanges = false;
   if(!CMPCpromoteDiffStaged(Dir, &HasChanges, pError)) {
      COLstring CleanupError;
      CMPCpromoteAbortAndRestore(Dir, Original, &CleanupError);
      CMPCpromotionStateClear(Dir);
      if(pError && !CleanupError.isWhitespace()) { pError->append("\n" + CleanupError); }
      return false;
   }

   if(!HasChanges) {
      COLstring RestoreError;
      const bool RestoreOk = CMPCpromoteRestoreBranch(Dir, Original, &RestoreError);
      (*pResult)["source_branch"]   = SourceBranch;
      (*pResult)["target_branch"]   = TargetBranch;
      (*pResult)["original_branch"] = Original;
      (*pResult)["original_source_branch"] = Original;
      (*pResult)["promotion_type"]  = PromotionType;
      (*pResult)["merge_staged"]    = false;
      (*pResult)["target_branch_created"] = TargetCreated;
      (*pResult)["committed"]       = false;
      (*pResult)["pushed"]          = false;
      (*pResult)["no_changes"]      = true;
      if(!RestoreOk) { (*pResult)["warning"] = "No changes were staged, but Iguana could not switch back to " + Original + ": " + RestoreError; }
      CMPCpromotionStateClear(Dir);
      return true;
   }

   (*pResult)["source_branch"]   = SourceBranch;
   (*pResult)["target_branch"]   = TargetBranch;
   (*pResult)["original_branch"] = Original;
   (*pResult)["original_source_branch"] = Original;
   (*pResult)["current_branch"]  = TargetBranch;
   (*pResult)["promotion_type"]  = PromotionType;
   (*pResult)["merge_staged"]    = true;
   (*pResult)["target_branch_created"] = TargetCreated;
   (*pResult)["committed"]       = false;
   (*pResult)["pushed"]          = false;
   (*pResult)["no_changes"]      = false;
   if(!MergeOut.isWhitespace()) { (*pResult)["merge_output"] = MergeOut; }
   return true;
}

static bool CMPCpromotionCompleteRun(const COLvar& Data, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(CMPCpromotionCompleteRun);
   const COLstring Component = Data["component"];
   const COLstring Dir       = DIRcomponentDevDir(Component);
   bool Active = FILfileExists(CMPCpromotionStateFile(Dir));
   if(!Active) {
      (*pResult)["active"] = false;
      return true;
   }
   if(!CMPCpromoteRepoClean(Dir, pError)) {
      (*pResult)["active"] = true;
      return true;
   }
   CMPCpromotionStateClear(Dir);
   (*pResult)["active"] = false;
   return true;
}

static void CMPCpromotionAddBranchWebLink(const COLstring& Dir, const COLstring& Branch, COLvar* pResult){
   COL_FUNCTION(CMPCpromotionAddBranchWebLink);
   if(!pResult || Branch.isWhitespace()) { return; }
   const COLstring Repo = GITUrepositoryRemoteUrl(Dir);
   if(Repo.isWhitespace()) { return; }
   GSVlink Link;
   if(!GSVconstructLink(Repo, &Link, Branch)) { return; }
   if(Link.m_Web_Branch.isWhitespace()) { return; }
   (*pResult)["web_branch_url"] = Link.m_Web_Branch;
}

static bool CMPCpromotionPushRun(const COLvar& Data, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(CMPCpromotionPushRun);
   const COLstring Username  = Data["username"];
   const COLstring Component = Data["component"];
   const COLstring Dir       = DIRcomponentDevDir(Component);

   COLvar State;
   if(!CMPCpromotionStateRead(Dir, &State, pError)) {
      if(pError && pError->isWhitespace()) { *pError = "No active promotion is waiting to be pushed."; }
      return false;
   }

   const COLstring StateName = COLvarGetString(State, "state", CMPCPROMOTION_STATE_PENDING_COMMIT);
   if(StateName != CMPCPROMOTION_STATE_PENDING_PUSH) {
      if(pError) { *pError = "Promotion must be committed before it can be pushed."; }
      return false;
   }

   COLstring Out, Error;
   if(!GITpush(Username, Dir, "", &Out, &Error)) {
      if(pError) {
         *pError = Error.isWhitespace()
            ? (Out.isWhitespace() ? "Could not push promotion." : Out)
            : Error;
      }
      return false;
   }

   const COLstring TargetBranch = COLvarGetString(State, "target_branch", "");
   (*pResult)["pushed"]        = true;
   (*pResult)["target_branch"] = TargetBranch;
   CMPCpromotionAddBranchWebLink(Dir, TargetBranch, pResult);
   if(!Out.isWhitespace()) { (*pResult)["output"] = Out; }
   return true;
}

static bool CMPCpromotionStateRun(const COLvar& Data, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(CMPCpromotionStateRun);
   const COLstring Component = Data["component"];
   const COLstring Dir       = DIRcomponentDevDir(Component);
   const COLstring FileName  = CMPCpromotionStateFile(Dir);
   if(!FILfileExists(FileName)) {
      (*pResult)["active"] = false;
      return true;
   }

   COLvar State;
   COLstring ReadError;
   if(!CMPCpromotionStateRead(Dir, &State, &ReadError)) {
      CMPCpromotionStateClear(Dir);
      (*pResult)["active"] = false;
      return true;
   }

   COLstring Status, StatusError;
   if(!GITstatusChanges(Dir, &Status, &StatusError)) {
      if(pError) { *pError = StatusError.isWhitespace() ? "Could not check repository status." : StatusError; }
      return false;
   }
   const COLstring OriginalSourceBranch = COLvarGetString(State, "original_source_branch", "");
   const COLstring TargetBranch         = COLvarGetString(State, "target_branch", "");
   const COLstring StateName            = COLvarGetString(State, "state", CMPCPROMOTION_STATE_PENDING_COMMIT);
   if(Status.isWhitespace()) {
      const COLstring CurrentBranch = GITbranchCurrent(Dir);
      if(!OriginalSourceBranch.isWhitespace() && CurrentBranch == OriginalSourceBranch) {
         CMPCpromotionStateClear(Dir);
         (*pResult)["active"] = false;
         return true;
      }
   }

   (*pResult)["active"]                 = true;
   (*pResult)["original_source_branch"] = OriginalSourceBranch;
   (*pResult)["source_branch"]          = OriginalSourceBranch;
   (*pResult)["target_branch"]          = TargetBranch;
   (*pResult)["state"]                  = StateName;
   (*pResult)["has_conflict"]           = CMPCpromotionHasConflictMarkers(Dir);
   return true;
}

static void CMPCpromotionStateImpl(COLvar Data, COLaddress Address, SCKloop* pLoop, COLclosure0* pCallback){
   COL_FUNCTION(CMPCpromotionStateImpl);
   COLvar Result;
   COLstring Error;
   const bool Success = CMPCpromotionStateRun(Data, &Result, &Error);
   if(Success) { COLrespondSuccess(Address, Result); }
   else        { COLrespondError(Address, Error); }
   pLoop->post(pCallback);
}

// /component/promotion_state
void CMPCpromotionState(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop){
   COL_FUNCTION(CMPCpromotionState);
   const COLstring Component    = COLvarRequiredString(Request.Data, "component");
   const bool      Development  = COLvarRequiredBool(Request.Data, "development");
   const CFGconfig* pConfig     = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Request.Address, "Component not found: " + Component); }
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pConfig->CoreConfig.Guid, ROLid::VIEW_SCRIPT, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::VIEW_SCRIPT));
   }
   if(!Development) {
      COLvar Out;
      Out["active"] = false;
      return COLrespondSuccess(Request.Address, Out);
   }
   COLvar Data;
   Data["component"] = Component;
   pLoop->threadPool()->scheduleTask(COLnewClosure0(CMPCpromotionStateImpl, Data, Request.Address, pLoop,
                                                    CMPCcallback("/component/promotion_state")));
}

static bool CMPCpromotionAdvanceStateRun(const COLvar& Data, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(CMPCpromotionAdvanceStateRun);
   const COLstring Dir = DIRcomponentDevDir(Data["component"]);
   return CMPCpromotionStateAdvance(Dir, CMPCPROMOTION_STATE_PENDING_PUSH, pResult, pError);
}

static bool CMPCpromoteChangesActionRun(const COLvar& Data, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(CMPCpromoteChangesActionRun);
   const COLstring Action = COLvarGetString(Data, "action", "start");
   if(Action == "start")    { return CMPCpromoteChangesRun(Data, pResult, pError); }
   if(Action == "abort")    { return CMPCpromoteAbortRun(Data, pResult, pError); }
   if(Action == "advance")  { return CMPCpromotionAdvanceStateRun(Data, pResult, pError); }
   if(Action == "push")     { return CMPCpromotionPushRun(Data, pResult, pError); }
   if(Action == "complete") { return CMPCpromotionCompleteRun(Data, pResult, pError); }
   if(pError) { *pError = "Unknown promotion action: " + Action; }
   return false;
}

static bool CMPCpromoteActionValid(const COLstring& Action){
   COL_FUNCTION(CMPCpromoteActionValid);
   return Action == "start" ||
          Action == "abort" ||
          Action == "advance" ||
          Action == "push" ||
          Action == "complete";
}

static void CMPCpromoteChangesImpl(COLvar Data, COLaddress Address, SCKloop* pLoop, COLclosure0* pCallback){
   COL_FUNCTION(CMPCpromoteChangesImpl);
   COLvar Result;
   COLstring Error;
   const bool Success = CMPCpromoteChangesActionRun(Data, &Result, &Error);
   GINSaddComponent(Data["component"]);
   if(Success) { COLrespondSuccess(Address, Result); }
   else        { COLrespondError(Address, Error); }
   pLoop->post(pCallback);
}

// /component/promote_changes
void CMPCpromoteChanges(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop){
   COL_FUNCTION(CMPCpromoteChanges);
   COL_VAR(Request.Data.json(1));
   const COLstring Component   = COLvarRequiredString(Request.Data, "component");
   const COLstring Action      = COLvarGetString(Request.Data, "action", "start");
   const bool      Development = COLvarRequiredBool(Request.Data, "development");
   const CFGconfig* pConfig    = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Request.Address, "Component not found: " + Component); }
   if(!CMPCpromoteActionValid(Action)) { return COLrespondError(Request.Address, "Unknown promotion action: " + Action); }
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pConfig->CoreConfig.Guid, ROLid::EDIT_SCRIPT, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   if(!Development) { return COLrespondError(Request.Address, "Cannot update promotion state in a run repository."); }

   COLvar Data;
   Data["username"]  = Request.User;
   Data["component"] = Component;
   Data["action"]    = Action;
   if(Action == "start") {
      Data["source_branch"]  = COLvarRequiredString(Request.Data, "source_branch");
      Data["target_branch"]  = COLvarRequiredString(Request.Data, "target_branch");
      Data["promotion_type"] = COLvarGetString(Request.Data, "promotion_type", "squash");
   }
   pLoop->threadPool()->scheduleTask(COLnewClosure0(CMPCpromoteChangesImpl, Data, Request.Address, pLoop,
                                                    CMPCcallback("/component/promote_changes")));
}
