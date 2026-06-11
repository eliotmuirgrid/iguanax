//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINSutils.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   17/09/24 1:22 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GINS/GINSutils.h>
#include <GIT/GITbranch.h>
#include <GIT/GITexecute.h>
#include <GIT/GITfetch.h>
#include <GIT/GITgenerateKeyPair.h>
#include <GIT/GITpull.h>
#include <GITU/GITUcache.h>
#include <GITU/GITUlistLoad.h>
#include <GITU/GITUmodules.h>
#include <GITU/GITUrepository.h>
#include <GITU/GITUtemplate.h>
#include <GITU/GITUutils.h>
COL_LOG_MODULE;

void GINSchangeKeyPermissions() {
   COL_FUNCTION(GINSchangeKeyPermissions);
   COLstring        UsersDir = DIRusersDir() + FIL_ANYFILE;
   COLstring        FileName;
   FILdirEnumerator Enum(UsersDir, true);
   while(Enum.getFile(FileName)) {  // have to fix key permissions for all users
      COLstring CurrentUserDir = FILpathAppend(DIRusersDir(), FileName);
      COLstring PrivateKeyPath = FILpathAppend(CurrentUserDir, "privatekey");
      COL_VAR(PrivateKeyPath);
      if(FILfileExists(PrivateKeyPath)) { FILsetPermission(PrivateKeyPath, 0600); }
   }
}

void GINSsetError(const COLstring& Error, COLvar* pOut) {
	(*pOut)["success"] = false;
	(*pOut)["error"]	 = Error;
}

bool GINSloadAndParse(COLvar* pConfig, COLstring* pErr) {
	COL_FUNCTION(GINSloadAndParse);
	const COLstring File = DIRfileConfigInstance();
	if(!FILfileExists(File)) { COL_TRC(File << " not found"); return true; }
	COLstring Data;
	try {
		FILreadFile(File, &Data);
		if(pConfig->parse(Data)) { return true; }
		*pErr = "Failed to parse instance config";
	} catch(const COLerror& E) { *pErr = E.description(); }
	COL_TRC(*pErr << ": " << Data);
	return false;
}

bool GINSstatusClean(const COLstring& Dir, COLstring* pOut, COLstring* pErr) {
	COL_FUNCTION(GINSstatusClean);
	if(!FILfileExists(Dir)) { (*pErr) = Dir + " not found"; return false; }
	const int ExitCode = GITexecuteString(Dir, "git status --porcelain", pOut, pErr);
	COL_VAR3(ExitCode, *pOut, *pErr);
	return ExitCode == 0 && pOut->is_null();
}

bool GINShashExists(const COLstring& Dir, const COLstring& Hash, COLstring* pOut, COLstring* pErr) {
	COL_FUNCTION(GINShashExists);
	if(!FILfileExists(Dir)) { (*pErr) = Dir + " not found"; return false; }
	const int ExitCode = GITexecuteString(Dir, "git cat-file -e " + Hash, pOut, pErr);
	COL_VAR3(ExitCode, *pOut, *pErr);
	return ExitCode == 0;
}

static COLmap<COLstring, GITUmodule> GINSrelevantSubmodules(const COLmap<COLstring, GITUmodule>& AllComponents, const COLstring& RelPath){
   COL_FUNCTION(GINSrelevantSubmodules);
   COLmap<COLstring, GITUmodule> ComponentModules;
   const COLstring PosixRelPath = FILpathSepToPosix(RelPath);
   for(auto i = AllComponents.begin(); i != AllComponents.end(); ++i) {
      // .gitmodules always stores paths with forward slashes, even on Windows.
      if(FILpathSepToPosix(i->first).substr(0, PosixRelPath.size()) == PosixRelPath) {
         ComponentModules.add(i->first, i->second);
      }
   }
   return ComponentModules;
}

static void GINScacheHistoricalLibraries(const COLstring& RepoDir, const COLstring& Username,
                                         COLmap<COLstring, bool>* pSeenRepos, COLstring* pError) {
   COL_FUNCTION(GINScacheHistoricalLibraries);
   if(Username.isWhitespace()) { return; }
   const COLstring GitModules = GITUsubmoduleFileInRepo(RepoDir);
   if(!FILfileExists(GitModules)) { return; }
   const COLstring SourceGitUrl = GITUrepositoryRemoteUrl(RepoDir);
   if(SourceGitUrl.is_null()) {
      COL_TRC("Could not determine upstream repo for " << RepoDir);
      return;
   }
   GITUlist RepoList;
   if(!GITUlistLoad(RepoDir, SourceGitUrl, &RepoList) || RepoList.List.size() == 0) { return; }
   GITUlist FilteredList;
   for(const auto& Repo : RepoList.List) {
      const COLstring Key = GITUresolveKeyFromBranchUrl(Repo.GitUrl, Repo.Branch);
      if(pSeenRepos->count(Key) > 0) { continue; }
      pSeenRepos->add(Key, true);
      FilteredList.List.push_back(Repo);
   }
   if(FilteredList.List.size() == 0) { return; }
   COLstring Log;
   bool Success = false;
   GITUcloneList(Username, FilteredList, &Log, &Success);
   if(!Success) {
      COL_TRC(Log);
      if(pError->is_null()) {
         *pError = "Failed to cache component libraries. Check service logs for details.";
      }
   }
}

