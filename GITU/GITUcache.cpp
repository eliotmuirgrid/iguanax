//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITUcache.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   03/10/24 2:36 PM
//  ---------------------------------------------------------------------------
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GIT/GITadd.h>
#include <GIT/GITclone.h>
#include <GIT/GITcommit.h>
#include <GIT/GITinit.h>
#include <GIT/GITpath.h>
#include <GIT/GITpull.h>
#include <GITU/GITUcache.h>
#include <GITU/GITUlist.h>
#include <GITU/GITUlistLoad.h>
#include <GITU/GITUrepository.h>
#include <GITU/GITUurl.h>
#include <GITU/GITUutils.h>
#include <LINK/LINKconstants.h>
#include <SCK/SCKcall.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
COL_LOG_MODULE;

void GITUcacheLog(const COLstring& Log, const COLstring& Tags){
   COL_FUNCTION(GITUcacheLog);
   COL_VAR2(Log, Tags);
   SDBIlogService(Log + "\n", Tags);
}

struct GITUcloneResult{
   bool Success = false;
   bool Pull = false;
   bool Ssh = false;
   COLstring Output;
};

class GITUcloneState{
public:
   bool Success=true;
   COLmap<COLstring, bool> List;
   COLmap<COLstring, GITUcloneResult> Results;
   int Count=0;
   COLstring Log;
   COLstring Username;
   COLclosure2<bool, COLstring>* pFinished;
   SCKloop* pLoop;
};
static void GITUaddList(GITUcloneState* pState, const GITUlist& List);

static COLstring GITUlogLinkUrl(const COLstring& Link, bool IsSsh) {
   if(IsSsh) { return Link; }
   bool      IsHttps = Link.substr(0, 5) == "https";
   COLstring LinkUrl;
   GITUwebUrl(Link, &LinkUrl);
   return LinkUrl;
}

static void GITUlogCloneSummary(const COLmap<COLstring, GITUcloneResult>& Results) {
   COLstring  Summary;
   COLostream Stream(Summary);
   Stream << "Git cache summary:" << newline;
   for(auto const& i : Results) {
      if(!i->second.Success) { continue; }
      COLstring LinkUrl = GITUlogLinkUrl(i->first, i->second.Ssh);
      if(i->second.Pull) {
         Stream << " - Successfully pulled repo " << LinkUrl << newline;
      } else {
         Stream << " - Successfully cloned repo " << LinkUrl << (i->second.Ssh ? " via SSH." : " via HTTPS.") << newline;
      }
   }
   GITUcacheLog(Summary, "#git #endpoint #summary");
}

static void GITUlogCloneError(const COLstring& GitUrl, const GITUcloneResult& Result) {
   COLstring  Error;
   COLostream Stream(Error);
   COLstring  LinkUrl = GITUlogLinkUrl(GitUrl, Result.Ssh);
   if(Result.Pull) { Stream << "Failed to pull repo " << GitUrl << newline; }
   else            { Stream << "Failed to clone repo " << GitUrl << (Result.Ssh ? " via SSH." : " via HTTPS.") << newline; }
   Stream << Result.Output << newline;
   GITUcacheLog(Error, "#error #git #endpoint");
}

static void GITUcloneDone(bool Success, COLstring GitUrl, GITUcloneResult Result, GITUcloneState* pState){
   COL_FUNCTION(GITUcloneDone);
   // We only manipulate pState on the main thread
   COL_VAR3(Success, GitUrl, Result.Output);
   if (!Success){
      pState->Success = false;
      GITUlogCloneError(GitUrl, Result);
   }
   pState->List[GitUrl] = true;
   pState->Results[GitUrl] = Result;
   pState->Count--;
   COL_VAR(pState->Count);
   pState->Log += Result.Output;
   COLstring Dir = GITgitTemplateDir(GitUrl);
   COL_VAR(Dir);
   GITUlist List;
   if (GITUlistLoad(Dir, GitUrl, &List)){
      GITUaddList(pState, List);
   }
   if (pState->Count == 0){
      COL_TRC("Done!");
      GITUlogCloneSummary(pState->Results);
      pState->pFinished->runAndDelete(pState->Success, pState->Log);
      delete pState;
   }
}

