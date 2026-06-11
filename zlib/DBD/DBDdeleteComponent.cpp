// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDdeleteComponent
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Friday 06 October 2023 - 03:00PM
// ---------------------------------------------------------------------------
#include <CFG/CFGmapApi.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <DBD/DBDdeleteComponent.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GINS/GINSinitialize.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
#include <SDB/SDBdir.h>
#include <SDB/SDBsystem.h>

#include <memory>
COL_LOG_MODULE;

struct DBDdeleteTracker {
   DBDdeleteTracker(const int Count, const COLstring& LogPrefix) : m_Count(Count), m_LogPrefix(LogPrefix) {}
   int       m_Count     = 0;
   COLstring m_LogPrefix = "";
};

static COLstring DBDarchiveDir(const COLstring& ComponentId) {
   COL_FUNCTION(DBDarchiveDir);
   COLstring Result = FILpathAppend(DIRarchiveDir(), ComponentId);
   FILaddPathSeparator(Result);
   COL_VAR2(ComponentId, Result);
   return Result;
}

static void DBDarchiveComponentRenameDir(const COLstring& ParentDir, const COLstring& Removed,
                                         const COLstring& SubDir) {
   COL_FUNCTION(DBDarchiveComponentRenameDir);
   COL_VAR2(ParentDir, Removed);
   if(FILfileExists(Removed)) {
      const COLstring Destination = DBDarchiveDir(ParentDir) + SubDir;
      COL_TRC("Archiving directory " << Removed << " to " << Destination);
      try {
         FILrenameDirectory(Removed, Destination, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX, true);
      } catch(const COLerror& Error) {  // Important to catch so we don't kill the worker thread and crash.
         COL_TRC("Failed to remove directory " + Removed);
         SDBIlogService("Failed to remove directory " + Removed + ". Please remove this directory manually.", "#error");
      }
   }
}

static void DBDdeleteFromInstanceFiles(const COLarray<COLstring> ToDelete, SCKloop* pLoop, COLclosure0* pCallback) {
   COL_FUNCTION(DBDdeleteFromInstanceFiles);
   GINSremoveComponentList(ToDelete);
   pLoop->post(pCallback);
}

static void DBDarchiveComplete(std::shared_ptr<DBDdeleteTracker> pTracker, COLclosure0* pApiCallback) {
   COL_FUNCTION(DBDarchiveComplete);
   pTracker->m_Count--;
   if(pTracker->m_Count == 0) {
      COL_TRC("Deletion completed -- removing from instance modules and config");
      pApiCallback->runAndDelete();
   }
}

static void DBDdeleteComponentWorker(COLstring Guid, std::shared_ptr<DBDdeleteTracker> pTracker, SCKloop* pLoop,
                                     COLclosure0* pCallback) {
   COL_FUNCTION(DBDdeleteComponentWorker);
   DBDarchiveComponentRenameDir("comps", DIRcomponentDir(Guid), Guid);
   // This one occasionally fails if Windows is still trying to write out log data. If it does fail, the auto-purge will
   // clean it up later.
   DBDarchiveComponentRenameDir(pTracker->m_LogPrefix, SDBdir(Guid), Guid);

   const COLstring RunDir = DIRcomponentRunDir(Guid);
   COLstring       RemoveError;
   if(FILfileExists(RunDir) && !FILremoveFullDirNewSafe(RunDir, &RemoveError)) {
      COL_TRC("Failed to remove directory " + RunDir);
      SDBIlogService("Failed to remove directory " + RunDir + ". Please remove this directory manually.", "#error");
   }
   COL_TRC("Archived all files for " << Guid);
   pLoop->post(pCallback);
}

void DBDdeleteComponent(const COLarray<COLstring>& ToDelete, const COLstring& User, COLclosure0* pCallback,
                        SCKloop* pLoop, SDBsystem* pSystem, CFGmap* pMap) {
   COL_FUNCTION(DBDdeleteComponent);
   const COLstring LogFolderPrefix = pSystem->LogEncryptionKey.is_null() ? "logt" : "loge";
   auto            pTracker        = std::make_shared<DBDdeleteTracker>(ToDelete.size() + 1, LogFolderPrefix);
   for(const auto& Guid : ToDelete) {
      pMap->ConfigMap.removeKey(Guid);
      pSystem->ComponentMap.removeKey(Guid);
      COLclosure0* pDeleteCallback = COLnewClosure0(&DBDarchiveComplete, pTracker, pCallback);
      pLoop->threadPool()->scheduleTask( COLnewClosure0(&DBDdeleteComponentWorker, Guid, pTracker, pLoop, pDeleteCallback));
   }
   COLclosure0* pDeleteCallback = COLnewClosure0(&DBDarchiveComplete, pTracker, pCallback);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&DBDdeleteFromInstanceFiles, ToDelete, pLoop, pDeleteCallback));
   CFGremoveDeletedSources(pMap);
}
