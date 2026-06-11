// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSinitialize
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Tuesday 10 September 2024 - 08:59AM
// ---------------------------------------------------------------------------
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GINI/GINIdefault.h>
#include <GINS/GINSaddAndCommit.h>
#include <GINS/GINSinitialize.h>
#include <GINS/GINSreinitialize.h>
#include <GIT/GITadd.h>
#include <GIT/GITbranch.h>
#include <GIT/GITcommit.h>
#include <GIT/GITinit.h>
#include <GIT/GITutils.h>
#include <GITU/GITUconfig.h>
#include <GITU/GITUrepository.h>
#include <GITU/GITUutils.h>
#include <INI/INIdata.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void GINSremoveSubmodulesFromConfigFile(){
   COL_FUNCTION(GINSremoveSubmodulesFromConfigFile);
   INIdata ConfigData;
   const COLstring FileName = DIRconfigFile(".git/config");
   GITUreadGitFile(FileName, &ConfigData);
   COLstring FileContent;
	INIformat(ConfigData, &FileContent);
   INIdata ConfigDataCopy = ConfigData;
   for(auto i = ConfigDataCopy.PrivateData.cbegin(); i != ConfigDataCopy.PrivateData.cend(); i++){
      COLstring Key = i->first;
      if(Key.find("submodule") != npos){
         COL_TRC("Found a submodule: " << Key);
         ConfigData.PrivateData.erase(Key);
      }
   }
   GITUsaveGitFile(FileName, ConfigData);
}

static void GINSupdateGitModulesFile(const COLstring& Key, const COLstring& UpstreamRepo, const COLstring& Branch) {
   COL_FUNCTION(GINSupdateGitModulesFile);
   INIdata ConfigData;
   const COLstring GitModules = DIRconfigFile(".gitmodules");
   GITUreadGitFile(GitModules, &ConfigData);
   INIdataKeySet(&ConfigData, "submodule", Key, "path", Key);
   if(!UpstreamRepo.is_null()) { INIdataKeySet(&ConfigData, "submodule", Key, "url",  UpstreamRepo); }
   else                        { INIdataKeyRemove(&ConfigData, "submodule", Key, "url"); }
   if(!Branch.is_null())       { INIdataKeySet(&ConfigData, "submodule", Key, "branch", Branch); }
   else                        { INIdataKeyRemove(&ConfigData, "submodule", Key, "branch"); }
   GITUsaveGitFile(GitModules, ConfigData);
}

static void GINSupdateGitConfigFile(const COLstring& Key, const COLstring& UpstreamRepo) {
   COL_FUNCTION(GINSupdateGitConfigFile);
   INIdata ConfigData;
   const COLstring FileName = DIRconfigFile(".git/config");
   COL_VAR2(Key, FileName);
   GITUreadGitFile(FileName, &ConfigData);
   INIdataKeySet(&ConfigData, "submodule", Key, "active", "true");
   if(!UpstreamRepo.is_null()) { INIdataKeySet(&ConfigData, "submodule", Key, "url",  UpstreamRepo); }
   else                        { INIdataKeyRemove(&ConfigData, "submodule", Key, "url"); }
   GITUsaveGitFile(FileName, ConfigData);
}

static void GINSupdateSubModules(const COLstring& Key, const COLstring& Dir) {
   COL_FUNCTION(GINSupdateSubModules);
   COLstring UpstreamRepo;
   if(!GITUrepository(Dir, "origin", &UpstreamRepo)) {
      COL_TRC("Skipping upstream URL for " << Dir << " because origin could not be determined");
      UpstreamRepo = "";
   }
   const COLstring Branch = GITbranchCurrentDev(Dir);
   COL_VAR2(UpstreamRepo, Branch);
   GINSupdateGitModulesFile(Key, UpstreamRepo, Branch);
   GINSupdateGitConfigFile(Key, UpstreamRepo);
}

static void GINSremoveFromConfigFile(const COLstring& Key) {
   COL_FUNCTION(GINSremoveFromConfigFile);
   INIdata         ConfigData;
   const COLstring FileName = DIRconfigFile(".git/config");
   COL_VAR(FileName);
   GITUreadGitFile(FileName, &ConfigData);
   GITUconfigSubmoduleClear(&ConfigData, Key);
   GITUsaveGitFile(FileName, ConfigData);
}

