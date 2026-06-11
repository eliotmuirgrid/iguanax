//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPLaddLibrary.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   27/03/25 10:27 AM
//  ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigApi.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCutils.h>
#include <CMPL/CMPLaddLibrary.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GIT/GITadd.h>
#include <GIT/GITpath.h>
#include <GIT/GITutils.h>
#include <GITU/GITUcacheRepoFromUrl.h>
#include <GITU/GITUrepository.h>
#include <GITU/GITUutils.h>
#include <INI/INIdata.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

void CMPLmodifyModulesFile(const COLstring& ComponentId, const COLstring& LibraryName, const COLstring& GitUrl) {
   COL_FUNCTION(CMPLmodifyModulesFile);
   COL_VAR2(LibraryName, GitUrl);
   INIdata   GitModulesData;
   COLstring GitModules = DIRcomponentDevDir(ComponentId) + ".gitmodules";
   GITUreadGitFile(GitModules, &GitModulesData);
   INIdataKeySet(&GitModulesData, "submodule", LibraryName, "path", LibraryName);
   INIdataKeySet(&GitModulesData, "submodule", LibraryName, "url", GitUrl);
   GITUsaveGitFile(GitModules, GitModulesData);
}

static void CMPLaddToConfigFile(const COLstring& ComponentId, const COLstring& LibraryName, const COLstring& GitUrl) {
   COL_FUNCTION(CMPLaddToConfigFile);
   INIdata   ConfigData;
   COLstring FileName = DIRcomponentDevDir(ComponentId);
   FileName += ".git";
   FILaddPathSeparator(FileName);
   FileName += "config";
   COL_VAR2(ComponentId, FileName);
   GITUreadGitFile(FileName, &ConfigData);
   INIdataKeySet(&ConfigData, "submodule", LibraryName, "url", GitUrl);
   INIdataKeySet(&ConfigData, "submodule", LibraryName, "active", "true");
   COL_VAR2(LibraryName, GitUrl);
   GITUsaveGitFile(FileName, ConfigData);
}

static void CMPLaddRequireToMain(const COLstring& ProjectPath, const COLstring& Prefix) {
   COL_FUNCTION(CMPLaddRequireToMain);
   FILdirEnumerator Enum(ProjectPath + FIL_DIR_SEPARATOR + Prefix + FIL_DIR_SEPARATOR + "*");
   COLstring        FileName, PrependToFile, MainFile;
   try {
      FILreadFile(FILpathAppend(ProjectPath, "main.lua"), &MainFile);
      while(Enum.getFile(FileName)) {
         if(FILpathExtLast(FileName, false) == "lua") {
            COLstring RequireString =
                "require \"" + Prefix + "." + FILpathNameWithoutLastExt(FileName) + "\"" + FIL_NEWLINE_CHARACTER;
            COL_VAR(RequireString);
            if(!COLstringContains(MainFile, RequireString)) PrependToFile += RequireString;
         }
      }
      PrependToFile += MainFile;
      FILwriteFile(FILpathAppend(ProjectPath, "main.lua"), PrependToFile);
   } catch(const COLerror& Error) { COL_VAR(Error.description()); }
}

static bool CMPLcheckExistingLibrary(const COLstring& LibDir, const COLstring& LibraryName, COLaddress Address,
                                     SCKloop* pLoop, COLclosure0* pCallback) {
   COL_FUNCTION(CMPLcheckExistingLibrary);
   COLstring File = LibDir;
   FILremovePathSeparator(File);
   if(FILfileExists(LibDir) || FILfileExists(File)) {
      COLrespondError(Address, "A folder or file already exists with the name: " + LibraryName);
      pLoop->post(pCallback);
      return false;
   }
   return true;
}

static bool CMPLobtainLibrarySource(const COLstring& Username, const COLstring& GitUrl, COLstring& Source,
                                    COLaddress Address, SCKloop* pLoop, COLclosure0* pCallback) {
   COL_FUNCTION(CMPLobtainLibrarySource);
   COLstring Output;
   if(!FILfileExists(Source)) {
      COL_TRC(Source << " not found in cache -- try to create");
      if(!GITUcacheRepoFromUrl(Username, GitUrl, "", &Output)) {
         COLrespondError(Address, "Failed to clone " + GitUrl + " to cache. " + Output);
         pLoop->post(pCallback);
         return false;
      }
   }
   if(!FILfileExists(Source) && !GITUcacheRepoFromUrl(Username, GitUrl, "", &Output)) {
      COLrespondError(Address, "Could not find library in the cache at: " + GitUrl);
      pLoop->post(pCallback);
      return false;
   }
   return true;
}

