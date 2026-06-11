// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCutils
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Wednesday 05 April 2023 - 01:03PM
// ---------------------------------------------------------------------------
#include <BLUA/BLUAmanager.h>
#include <BLUA/BLUAstart.h>
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigSave.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCutils.h>
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <COL/COLstack.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GIT/GITbranch.h>
#include <GIT/GITcommit.h>
#include <GIT/GITpush.h>
#include <GITU/GITUrepository.h>
#include <SCK/SCKworker.h>
#include <SDB/SDBcapi.h>
COL_LOG_MODULE;

static void CMPCcallbackComplete(COLstring Api) {
   COL_FUNCTION(CMPCcallbackComplete);
   COL_TRC(Api + " completed");
}

COLclosure0* CMPCcallback(const COLstring& Api) {
   COL_FUNCTION(CMPCcallback);
   return COLnewClosure0(&CMPCcallbackComplete, Api);
}

static void CMPCsaveCallback(COLvar Result, COLaddress Address) {
   COL_FUNCTION(CMPCsaveCallback);
   COL_VAR(Result);
   Result["success"].asBool() ? COLrespondSuccess(Address) : COLrespondError(Address, Result["error"].asString());
}

static void CMPCsaveConfigWorker(const COLvar& In, COLvar* pOut) {
   COL_FUNCTION(CMPCsaveConfigWorker);
   COLstring       Error;
   const COLstring File    = DIRcomponentConfig(In["config"]["guid"].asString());
   const bool      Success = FILsave(File, In["config"].json(1), &Error);
   (*pOut)["success"]      = Success;
   if(!Success) { (*pOut)["error"] = Error; }
}

void CMPCsaveConfig(const COLstring& Username, const CFGconfig& Config, bool SkipCommit, SCKloop* pLoop,
                    const COLaddress& Address) {
   COL_FUNCTION(CMPCsaveConfig);
   COLvar Data;
   Data["user"]        = Username;
   Data["skip_commit"] = SkipCommit;
   Data["config"]      = CFGconfigSerialize(Config);
   SCKinvokeWorker(pLoop, CMPCsaveConfigWorker, Data, COLnewClosure1(CMPCsaveCallback, Address));
}

bool CMPCisDevelopment(const COLstring& Commit) {
   return Commit == "DEVELOPMENT" || Commit.is_null();
}

static bool CMPCisRequireLine(const COLstring& Line, const COLstring& PatternSingle, const COLstring& PatternDouble) {
   return Line.find(PatternSingle) != npos || Line.find(PatternDouble) != npos;
}

static void CMPCremoveRequireFromContent(COLstring& Content, const COLstring& PatternSingle,
                                         const COLstring& PatternDouble) {
   COL_FUNCTION(CMPCremoveRequireFromContent);
   COLstring Buffer;
   size_t    Start = 0;
   size_t    End   = Content.find('\n');

   while(End != npos) {
      COLstring Line = Content.substr(Start, End - Start);
      if(!CMPCisRequireLine(Line, PatternSingle, PatternDouble)) { Buffer.append(Line).append("\n"); }
      Start = End + 1;
      End   = Content.find('\n', Start);
   }

   // Handle last line
   COLstring LastLine = Content.substr(Start);
   if(!CMPCisRequireLine(LastLine, PatternSingle, PatternDouble)) { Buffer.append(LastLine); }

   Content = Buffer;
}

static void CMPCprocessLuaFile(const COLstring& Path, const COLstring& PatternSingle, const COLstring& PatternDouble) {
   COL_FUNCTION(CMPCprocessLuaFile);
   try {
      COLstring Content;
      FILreadFile(Path, &Content);
      CMPCremoveRequireFromContent(Content, PatternSingle, PatternDouble);
      FILwriteFile(Path, Content);
   } catch(const COLerror& E) { COL_ERR("Error processing file " + Path + ": " + E.description()); }
}

void CMPCremoveRequireFromAllFiles(const COLstring& Dir, const COLstring& StringPattern) {
   COL_FUNCTION(CMPCremoveRequireFromAllFiles);
   COLstring PatternSingle = "require \'" + StringPattern;
   COLstring PatternDouble = "require \"" + StringPattern;
   if(StringPattern.find(".") == npos) {
      PatternSingle += "\'";
      PatternDouble += "\"";
   }
   COLstack<COLstring> DirsToProcess;
   DirsToProcess.push(Dir);
   try {
      while(!DirsToProcess.empty()) {
         COLstring CurrentDir = DirsToProcess.top();
         DirsToProcess.pop();

         FILdirEnumerator Enum(CurrentDir + FIL_DIR_SEPARATOR + FIL_ANYFILE);
         COLstring        File;
         while(Enum.getFile(File)) {
            const COLstring Path = FILpathAppend(CurrentDir, File);
            COL_VAR(Path);

            if(FILisDirectory(Path)) {
               DirsToProcess.push(Path);
            } else if(FILpathExtLast(Path, true) == ".lua") {
               CMPCprocessLuaFile(Path, PatternSingle, PatternDouble);
            }
         }
      }
   } catch(const COLerror& E) { COL_ERR(E.description()); }
}

bool CMPCsetErrorVar(COLvar* pOut, const COLstring& Error) {
   COL_FUNCTION(CMPCsetErrorVar);
   (*pOut)["error"]   = Error;
   (*pOut)["success"] = false;
   return false;
}