static void GINSremoveGitModules(const COLstring& Key){
   COL_FUNCTION(GINSremoveGitModules);
   INIdata ConfigData;
   const COLstring GitModules = DIRconfigFile(".gitmodules");
   COL_VAR(GitModules);
   GITUreadGitFile(GitModules, &ConfigData);
   GITUconfigSubmoduleClear(&ConfigData, Key);
   GITUsaveGitFile(GitModules, ConfigData);
}

void GINSremoveComponent(const COLstring& CompId) {
   COL_FUNCTION(GINSremoveCompWhichDir);
   const COLstring Key = FILpathSepToPosix(FILpathAppend("comps", CompId + "/" + "dev"));
   GINSremoveGitModules(Key);
   GINSremoveFromConfigFile(Key);
}

void GINSremoveComponentList(const COLarray<COLstring>& CompList) {
   COL_FUNCTION(GINSremoveComponentList);
   const COLstring GitModules = DIRconfigFile(".gitmodules");
   const COLstring GitConfig  = DIRconfigFile(".git/config");
   INIdata         ModuleData;
   INIdata         ConfigData;
   GITUreadGitFile(GitModules, &ModuleData);
   GITUreadGitFile(GitConfig, &ConfigData);
   for(const auto& comp : CompList) {
      const COLstring Key = FILpathSepToPosix(FILpathAppend("comps", comp + "/" + "dev"));
      GITUconfigSubmoduleClear(&ModuleData, Key);
      GITUconfigSubmoduleClear(&ConfigData, Key);
   }
   GITUsaveGitFile(GitModules, ModuleData);
   GITUsaveGitFile(GitConfig, ConfigData);
}

void GINSaddComponent(const COLstring& CompId) {
   COL_FUNCTION(GINSaddComponent);
   COL_TRC("Adding component: " << CompId);
   const COLstring Dir = DIRcomponentDevDir(CompId);
   const COLstring Key = FILpathSepToPosix(FILpathAppend("comps", CompId + "/" + "dev"));
   GINSupdateSubModules(Key, Dir);
}

static void GINSmakeIniChangeMT(INIdata* pModuleData, INIdata* pConfigData, COLstring Key, COLstring Branch, COLstring UpstreamRepo, COLclosure0* pDecrementCallback){
   COL_FUNCTION(GINSmakeIniChangeMT);
   INIdataKeySet(pModuleData, "submodule", Key, "path", Key);
   INIdataKeySet(pConfigData, "submodule", Key, "active", "true");
   if(!Branch.is_null())       { INIdataKeySet(pModuleData, "submodule", Key, "branch", Branch); }
   else                        { INIdataKeyRemove(pModuleData, "submodule", Key, "branch"); }
   if(UpstreamRepo.is_null()) {
      INIdataKeyRemove(pModuleData, "submodule", Key, "url");
      INIdataKeyRemove(pConfigData, "submodule", Key, "url");
   } else {
      INIdataKeySet(pModuleData, "submodule", Key, "url", UpstreamRepo);
      INIdataKeySet(pConfigData, "submodule", Key, "url", UpstreamRepo);
   }
   pDecrementCallback->runAndDelete();
}

static void GINSchangeIniEntry(COLstring comp, INIdata* pModuleData, INIdata* pConfigData, SCKloop* pLoop, COLclosure0* pDecrementCallback){
   COL_FUNCTION(GINSchangeIniEntry);
   const COLstring Dir = DIRcomponentDevDir(comp);
   const COLstring Key = FILpathSepToPosix(FILpathAppend("comps", comp + "/" + "dev"));
   COLstring       UpstreamRepo;
   if(!GITUrepository(Dir, "origin", &UpstreamRepo)) {
      COL_TRC("Skipping upstream URL for " << Dir << " because origin could not be determined");
      UpstreamRepo = "";
   }
   const COLstring Branch = GITbranchCurrentDev(Dir);
   COL_VAR4(Dir, Key, UpstreamRepo, Branch);
   pLoop->post(COLnewClosure0(&GINSmakeIniChangeMT, pModuleData, pConfigData, Key, Branch, UpstreamRepo, pDecrementCallback));
}