static void CMPLcopyLibraryContents(const COLstring& Source, const COLstring& LibDir) {
   COL_FUNCTION(CMPLcopyLibraryContents);
   FILmakeFullDir(LibDir, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   FILcopyDirectoryContents(Source, LibDir, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
}

static void CMPLsetupGitSubmodule(const COLstring& Component, const COLstring& LibraryName, const COLstring& LibDir,
                                  const COLstring& GitUrl) {
   COL_FUNCTION(CMPLsetupGitSubmodule);
   COLstring UpstreamRepo;
   if(!GITUrepository(LibDir, "origin", &UpstreamRepo)) { UpstreamRepo = GitUrl; }
   CMPLmodifyModulesFile(Component, LibraryName, UpstreamRepo);
   CMPLaddToConfigFile(Component, LibraryName, UpstreamRepo);
}

static void CMPLhandleGitConfiguration(bool Plain, const COLstring& LibDir, const COLstring& Component,
                                       const COLstring& LibraryName, const COLstring& GitUrl, COLstring& AddFiles) {
   COL_FUNCTION(CMPLhandleGitConfiguration);
   if(Plain) {
      const COLstring LibGitDir = FILpathAppend(LibDir, ".git");
      if(FILfileExists(LibGitDir)) {
         COL_TRC("Removing existing git directory for " + LibraryName);
         GITcleanUpGitDir(LibDir, false);
      }
   } else {
      CMPLsetupGitSubmodule(Component, LibraryName, LibDir, GitUrl);
      AddFiles += " .gitmodules";
   }
}

static void CMPLaddLibraryImpl(COLvar Data, COLaddress Address, SCKloop* pLoop, COLclosure0* pCallback) {
   COL_FUNCTION(CMPLaddLibraryImpl);
   const COLstring Component   = Data["component"];
   const COLstring LibraryName = Data["name"];
   const COLstring GitUrl      = Data["url"];
   const COLstring Username    = Data["username"];
   const bool      Plain       = Data["plain"];
   const COLstring DevDir      = DIRcomponentDevDir(Component);
   const COLstring LibDir      = FILpathAppend(DevDir, LibraryName);

   if(!FILfileExists(DevDir + ".git") && !Plain) {
      COLrespondError(Address, "Cannot add library to a plain component. Add the library as a plain folder instead.");
      return pLoop->post(pCallback);
   }

   if(!CMPLcheckExistingLibrary(LibDir, LibraryName, Address, pLoop, pCallback)) return;

   COLstring Source = GITgitTemplateDir(GitUrl);
   if(!CMPLobtainLibrarySource(Username, GitUrl, Source, Address, pLoop, pCallback)) return;

   CMPLcopyLibraryContents(Source, LibDir);

   COLstring AddFiles = "\"" + LibraryName + "\"";
   CMPLhandleGitConfiguration(Plain, LibDir, Component, LibraryName, GitUrl, AddFiles);

   CMPLaddRequireToMain(DevDir, LibraryName);
   COLrespondSuccess(Address);
   pLoop->post(pCallback);
}

// /component/library/add
void CMPLaddLibrary(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(CMPLaddLibrary);
   COL_VAR(Request.Data.json(1));
   COLstring ComponentId, Error;
   const bool      Dev       = COLvarRequiredBool(Request.Data, "development");
   if(!Dev) { return COLrespondError(Request.Address, "Cannot modify file structure of a run repository."); }
   if (!CFGcomponentId(Request.Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::EDIT_SCRIPT, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   COLvar Data;
   Data["component"] = ComponentId;
   Data["name"]      = COLvarRequiredString(Request.Data, "name");
   Data["url"]       = COLvarRequiredString(Request.Data, "git");
   Data["plain"]     = COLvarGetBool(Request.Data, "plain", false);
   Data["username"]  = Request.User;
   if(!GITUvalidLibraryName(Data["name"], &Error)) { return COLrespondError(Request.Address, Error); }
   pLoop->threadPool()->scheduleTask(
       COLnewClosure0(&CMPLaddLibraryImpl, Data, Request.Address, pLoop, CMPCcallback("/component/library/add")));
}