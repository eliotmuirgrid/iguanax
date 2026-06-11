// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: parallelUpload.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   Friday, September 29, 2023
// ---------------------------------------------------------------------------
#include "parallel.h"
#include "constants.h"
#include "download.h"
#include "serverUpload.h"
#include "utils.h"
#include "version.h"
#include <cstdio>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <SCK/SCKloop.h>

#include <COL/COLvar.h>
#include <COL/COLarray.h>
#include <COL/COLclosure.h>
#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

class BinaryTracker {
 public:
   COLstring Hash;
   COLstring BuildLocation;
   COLstring Changelog;
   COLmap<COLstring, COLarray<COLstring>> pFileMap;
   COLarray<const char*> pOsList;
   int  OsRequests      = 0;
   int  BinaryDownloads = 0;
   bool Success         = true;
   SCKloop* pLoop;
};

// add to this list for new builds
static COLarray<const char*> constructList() {
   COL_FUNCTION(constructList);
   COLarray<const char*> OsList;
   OsList.push_back(LIN_ARM_URL);
   OsList.push_back(LIN_URL);
   OsList.push_back(MAC_URL);
   OsList.push_back(WIN_URL);
   return OsList;
}

static void cleanupParallelItems(BinaryTracker* pTracker) {
   COL_FUNCTION(cleanupParallelItems);
   pTracker->pLoop->shutdown();
   delete pTracker;
}

static bool updateVersionJson(const COLstring& Version) {
   COL_FUNCTION(updateVersionJson);
   char Answer[25]; // big for safety
   reportMessage("Would you like to update version.json to " + Version + "? (y/yes)");
   if(!fgets(Answer, sizeof(Answer), stdin)) { return reportMessage("Nothing was entered -- exiting program", false); }
   COLstring AnsString = COLremoveAllWhiteSpace(COLstring(Answer).toLowerCase());
   if(AnsString != "y" && AnsString != "yes") { return false; }
   if(!createReleaseJson(Version)) { return reportMessage("Error creating release json", false); }
   if(!uploadReleaseJson())        { return reportMessage("Error uploading release json", false); }
   return true;
}

static bool renameAndUpload(const COLstring& PathToBuilds, COLstring* pVersion) {
   COL_FUNCTION(renameAndUpload);
   COL_VAR(PathToBuilds);
   *pVersion = getVersion(PathToBuilds);
   if(pVersion->is_null()) { return reportMessage("Could not get version from executable", false); }
   reportMessage("IguanaX Version: " + *pVersion);
   COLstring ReleaseDir = FILpathAppend(FILworkingDir(), *pVersion) + FIL_DIR_SEPARATOR;
   COL_VAR(ReleaseDir);
   cleanupRelease(ReleaseDir);
   FILrenameFile(PathToBuilds, ReleaseDir);
   if(!uploadRelease(ReleaseDir)) {
      cleanupRelease(ReleaseDir);
      return reportMessage("Upload failed -- check PEM_FILE is correct and the server is running", false);
   }
   changeFilePermission(*pVersion);
   reportMessage("Release upload completed");
   return true;
}

static void downloadBinaryCallback(bool Success, const char* OsUrl, COLstring File, BinaryTracker* pTracker) {
   COL_FUNCTION(downloadBinaryCallback);
   pTracker->BinaryDownloads--;
   if(Success)  { reportMessage("Successfully downloaded " + File + " from " + OsUrl); }
   else         { pTracker->Success = false; }
   if(pTracker->BinaryDownloads == 0) {
      if(!pTracker->Success) { return cleanupParallelItems(pTracker); }
      reportMessage("Finished downloading all OS-specific binaries -- beginning upload process");
      COLstring Version;
      if(!renameAndUpload(pTracker->BuildLocation, &Version)) { return cleanupParallelItems(pTracker); }
      if(!uploadChangelog(Version, pTracker->Changelog))      { return cleanupParallelItems(pTracker); }
      updateVersionJson(Version);
      cleanupParallelItems(pTracker);
   } else { reportMessage(COLint32ToString(pTracker->BinaryDownloads) + " downloads remaining"); }
}

static void downloadParallelBinary(const char* OsUrl, COLstring File, BinaryTracker* pTracker) {
   COL_FUNCTION(downloadParallelBinary);
   bool Success = downloadHash(OsUrl, pTracker->Hash, File, pTracker->BuildLocation);
   pTracker->pLoop->post(COLnewClosure0(&downloadBinaryCallback, Success, OsUrl, File, pTracker));
}

static void appendFileList(const COLstring& OsUrl, COLvar* pOsFileList, BinaryTracker* pTracker) {
   COL_FUNCTION(appendFileList);
   reportMessage("Successfully got file list from " + OsUrl);
   COLvar& OsFileList = *pOsFileList;
   COLarray<COLstring>& FileList = pTracker->pFileMap[COLstring(OsUrl)];
   for(int i = 0; i < OsFileList.size(); i++) {
      COL_TRC("Appending file: " << OsFileList[i].asString() << " to list " << OsUrl << newline);
      FileList.push_back(OsFileList[i].asString());
      pTracker->BinaryDownloads++;
   }
   delete pOsFileList;
}

static void requestOsCallback(const char* OsUrl, bool Success, COLvar* pOsFileList, BinaryTracker* pTracker) {
   COL_FUNCTION(requestOsCallback);
   pTracker->OsRequests--;
   if(Success) { appendFileList(OsUrl, pOsFileList, pTracker); }
   else        { pTracker->Success = reportMessage("Failed to download file list from " + COLstring(OsUrl), false); }
   if(pTracker->OsRequests == 0) {
      if(!pTracker->Success) { return cleanupParallelItems(pTracker); }
      reportMessage("Finished querying OS-specific files -- beginning download of " + COLint32ToString(pTracker->BinaryDownloads) + " binaries");
      for(const auto& it : pTracker->pFileMap) {
         for(const auto& File : it->second) {
            pTracker->pLoop->threadPool()->scheduleTask(COLnewClosure0(&downloadParallelBinary, it->first.c_str(), File, pTracker));
         }
      }
   }
}

static void requestParallelOs(const char* OsUrl, BinaryTracker* pTracker) {
   COL_FUNCTION(requestParallelOs);
   auto pFileList = new COLvar();
   bool Success = requestOs(OsUrl, pTracker->Hash, pFileList);
   pTracker->pLoop->post(COLnewClosure0(&requestOsCallback, OsUrl, Success, pFileList, pTracker));
}

void beginParallelProcess(const COLstring& Hash, const COLstring& Changelog) {
   COL_FUNCTION(beginParallelProcess);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   COLarray<const char*> OsList = constructList();
   auto pTracker = new BinaryTracker();
   pTracker->OsRequests    = OsList.size();
   pTracker->Hash          = Hash;
   pTracker->BuildLocation = FILpathAppend(FILworkingDir(), "IguanaXBuilds") + FIL_DIR_SEPARATOR;
   pTracker->Changelog     = Changelog;
   pTracker->pLoop         = &Loop;
   cleanupRelease(pTracker->BuildLocation);
   FILmakeFullDir(pTracker->BuildLocation, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   for(const auto& Os : OsList) {
      COL_VAR(Os);
      Loop.threadPool()->scheduleTask(COLnewClosure0(&requestParallelOs, Os, pTracker));
   }
   Loop.start();
}