static bool CMPCpushNewBranch(const COLstring& Username, const COLstring& Dir, const COLstring& BranchName, COLstring* pError){
   COL_FUNCTION(CMPCpushNewBranch);
   // Bitbucket may not show a newly-pushed branch until it has its own commit.
   COLstring CommitErr;
   const COLstring CommitMsg = "Created branch '" + BranchName + "'.";
   if(!GITcommit(Username, Dir, "--allow-empty", CommitMsg, &CommitErr)) {
      if(pError) { *pError = CommitErr; }
      return false;
   }
   const COLstring Url = GITUrepositoryRemoteUrl(Dir);
   if(Url.is_null() || Url == "") { return true; }
   COLstring DefaultBranch = GITrepositoryDefaultBranch(Dir);
   COLstring Flags = "-u origin " + BranchName;
   COLstring Out;
   if(DefaultBranch != BranchName) {
      // Best-effort; keep behavior aligned with existing set branch flow.
      GITpush(Username, Dir, Flags, &Out, pError);
   }
   if(!GITpush(Username, Dir, "", &Out, pError)) { return false; }
   return true;
}

bool CMPCcheckoutOrCreateBranch(const COLstring& Username, const COLstring& Dir, const COLstring& BranchName,
                                bool* pCreated, COLstring* pError){
   COL_FUNCTION(CMPCcheckoutOrCreateBranch);
   if(pCreated) { *pCreated = false; }
   COLstring Error;
   if(GITbranchCheckoutExistingOrTrack(Dir, BranchName, &Error)) { return true; }
   if(!GITbranchCreateAndCheckout(Dir, BranchName, &Error)) {
      if(pError) { *pError = Error; }
      return false;
   }
   if(!CMPCpushNewBranch(Username, Dir, BranchName, &Error)) {
      if(pError) { *pError = Error; }
      return false;
   }
   if(pCreated) { *pCreated = true; }
   return true;
}

void CMPCcreateConfiguration(const COLstring& ComponentId, const COLstring& Name, const COLstring& Description,
                             const COLvar& TagVar, const COLstring& SourceId, const COLstring& Commit,
                             const COLstring& Repo, const bool PlainFolder, COLvar* pOut) {
   COL_FUNCTION(CMPCcreateConfiguration);
   COL_VAR4(ComponentId, Name, Description, SourceId);
   CFGconfig Comp;
   Comp.CoreConfig = CFGcore(ComponentId, Name, Description, TagVar, true);
   if(!SourceId.is_null()) { Comp.CoreConfig.Sources.push_back(SourceId); }
   Comp.CoreConfig.Commit   = Commit;
   Comp.CoreConfig.Repo     = Repo;
   Comp.CoreConfig.Template = Repo.is_null() ? false : !PlainFolder;  // blank component is never template
   const COLstring FileName = DIRcomponentConfig(ComponentId);
   const COLstring DirPath  = FILpathDir(FileName);
   if(!FILfileExists(DirPath)) {
      COL_TRC("Making " << DirPath);
      FILmakeFullDir(DirPath, FIL_USER_RWX);
   }
   COL_TRC("Saving " << FileName);
   COLstring Err;
   *pOut        = CFGconfigSerialize(Comp);
   bool Success = FILsave(FileName, pOut->json(1), &Err);
   COL_VAR2(Success, Err);
}

void CMPCstartComponentWithRun(const COLstring& ComponentId, BLUAinstanceManager* pManager, SCKloop* pLoop,
                               CFGmap* pMap) {
   COL_FUNCTION(CMPCstartComponentWithRun);
   CFGconfig* pConfig = CFGmapNode(pMap, ComponentId);
   if(!pConfig) { return; }
   for(const auto& it : pConfig->CoreConfig.Tags) {
      if(it == "#run") {
         SDBIlogComponent("IguanaX is automatically starting the component.", ComponentId, "#run");
         return BLUAinstanceStart(pManager, pConfig, pLoop);
      }
   }
}

// Shared helper for extracting value from config
COLstring CMPCgetRunConfigField(const COLstring& Component, const COLstring& Field) {
   COL_FUNCTION(CMPCgetRunConfigField);
   const COLstring ConfigFile = DIRcomponentConfig(Component);
   if(!FILfileExists(ConfigFile)) { return ""; }
   try {
      COLstring ConfigData;
      FILreadFile(ConfigFile, &ConfigData);
      COLvar ConfigVar;
      if(ConfigVar.parse(ConfigData)) { return ConfigVar[Field].asString(); }
   } catch(const COLerror& E) { COL_TRC(E.description()); }
   return "";
}

COLstring CMPCgetRunBranch(const COLstring& Component) {
   COL_FUNCTION(CMPCgetRunBranch);
   return CMPCgetRunConfigField(Component, "run_branch");
}

COLstring CMPCgetRunRepo(const COLstring& Component) {
   COL_FUNCTION(CMPCgetRunRepo);
   return CMPCgetRunConfigField(Component, "run_repo");
}

COLstring CMPCgetRunCommit(const COLstring& Component) {
   COL_FUNCTION(CMPCgetRunCommit);
   return CMPCgetRunConfigField(Component, "run_commit");
}
