// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCfetchReposThreaded
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Monday 20 March 2023 - 12:52PM
// ---------------------------------------------------------------------------

#include <CMPC/CMPCfetchReposThreaded.h>
#include <CFG/CFGmap.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <GIT/GITfetch.h>
#include <GIT/GITpath.h>
#include <GITU/GITUcache.h>
#include <GITU/GITUmodules.h>
#include <GITU/GITUrepository.h>
#include <GIT/GITbranch.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

class CMPCfetchState{
public:
   bool Success=true;
   int Count=0;
   COLstring Log;
   COLstring Username;
   COLclosure2<bool, COLstring>* pFinished;
   SCKloop* pLoop;
};

void CMPCfetchDone(bool Success, COLstring Log, CMPCfetchState* pState){
   COL_FUNCTION(CMPCfetchDone);
   // We only manipulate pState on the main thread
   if (!Success){
      pState->Success = false;
   } 
   pState->Count--;
   COL_VAR(pState->Count);
   pState->Log += Log;
   if (pState->Count == 0){
      COL_TRC("Done!");
      pState->pFinished->runAndDelete(pState->Success, pState->Log);
      delete pState;
   }
}


void CMPCfetchOnThread(COLstring LibraryDir, CMPCfetchState* pState){
   COL_FUNCTION(CMPCfetchOnThread);
   COLstring Out, Error;
   bool Success = GITfetch(pState->Username, LibraryDir, "", &Out, &Error);
   COL_TRC("Cloned " << LibraryDir);
   pState->pLoop->post(COLnewClosure0(&CMPCfetchDone, Success, Out + Error, pState));
}


void CMPCfetchReposThreadedImpl(const COLstring& Username, SCKloop* pLoop, const COLmap<COLstring, GITUmodule>& ModuleList, const COLstring& ComponentId, COLclosure2<bool, COLstring>* pFinished){
   COL_FUNCTION(CMPCfetchReposThreadedImpl);
   CMPCfetchState* pState = new CMPCfetchState();
   pState->pFinished = pFinished;
   pState->Count = ModuleList.size();
   pState->Username = Username;
   pState->pLoop = pLoop;
   for (auto i=ModuleList.begin(); i != ModuleList.end(); i++){
      COLstring ModuleName = i->first;
      COLstring LibraryDir = GITUmoduleDir(ComponentId, ModuleName);
      pLoop->threadPool()->scheduleTask(COLnewClosure0(CMPCfetchOnThread, LibraryDir, pState));
   }
}

static void CMPConFetchRepoComplete(bool* pGitCacheRefreshing) {
   COL_FUNCTION(CMPConFetchRepoComplete);
   COL_TRC("Done Git cache refresh, flipping boolean");
   *pGitCacheRefreshing = false;
   COL_VAR(*pGitCacheRefreshing);
}

void CMPCcompletedFetch(bool Success, COLstring ErrorLog, SCKloop* pLoop, COLstring* pError, bool* pResult){
   COL_FUNCTION(CMPCcompletedFetch);
   *pError = ErrorLog;
   *pResult = Success;
   pLoop->shutdown();
}

void CMPCfetchRespositoriesWorker(COLstring Username, COLstring ComponentDir, COLstring ComponentId, COLaddress Address, COLclosure0* pCallback, SCKloop* pLoop){
   COL_FUNCTION(CMPCfetchRespositoriesWorker);
   if(!FILfileExists(ComponentDir + ".git")) return COLrespondError(Address, "Component repository does not exist: " + ComponentDir);
   COLstring Error;
   bool Success;
   COLthreadPool Pool(1,5);
   SCKloop Loop(&Pool);
   Pool.start();
   COLmap<COLstring, GITUmodule> ModuleList;
   COLstring FileName = ComponentDir + ".gitmodules";
   if(FILfileExists(FileName)){ GITUmoduleLoad(FileName, &ModuleList); }
   GITupdateDefaultBranch(ComponentDir);
   ModuleList.add("", GITUmodule("", ""));        // so that we fetch the main repo too
   CMPCfetchReposThreadedImpl(Username, &Loop, ModuleList, ComponentId, COLnewClosure2(&CMPCcompletedFetch, &Loop, &Error, &Success));
   Loop.start();
   COLrespondSuccess(Address);
   pLoop->post(pCallback);
}

// /component/fetch_repositories
void CMPCfetchRepositoriesThreaded(const COLwebRequest& Request, bool* pGitCacheRefreshing, SCKloop* pLoop) {
   COL_FUNCTION(CMPCfetchRepositoriesThreaded);
   const COLvar& Data = Request.Data;
   COL_VAR(Data);
   COLstring ComponentId, Error;
   if (!CFGcomponentId(Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   COLstring ComponentDir = DIRcomponentDevDir(ComponentId);
   if(GITUisCacheRefreshInProgress(pGitCacheRefreshing, &Error)) {  // Same issue as IX-3110
      COLvar Response;
      Response["busy"]    = true;
      Response["message"] = Error;
      return COLrespondSuccess(Request.Address, Response);
   }
   *pGitCacheRefreshing   = true;
   COLclosure0* pCallback = COLnewClosure0(&CMPConFetchRepoComplete, pGitCacheRefreshing);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&CMPCfetchRespositoriesWorker, Request.User, ComponentDir,
                                                    ComponentId, Request.Address, pCallback, pLoop));
}