struct GINSchecker{
   GINSchecker(int Count) { m_Count = Count; }
   int m_Count = 0;
};

static void GINStrackerDecrementCount(GINSchecker* pTracker, SCKloop* pLoop){
   COL_FUNCTION(GINStrackerDecrementCount);
   pTracker->m_Count--;
   COL_TRC("GINStracker jobs remaining " + COLintToString(pTracker->m_Count));
   if (pTracker->m_Count <= 0 && pLoop) {
      COL_TRC("No more components to go through, cleaning up tracker");
      delete pTracker;
      pLoop->shutdown();
   }
}

void GINSaddComponentList(const COLarray<COLstring>& CompList) {
   COL_FUNCTION(GINSaddComponentList);
   COLthreadPool Pool(1,5);
   SCKloop Loop(&Pool);
   Pool.start();
   const COLstring GitModules = DIRconfigFile(".gitmodules");
   const COLstring GitConfig  = DIRconfigFile(".git/config");
   INIdata         ModuleData;
   INIdata         ConfigData;
   GITUreadGitFile(GitModules, &ModuleData);
   GITUreadGitFile(GitConfig, &ConfigData);
   GINSchecker* pTracker = new GINSchecker(CompList.size());
   COL_TRC("GINStracker jobs set to " + COLintToString(pTracker->m_Count));
   for(const auto& comp : CompList) {
      COLclosure0* pDecrementCallback = COLnewClosure0(&GINStrackerDecrementCount, pTracker, &Loop);
      Loop.threadPool()->scheduleTask(COLnewClosure0(&GINSchangeIniEntry, comp, &ModuleData, &ConfigData, &Loop, pDecrementCallback));
   }
   if(CompList.size() > 0) { Loop.start(); } // This loop gets shutdown in GINStrackerDecrementCount
   else                    { delete pTracker; }
   GITUsaveGitFile(GitModules, ModuleData);
   GITUsaveGitFile(GitConfig, ConfigData);
}

void GINSinitializeComponents() {
   COL_FUNCTION(GINSinitializeComponents);
   const COLstring Folder = DIRconfigFile("comps/");
   FILdirEnumerator DirEnumerator(Folder + "*", true, true);
   COLarray<COLstring> Components;
   COLstring CompId;
   while (DirEnumerator.getFile(CompId)) { if(!GITisPlainFolder(CompId)) { Components.push_back(CompId); } }
   GINSaddComponentList(Components);
}

void GINSaddFiles(){
   COL_FUNCTION(GINSaddFiles);
   COLstring Error;
   GINSaddSettings(&Error);
   GINSaddReports(&Error);
   GINSaddUsers(&Error);
   GINSaddOther(&Error);
}

void GINSinitialize(){
   COL_FUNCTION(GINSinitialize);
	const COLstring Dir = FILpathAppend(FILworkingDir(), "config");
   if(!FILfileExists(Dir)){
      COL_TRC("Creating config directory");
      FILmakeFullDir(Dir, FIL_USER_RWX);
   }
   COLstring GitDir = DIRconfigFile(".git");
   COLstring Error;
   if(!FILfileExists(GitDir) && !GITinit("", Dir, true, &Error)){
      COL_TRC("Failed to initialize git repository");
      COL_TRC(Error);
      return;
   }
   // Make sure the .gitmodules and config files are up to date. Users could have deleted component folders.
   GINSremoveGitModulesFile(&Error);
   GINSremoveSubmodulesFromConfigFile();
   GINSinitializeComponents();
   GINSaddFiles();
	if(GITcommit("", Dir, "", "Detected changes made to Iguana during startup.", &Error)) {
		COL_TRC("Successfully added .gitmodules to git");
	} else {
		COL_TRC("Failed to add .gitmodules to git");
		COL_TRC(Error);
	}  
   GINIdefault(&Error, !FILfileExists(DIRconfigFile(".gitignore")));
}