static bool GINSisComponentDevPath(const COLstring& Path) {
   if(Path.size() < 9) { return false; }
   if(Path.substr(0, 6) != "comps/") { return false; }
   return Path.substr(Path.size() - 4) == "/dev";
}

void GINSpullComponentRepos(const COLstring& Dir, const COLstring& Username, COLstring* pError) {
   COL_FUNCTION(GINSpullComponentRepos);
   const COLstring SubmoduleFile = DIRconfigFile(".gitmodules");
   if(!FILfileExists(SubmoduleFile)) { return; }
   COLmap<COLstring, GITUmodule> ModuleList;
   GITUmoduleLoad(SubmoduleFile, &ModuleList);
   for(auto i = ModuleList.begin(); i != ModuleList.end(); ++i) {
      const COLstring& Path = i->first;
      const COLstring& Branch = i->second.Branch;
      if(!GINSisComponentDevPath(Path) || Branch.isWhitespace()) { continue; }
      const COLstring CompDevDir = FILpathAppend(Dir, Path);
      COLstring Out, Err;
      COL_TRC("Fetching component repo: " << CompDevDir);
      if(!GITfetch(Username, CompDevDir, "", &Out, &Err)) {
         COL_TRC("Failed to fetch component repo " << CompDevDir << ": " << Err);
         if(pError->is_null()) { *pError = "Failed to fetch certain component repos. Check service logs for details."; }
         continue;
      }
      if(!GITbranchCheckoutExistingOrTrack(CompDevDir, Branch, &Err)) {
         COL_TRC("Failed to checkout branch " << Branch << " for " << CompDevDir << ": " << Err);
         if(pError->is_null()) { *pError = "Failed to checkout branches for certain components. Check service logs for details."; }
         continue;
      }
      if(!GITpull(Username, CompDevDir, &Out, &Err)) {
         COL_TRC("Failed to pull component repo " << CompDevDir << ": " << Err);
         if(pError->is_null()) { *pError = "Failed to pull certain component repos. Check service logs for details."; }
         continue;
      }
      GITUintializeSubmodulesSubmodules(CompDevDir, "", pError);
   }
}

void GINSinitializeSubmodules(const COLstring& Dir, COLstring* pError) {
   GINSinitializeSubmodules(Dir, "", pError);
}

void GINSinitializeSubmodules(const COLstring& Dir, const COLstring& Username, COLstring* pError) {
   COL_FUNCTION(GINSinitializeSubmodules);
   const COLstring SubmoduleFile = DIRconfigFile(".gitmodules");
   if(!FILfileExists(SubmoduleFile)) { return; }
   COLmap<COLstring, GITUmodule> ModuleList;
   COLmap<COLstring, bool>      SeenHistoricalRepos;
   GITUmoduleLoad(SubmoduleFile, &ModuleList);
   GITUinitSubmoduleAndUpstream(Dir, ModuleList, pError);
   const COLstring Folder = DIRconfigFile("comps/");
   FILdirEnumerator DirEnumerator(Folder + "*", true, true);
   COLstring CompId;
   COL_TRC("Beginning Directory enueration of " << Folder << " " << COLintToString(time(NULL)));
   while (DirEnumerator.getFile(CompId)) {
      COLstring CompDir = FILpathAppend(Folder, CompId);
      COL_TRC("starting component " << CompId << " " << COLintToString(time(NULL)));
      COLstring RelPath = "comps/" + CompId + "/";
      COL_VAR(RelPath);
      FILcorrectPathSeparators(RelPath);
      // Only process the current component's entries from the instance repo.
      // Running GITUmoduleGetCommits against every entry on every iteration becomes O(N^2).
      COLmap<COLstring, GITUmodule> ComponentModules = GINSrelevantSubmodules(ModuleList, RelPath);
      if(ComponentModules.empty()) {
         COL_DBG("No submodules found for " << RelPath);
      } else {
         GITUmoduleGetCommits(CompDir, RelPath, "", &ComponentModules);
         GITUcheckoutSubmoduleCommits(Dir, ComponentModules, pError);
      }
      COLstring CacheDir = Dir + RelPath + "dev/";
      FILcorrectPathSeparators(CacheDir);
      if(FILfileExists(CacheDir)) {
         GINScacheHistoricalLibraries(CacheDir, Username, &SeenHistoricalRepos, pError);
      } else {
         CacheDir = Dir + RelPath + "run/";
         FILcorrectPathSeparators(CacheDir);
         if(FILfileExists(CacheDir)) {
            GINScacheHistoricalLibraries(CacheDir, Username, &SeenHistoricalRepos, pError);
         }
      }
      COLstring SubDir = Dir + RelPath + "run/";
      FILcorrectPathSeparators(SubDir);
      COL_VAR(SubDir);
      if(FILfileExists(SubDir)){
         GITUintializeSubmodulesSubmodules(SubDir, RelPath, pError);
      }
      SubDir = Dir + RelPath + "dev/";
      FILcorrectPathSeparators(SubDir);
      COL_VAR(SubDir);
      if(FILfileExists(SubDir)){
         GITUintializeSubmodulesSubmodules(SubDir, RelPath, pError);
      }
      COL_TRC("next component");
   }
}
