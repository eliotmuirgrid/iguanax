// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCrunDir
//
// Description: Create a run directory for a component
//
// Author: Matthew Sobkowwski
// Date:   Tuesday August 8, 2023
// ---------------------------------------------------------------------------
#include <CMPC/CMPCrunDir.h>
#include <CMPC/CMPCutils.h>
#include <COL/COLlog.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GIT/GITcheckout.h>
#include <GIT/GITclone.h>
#include <GIT/GITpath.h>
#include <GIT/GITutils.h>
#include <GITU/GITUaddRemote.h>
#include <GITU/GITUmodules.h>
#include <SDB/SDBcapi.h>
COL_LOG_MODULE;

static bool CMPCremoveAndCreateDir(const COLstring& Dir, COLstring* pError) {
   COL_FUNCTION(CMPCremoveAndCreateDir);
   COLstring RemoveError;
   if(FILfileExists(Dir) && !FILremoveFullDirNewSafe(Dir, &RemoveError)) {
      *pError = "Failed to remove directory " + Dir + ". " + RemoveError;
      return false;
   }
   FILmakeFullDir(Dir, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   return true;
}

// if all else fails clone in the module directly
static bool CMPCcloneModuleFallback(const COLstring& Dir, const COLstring& Url, const COLstring& Commit,
                                    COLstring* pError) {
   COL_FUNCTION(CMPCcloneModuleFallback);
   COL_VAR3(Dir, Url, Commit);
   COLstring Error;
   if(FILfileExists(Dir) && !FILremoveFullDirNewSafe(Dir, &Error)) {
      COL_TRC("Failed to remove module directory: " + Dir + ". " + Error);
      return false;
   }
   bool      UseSsh = false;
   COLstring Out, Err;
   if(!GITinitializeCachedRepo("admin", Url, "", Dir, UseSsh, &Out, &Err)) {
      COL_TRC("Failed to clone module repository: " + Url);
      *pError = "Failed to clone module repository: " + Url + ". " + Err;
      return false;
   }
   if(!GITcheckout(Dir, Commit, pError)) {
      COL_TRC("Failed to checkout module commit: " + Commit);
      *pError = "Commit references a library commit not cached (" + Commit + ") at: " + GITgitTemplateDir(Url) + ". ";
      return false;
   }
   pError->clear();
   return true;
}

static bool CMPCcheckoutModules(const COLstring& ComponentId, const COLmap<COLstring, GITUmodule>& ModuleList,
                                COLstring* pError) {
   COL_FUNCTION(CMPCcheckoutModules);
   for(auto i = ModuleList.begin(); i != ModuleList.end(); ++i) {
      const COLstring RunModuleDir = GITrunModuleDir(ComponentId, i->first);
      if(!GITcheckout(RunModuleDir, i->second.CommitId, pError)) {
         if(!CMPCcloneModuleFallback(RunModuleDir, i->second.GitUrl, i->second.CommitId, pError)) {
            COL_TRC("Failed to clone module repository: " + i->second.GitUrl);
            return false;
         }
      }
   }
   return true;
}

static bool CMPCprocessModule(const COLstring &ComponentId, const COLstring &ModuleName, const GITUmodule &Module,
                              COLstring *pError){
   COL_FUNCTION(CMPCprocessModule);
   const COLstring Dest = GITrunModuleDir(ComponentId, ModuleName);
   const COLstring Source = GITgitTemplateDir(Module.GitUrl);
   if(!FILfileExists(Source)){
      COLstring RemoveError;
      if (FILfileExists(Dest) && !FILremoveFullDirNewSafe(Dest, &RemoveError)){
         *pError = "Failed to clear existing destination for clone: " + Dest + ". " + RemoveError;
         return false;
      }
      COLstring Out, Err;
      bool UseSsh = false;
      if(!GITinitializeCachedRepo("admin", Module.GitUrl, "", Dest, UseSsh, &Out, &Err)){
         *pError = "Failed to clone module repository: " + Module.GitUrl + ". " + Err;
         return false;
      }
   }
   else{
      FILcopyDirectoryContents(Source, Dest, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   }
   return GITUaddRemote(Dest, Module.GitUrl, pError);
}

static bool CMPChandleModules(const COLstring& ComponentId, const COLstring& RunDir, COLstring* pError) {
   COL_FUNCTION(CMPChandleModules);
   const COLstring ModulesFile = RunDir + ".gitmodules";
   if(!FILfileExists(ModulesFile)) { return true; }
   COLmap<COLstring, GITUmodule> ModuleList;
   GITUmoduleLoad(ModulesFile, &ModuleList);
   for(auto i = ModuleList.begin(); i != ModuleList.end(); ++i) {
      if(!CMPCprocessModule(ComponentId, i->first, i->second, pError)) { return false; }
   }
   GITUmoduleGetCommits(RunDir, "", "", &ModuleList);
   return CMPCcheckoutModules(ComponentId, ModuleList, pError);
}

static void CMPCcopySourceToScratch(const COLstring& Path, const COLstring& ScratchDir, bool IsGitRepo) {
   COL_FUNCTION(CMPCcopySourceToScratch);
   const COLstring SourceDir = IsGitRepo ? Path : DIRconfigFile("");
   FILcopyDirectoryContents(SourceDir, ScratchDir, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
}

static bool CMPCcheckoutCommit(const COLstring& ScratchDir, const COLstring& CommitId, COLstring* pError) {
   COL_FUNCTION(CMPCcheckoutCommit);
   if(!GITcheckout(ScratchDir, CommitId, pError)) {
      COL_TRC(*pError);
      *pError =
          "You have selected a commit where there is no source code for the component. You may be selecting a commit "
          "that exists before the component was created or the component was not a plain folder at this time.";
      return false;
   }
   return true;
}

static bool CMPCmoveOrCopyToRunDir(const COLstring& ScratchDir, const COLstring& RunDir, const COLstring& RunPath,
                                   const COLstring& ComponentId, bool IsGitRepo, COLstring* pError) {
   COL_FUNCTION(CMPCmoveOrCopyToRunDir);
   COL_VAR3(ScratchDir, RunDir, ComponentId);
   const int Mode = FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX;
   COLstring RemoveError;
   if(IsGitRepo) {
      if(FILfileExists(RunDir) && !FILremoveFullDirNewSafe(RunDir, &RemoveError)) {
         *pError = "Failed to remove run directory " + RunDir + ". " + RemoveError;
         return false;
      }
      FILrenameDirectory(ScratchDir, RunDir, Mode, true);
   } else {
      const COLstring CompScratchDir = FILpathAppend(ScratchDir, RunPath);
      if(!FILfileExists(CompScratchDir)) {
         FILremoveFullDirNewSafe(ScratchDir, &RemoveError);
         *pError = ComponentId + " not found in this commit";
         return false;
      }
      if(FILfileExists(RunDir) && !FILremoveFullDirNewSafe(RunDir, &RemoveError)) {
         FILremoveFullDirNewSafe(ScratchDir, &RemoveError);
         *pError = "Failed to remove run directory " + RunDir + ". " + RemoveError;
         return false;
      }
      FILmakeFullDir(RunDir, Mode);
      FILcopyDirectoryContents(CompScratchDir, RunDir, Mode);
      FILremoveFullDirNewSafe(ScratchDir, &RemoveError);
   }
   if(FILisDirectoryEmpty(DIRscratchDir(""))) {
      FILremoveFullDirNewSafe(DIRscratchDir(""), &RemoveError);
   }
   return true;
}

static bool CMPCprepareRunDir(const COLstring& ComponentId, const COLstring& SourceDir, const COLstring& CommitId,
                              const COLstring& RunPath, COLstring* pError) {
   COL_FUNCTION(CMPCprepareRunDir);
   const COLstring RunDir         = DIRcomponentRunDir(ComponentId);
   const COLstring CompScratchDir = DIRscratchDir(ComponentId);
   const bool      IsGitRepo      = FILfileExists(FILpathAppend(SourceDir, ".git"));
   if(!CMPCremoveAndCreateDir(CompScratchDir, pError)) {
      return false;
   }
   CMPCcopySourceToScratch(SourceDir, CompScratchDir, IsGitRepo);
   if(!CMPCcheckoutCommit(CompScratchDir, CommitId, pError)) {
      COLstring Error;
      FILremoveFullDirNewSafe(CompScratchDir, &Error);
      return false;
   }
   return CMPCmoveOrCopyToRunDir(CompScratchDir, RunDir, RunPath, ComponentId, IsGitRepo, pError);
}

static COLstring CMPCgetRunPath(const COLstring& ComponentId, const COLstring& RunComp) {
   COL_FUNCTION(CMPCgetRunPath);
   COLstring UseComp = RunComp.is_null() ? ComponentId : RunComp;
   COLstring RunPath = "comps/" + UseComp + "/dev";
   FILcorrectPathSeparators(RunPath);
   return RunPath;
}

COLstring CMPCgetRunDir(const COLstring& ComponentId, const COLstring& RunComp){
   COL_FUNCTION(CMPCgetRunDir);
   COLstring RelativePath = CMPCgetRunPath(ComponentId, RunComp);
   COLstring SourceDir    = RelativePath;
   if(SourceDir.is_null()) {
      SourceDir = DIRcomponentDevDir(ComponentId);
   } else {
      SourceDir = DIRcheckAboveRoot(DIRconfigFile(""), SourceDir);
   }
   return SourceDir;
}

bool CMPCrunDir(const COLstring& ComponentId, const COLstring& RunComp, const COLstring& CommitId, COLstring* pError) {
   COL_FUNCTION(CMPCrunDir);
   COL_VAR3(ComponentId, RunComp, CommitId);
   COLstring RelativePath = CMPCgetRunPath(ComponentId, RunComp);
   COLstring SourceDir    = RelativePath;
   if(SourceDir.is_null()) {
      SourceDir = DIRcomponentDevDir(ComponentId);
   } else {
      SourceDir = DIRcheckAboveRoot(DIRconfigFile(""), SourceDir);
      if(SourceDir.is_null()) {
         *pError = "Source directory is above the root directory";
         return false;
      }
   }
   COL_VAR(SourceDir);
   if(!CMPCprepareRunDir(ComponentId, SourceDir, CommitId, RelativePath, pError)) {
      COL_TRC("Error preparing run dir");
      return false;
   }
   const COLstring RunDir  = DIRcomponentRunDir(ComponentId);
   bool            Success = CMPChandleModules(ComponentId, RunDir, pError);
   GITcleanupAllGitDirs(RunDir, false);
   COL_VAR3(Success, RunDir, *pError);
   if(!Success) { SDBIlogService("Setting run commit for " + ComponentId + ": " + *pError, "#error"); }
   return Success;
}
