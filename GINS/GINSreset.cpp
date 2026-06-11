// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSreset
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Tuesday 17 September 2024 - 08:13AM
// ---------------------------------------------------------------------------
#include <CFG/CFGmap.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILutils.h>
#include <GINS/GINSreset.h>
#include <GINS/GINSutils.h>
#include <GIT/GITpath.h>
#include <GITU/GITUcacheRepoFromUrl.h>
#include <GITU/GITUurl.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
#include <USR/USRmap.h>
COL_LOG_MODULE;

struct GINSresetStatus{
   bool      Running = false;
   COLstring Error;
};

static GINSresetStatus GINSresetStatusTracker;

static void GINSclearStatusTracker(){
   // COL_FUNCTION(GINSclearStatusTracker);   
   GINSresetStatusTracker.Running = false;
   GINSresetStatusTracker.Error = "";
}

static void GINSsetStatusRunning(bool Running) { 
   // COL_FUNCTION(GINSsetStatusRunning);   
   GINSresetStatusTracker.Running = Running; 
}

static void GINSsetStatusError(const COLstring& Error){ 
   // COL_FUNCTION(GINSsetStatusError);
   GINSresetStatusTracker.Error = Error;
}

static void GINSresetCallback() {
   COL_FUNCTION(GINSresetCallback);
   GINSsetStatusRunning(false);
   COL_VAR(time(NULL));
   COL_TRC("Reset complete");
}

static bool GINSresetCopy(const COLstring& SourceDir, const COLstring& DestDir) {
   COL_FUNCTION(GINSresetCopy);
   COL_VAR2(SourceDir, DestDir);
   const int Mode = FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX;
   try {
      if(!FILfileExists(SourceDir)) {
         COL_ERR("Source repo dir not found: " << SourceDir);
         return false;
      }
      FILremoveFullDirNew(DestDir);
      FILmakeFullDir(DestDir, Mode);
      const int CopyCount = FILcopyDirectoryContents(SourceDir, DestDir, Mode, true);
      if(CopyCount < 0) {
         COL_ERR("Failed to copy instance repo from " << SourceDir << " to " << DestDir);
         return false;
      }
      COL_TRC("Successfully reset by copying " << COLintToString(CopyCount) << " files.");
   } catch(const COLerror& E) {
      COL_ERR("Error resetting: " << E.description());
      return false;
   }
   return true;
}

static void GINSresetWorker(COLmap<COLstring, USRuser>* pUserMap, COLstring GitUrl,
                            COLclosure0* pCallback, SCKloop* pLoop) {
   COL_FUNCTION(GINSresetWorker);
   const COLstring Dir = DIRconfigFile("");
   COLstring       Error, Out;
   COL_VAR(Dir);
   COLstring SshUrl;
   if(!GITUsshUrl(GitUrl, &SshUrl)) {
      GINSsetStatusError("Failed to parse url into ssh form");
      return pLoop->post(pCallback);
   }
   COLvar    Config;
   COLstring Err;
   COLstring Username;
   if(GINSloadAndParse(&Config, &Err)) {
      Username = Config["user"].asString();
   } else {
      GINSsetStatusError("Failed to load git user settings");
      return pLoop->post(pCallback);
   }
   if(Username.is_null() || pUserMap->count(Username) == 0) {
      GINSsetStatusError("\"Push/Pull as\" user must be set and valid");
      return pLoop->post(pCallback);
   }
   // SDBIlogService("GITUcacheRepoFromUrl started " + COLintToString(time(NULL)), "#debug");
   if(!GITUcacheRepoFromUrl(Username, SshUrl, "", &Out)) {
      GINSsetStatusError("Failed to cache instance repository. Check service logs for details.");
      return pLoop->post(pCallback);
   }
   COLstring SourceDir = GITgitTemplateDir(SshUrl);
   // SDBIlogService("GINSresetCopy started "  + COLintToString(time(NULL)), "#debug");
   if(!GINSresetCopy(SourceDir, Dir)) {
      GINSsetStatusError("Failed to copy instance repository into place. Check service logs for details.");
      return pLoop->post(pCallback);
   }
   // SDBIlogService("GINSinitializeSubmodules started " + COLintToString(time(NULL)), "#debug");
   GINSinitializeSubmodules(Dir, Username, &Error);
   GINSpullComponentRepos(Dir, Username, &Error);
   // SDBIlogService("GINSchangeKeyPermissions started " + COLintToString(time(NULL)), "#debug");
   GINSchangeKeyPermissions();
   if(!Error.is_null()) {
      GINSsetStatusError("Failed to setup certain components. Check service logs for details.");
   }
   // SDBIlogService("GINSresetWorker done " + COLintToString(time(NULL)), "#debug");
   pLoop->post(pCallback);
}

// instance/reset
void GINSreset(const COLwebRequest& Request, COLmap<COLstring, USRuser>* pUserMap, SCKloop* pLoop) {
   COL_FUNCTION(GINSreset);
   COL_VAR(Request.Data.json(1));
   COL_VAR(time(NULL));
   COLstring Error;
   if (GINSresetStatusTracker.Running) { 
      return COLrespondError(Request.Address, "Failed to start git instance reset. Another git instance reset is currently in progress.");
   }
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", MANAGE_INSTANCE, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(MANAGE_INSTANCE));
   }
   GINSclearStatusTracker();
   GINSsetStatusRunning(true);
   const COLstring GitUrl    = COLvarRequiredString(Request.Data, "git");
   COLclosure0*    pCallback = COLnewClosure0(&GINSresetCallback);
   pLoop->threadPool()->scheduleTask(
       COLnewClosure0(&GINSresetWorker, pUserMap, GitUrl, pCallback, pLoop));
   return COLrespondSuccess(Request.Address);
}

// instance/reset/check
void GINSresetCheck(const COLwebRequest& Request){
   COL_FUNCTION(GINSresetCheck);
   COLvar Result;
   Result["running"] = GINSresetStatusTracker.Running;
   if (Result["running"].asBool()){
      COL_DBG("Git instance reset in progress");
      return COLrespondSuccess(Request.Address, Result);
   }
   // Reset is either done or has run into error
   if (GINSresetStatusTracker.Error.is_null()){
      COL_TRC("Git instance reset completed successfully");
      return COLrespondSuccess(Request.Address, Result);
   } else {
      COL_TRC("Git instance encountered an error: " << GINSresetStatusTracker.Error);
      return COLrespondError(Request.Address, GINSresetStatusTracker.Error);
   }
}
