// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITWtemplatesRefresh
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday 14 February 2023 - 04:34PM
// ---------------------------------------------------------------------------

#include <COL/COLclosure.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILutils.h>
#include <GITU/GITUcache.h>
#include <GIT/GITpath.h>
#include <GITW/GITWtemplatesRefresh.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
#include <time.h>
COL_LOG_MODULE;

#define GIT_CACHE_REFRESH_DELAY_SECONDS "GIT_CACHE_REFRESH_DELAY_SECONDS"
static time_t GITlatestCacheRefresh = 0;

static void GITWonCacheRefreshComplete(COLstring Log, bool Success, bool* pGitCacheRefreshing, COLaddress Address) {
   COL_FUNCTION(GITWonCacheRefreshComplete);
   COL_TRC("Done Git cache refresh, flipping boolean");
   *pGitCacheRefreshing = false;
   if (Success) {
      COLvar Result;
      Result["log"] = Log;
      COLrespondSuccess(Address, Result);
   } else {
      COLrespondError(Address, Log);
   }
}

static bool GITWremoveOld(const COLstring& CacheDir, COLstring& Error) {
   COL_FUNCTION(GITWremoveOld);
   COL_VAR(CacheDir);
   if(FILfileExists(CacheDir) && !FILremoveFullDirNewSafe(CacheDir, &Error)) {
      Error = "Failed to remove git cache. " + Error;
      COL_VAR(Error);
      return false;
   }
   return true;
}

static void GITWtemplatesRefreshImpl(const bool ClearCache, COLstring Username, SCKloop* pLoop, COLclosure2<COLstring, bool>* pCallback) {
   COL_FUNCTION(GITWtemplatesRefreshImpl);
   const COLstring GitCache  = GITcacheDir();
   const COLstring BranchDir = GITbranchDir();
   if(ClearCache) {
      COLstring RemoveError;
      if(!GITWremoveOld(GitCache, RemoveError)) {
         return pLoop->post(COLnewClosure0(pCallback, &COLclosure2<COLstring, bool>::runAndDelete, RemoveError, false));
      }
      if(!GITWremoveOld(BranchDir, RemoveError)) {
         return pLoop->post(COLnewClosure0(pCallback, &COLclosure2<COLstring, bool>::runAndDelete, RemoveError, false));
      }
   }
   COLstring  Log;
   const bool Success = GITUcacheInit(Username, &Log);
   pLoop->post(COLnewClosure0(pCallback, &COLclosure2<COLstring, bool>::runAndDelete, Log, Success));
}

static COLvar GITWtemplatesAbortMessage(const COLstring& User, const COLstring& Error){
   COL_FUNCTION(GITWtemplatesAbortMessage);
   COLvar Response;
   Response["busy"] = true;
   Response["message"] = Error;
   COLstring Err = "Git cache refresh request from user '" + User + "' was aborted. " + Error;
   SDBIlogService(Err, "#warning");
   return Response;
}

static bool GITWcacheRefreshThrottled() {
   COL_FUNCTION(GITWcacheRefreshThrottled);
   time_t Now = time(NULL);
   if(!GITlatestCacheRefresh) {
      COL_TRC("First git cache refresh, set GITlatestCacheRefresh to current time.");
      GITlatestCacheRefresh = Now;
      return false;
   }
   const char* pValue = getenv(GIT_CACHE_REFRESH_DELAY_SECONDS);
   if(pValue) {
      int Interval = ::atoi(pValue);
      if(Interval > 0 && (Now - GITlatestCacheRefresh) < Interval) {
         COL_TRC("Git cache refresh request was skipped. " <<  GIT_CACHE_REFRESH_DELAY_SECONDS << " is active.");
         return true;
      }
   }
   COL_TRC("Git cache refresh after " << GIT_CACHE_REFRESH_DELAY_SECONDS << ", proceed with refresh.");
   GITlatestCacheRefresh = Now;
   return false;
}

static COLvar GITWtemplatesThrottledMessage(const COLstring& User) {
   COL_FUNCTION(GITWtemplatesThrottledMessage);
   COLvar Response;
   Response["throttled"] = true;
   Response["message"]   = "Git cache refresh request from user '" + User + "' was skipped. Refresh is too soon after the previous refresh. Configure the GIT_CACHE_REFRESH_DELAY variable if you want more frequent refreshes.";
   return Response;
}

// /git/cache/refresh
void GITWtemplatesRefresh(const COLwebRequest& Request, bool* pGitCacheRefreshing, SCKloop* pLoop) {
   COL_FUNCTION(GITWtemplatesRefresh);
   const bool ForceRefresh = COLvarGetBool(Request.Data, "force", false);
   const bool Clear        = COLvarGetBool(Request.Data, "clear", false);
   // force used to force and clear. Now force & clear requires both to be set.
   // Forbid clear without force for now. IX-3800
   if(Clear && !ForceRefresh) { return COLrespondError(Request.Address, "'force' must be true if 'clear' is set."); }
   COLstring Error;
   if(!ForceRefresh && GITUisCacheRefreshInProgress(pGitCacheRefreshing, &Error)) {  // If its a not force refresh, abort if another git cache refresh is ongoing
      return COLrespondSuccess(Request.Address, GITWtemplatesAbortMessage(Request.User, Error));
   }
   if(!ForceRefresh && GITWcacheRefreshThrottled()) {
      return COLrespondSuccess(Request.Address, GITWtemplatesThrottledMessage(Request.User));
   }
   *pGitCacheRefreshing = true;
   COLstring Message    = "";
   if(ForceRefresh) { Message.append("Forced "); }
   if(Clear)        { Message.append("clean "); }
   Message.append("Git cache refresh request from user '" + Request.User + "' has started.");
   SDBIlogService(Message, "#git #warning");
   COLclosure2<COLstring, bool>* GitCacheRefreshCallback = COLnewClosure2(&GITWonCacheRefreshComplete, pGitCacheRefreshing, Request.Address);
   COL_TRC("Scheduling cache refresh on worker thread");
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&GITWtemplatesRefreshImpl, Clear, Request.User, pLoop, GitCacheRefreshCallback));
}