static void GITUcloneOnThreadReportError(COLstring* pOut, const COLstring& Error, const COLstring& GitUrl){
   COL_FUNCTION(GITUcloneOnThreadReportError);
   COL_VAR(Error);
   *pOut = Error + COL_T(" Git url: ") + GitUrl;
}

// WORKER THREAD
static void GITUcloneOnThread(COLstring GitUrl, COLstring Branch, COLstring Username, SCKloop* pLoop, COLclosure3<bool, COLstring, GITUcloneResult>* pCallback){
   COL_FUNCTION(GITUcloneOnThread);
   COL_VAR2(GitUrl, Username);
   GITUcloneResult Result;
   // TODO - we may need to use GITUparseIsAzure instead of GITUisAzureSshLocal.
   if(GitUrl.rfind(".git") != GitUrl.size() - 4 && !GITUisAzureSshLocal(GitUrl)) {
      Result.Success = false;
      Result.Output  = "Please verify that the provided path is a valid Git clone URL (e.g., ends in .git).";
      return pLoop->post(COLnewClosure0(pCallback, &COLclosure3<bool, COLstring, GITUcloneResult>::runAndDelete, Result.Success, GitUrl, Result));
   }
   try{
      COLstring Error;
      const COLstring RepoDir = GITgitTemplateDir(GitUrl, Branch);
      if (FILfileExists(RepoDir)){
         COL_TRC("GIT pull " << GitUrl << " into " << RepoDir);
         Result.Pull    = true;
         Result.Success = GITpull(Username, RepoDir, &Result.Output, &Error);
         Result.Output  = GitUrl + " refreshed.\n";
      } else{
         COL_TRC("GIT clone " << GitUrl << " into " << RepoDir);
         Result.Pull    = false;
         Result.Ssh     = false;
         Result.Success = GITinitializeCachedRepo(Username, GitUrl, Branch, RepoDir, Result.Ssh, &Result.Output, &Error);
         Result.Output  = GitUrl + " cloned.\n";
      }
      if (!Result.Success) { GITUcloneOnThreadReportError(&Result.Output, Error, GitUrl); }
   } catch(const COLerror& Error){
      COL_ERR(Error);
      Result.Success = false;
      Result.Output = Error.description();
   }
   pLoop->post(COLnewClosure0(pCallback, &COLclosure3<bool, COLstring, GITUcloneResult>::runAndDelete, Result.Success, GitUrl, Result));
   COL_CRASH_HERE_IF_ENV_VAR_IS_SET_ON_CALL(3);
}

static void GITUaddList(GITUcloneState* pState, const GITUlist& List){
   COL_FUNCTION(GITUaddList);
   COL_VAR2(pState->Count, List.List.size());
   pState->Count += List.List.size();
   for(int i = 0; i < List.List.size(); i++) {
      const COLstring GitUrl = List.List[i].GitUrl;
      const COLstring Branch = List.List[i].Branch;
      const COLstring Key    = GITUresolveKeyFromBranchUrl(GitUrl, Branch);
      if (pState->List.count(Key) == 0){
         COL_TRC("Adding " << Key << " to list to refresh.");
         pState->List.add(Key, false);
         COLclosure3<bool, COLstring, GITUcloneResult>* pCallback = COLnewClosure3(&GITUcloneDone, pState);
         pState->pLoop->threadPool()->scheduleTask(COLnewClosure0(GITUcloneOnThread, GitUrl, Branch, pState->Username, pState->pLoop, pCallback));
      } else {
         COL_TRC("Seen " << Key << " already.");
         pState->Count--;
      }
   }
}

void GITUcloneCollection(const COLstring& Username, SCKloop* pLoop, const GITUlist& List, COLclosure2<bool, COLstring>* pFinished){
   COL_FUNCTION(GITUcloneCollection);
   GITUcloneState* pState = new GITUcloneState();
   pState->pFinished = pFinished;
   pState->Count = 0;
   pState->Username = Username;
   pState->pLoop = pLoop;
   GITUaddList(pState, List);
}

static void GITUcacheInitComplete(bool Success, COLstring Log, SCKloop* pLoop, COLstring* pLog, bool* pResult){
   COL_FUNCTION(GITUcacheInitComplete);
   GITUcacheLog("Cache refresh completed.", "#git #endpoint #end");
   *pLog = Log;
   *pResult = Success;
   pLoop->shutdown();
}

static COLstring GITUblankMainLua() {
   return R"(function main(Data)

end)";
}

static void GITUsetupBlankComponentCache() {
   COL_FUNCTION(GITUsetupBlankComponentCache);
   const COLstring BlankCacheDir = GITUblankTemplateDir();
   if(FILfileExists(BlankCacheDir) && !FILisDirectoryEmpty(BlankCacheDir)) { return; }
   COLvar          ConfigJson;
   ConfigJson["fields"];
   ConfigJson["fields"].setArrayType();
   try {
      const COLstring Main   = FILpathAppend(BlankCacheDir, "main.lua");
      const COLstring Config = FILpathAppend(BlankCacheDir, "config.json");
      FILcreateDirectoryTreeToFile(Main, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
      FILwriteFile(Main, GITUblankMainLua());
      FILwriteFile(Config, ConfigJson.json(1));
      COLstring Err;
      GITinit("admin", BlankCacheDir, false, &Err);
      GITadd(BlankCacheDir, "*", true, &Err);
      GITcommit("admin", BlankCacheDir, "", "Created blank component", &Err);
   } catch(const COLerror& E) { COL_TRC(E.description()); }
}

bool GITUcacheInit(const COLstring& Username, COLstring* pLog) {
   COL_FUNCTION(GITUcacheInit);
   COLstring Error;
   const COLstring ComponentDir = DIRcompsDir();
   GITUcacheLog("Begin refresh of git cache.\nDir: " + GITcacheDir() + "\nMore info: " + LINKgitCache, "#git #endpoint #start");
   GITUsetupBlankComponentCache();
   GITUlist List;
   try {
      const bool HasCatalogueRepos = GITUlistCatalogueLoad(&List);
      const bool HasUpstreamRepos  = GITUgetUpstreamList(ComponentDir, &List, &Error);
      if(!HasCatalogueRepos && !HasUpstreamRepos) {
         *pLog = "No catalogue or upstream repositories found";
         return true;
      }
   } catch(const COLerror& Error) {
      *pLog = "Failed to retrieve upstream list. " + Error.description();
      return false;
   }
   bool Success;
   GITUcloneList(Username, List, pLog, &Success);
   return Success;
}

void GITUcloneList(const COLstring& Username, const GITUlist& List, COLstring* pLog, bool* pResult){
   COL_FUNCTION(GITUcloneList);
   if(List.List.size() == 0) {
      *pLog = "No repositories to refresh";
      *pResult = true;
      return;
   }
   COLthreadPool Pool(1,5);
   SCKloop Loop(&Pool);
   Pool.start();
   GITUcloneCollection(Username, &Loop, List, COLnewClosure2(&GITUcacheInitComplete, &Loop, pLog, pResult));
   Loop.start();
   COL_VAR(*pLog);
}

bool GITUisCacheRefreshInProgress(bool* pGitCacheRefreshing, COLstring* pError){
   COL_FUNCTION(GITUisCacheRefreshInProgress);
   COL_TRC("Check if Git cache refresh is already running: " << *pGitCacheRefreshing);
   if (*pGitCacheRefreshing) {
      *pError = "Git cache refresh in progress, please try again later.";
      return true;
   }
   return false;
}
