// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDUPreleaseInfo
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 14 August 2023 - 10:19AM
// ---------------------------------------------------------------------------
#include <DBDUP/DBDUPreleaseInfo.h>

#include <DBDVER/DBDVERversion.h>
#include <DBDVER/DBDVERgitInfo.h>

#include <DIR/DIRurl.h>

#include <FIL/FILutils.h>

#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>

#include <time.h>

#include <COL/COLsplit.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// depends on FILinitExecutableName() for Posix
static bool DBDUPupgradeIniPresent() {
   COL_FUNCTION(DBDUPupgradeIniPresent);
   COLstring InitFilePath = FILexecutableDir() + "upgrade.ini";
   return FILfileExists(InitFilePath);
}

// Compares release version to DBD_STRING_VERION following semantic version rules. 
// Returns 1 if release is newer, -1 if release is older and 0 if equal.
static int DBDUPcompareVersionImpl(const COLstring& ReleaseVersion){
   COL_FUNCTION(DBDUPcompareVersionImpl);
   COLarray<COLstring> ReleaseVersionParts, CurrentVersionParts;
   COLsplit(&ReleaseVersionParts, ReleaseVersion, ".");

   COLstring CurrentVersion = DBD_STRING_VERSION;
   COLsplit(&CurrentVersionParts, CurrentVersion, ".");

   // Compare MAJOR, MINOR, and PATCH
   for (size_t i = 0; i < 3; ++i) {
      if (atoi(ReleaseVersionParts[i].c_str()) < atoi(CurrentVersionParts[i].c_str())) return -1;
      if (atoi(ReleaseVersionParts[i].c_str()) > atoi(CurrentVersionParts[i].c_str())) return 1;
   }
   return 0;  // Versions are equal
}

bool DBDUPcompareVersion(const COLstring& ReleaseVersion, bool* IsNewer){
   COL_FUNCTION(DBDUPcompareVersion);
   int CompareResult = DBDUPcompareVersionImpl(ReleaseVersion);
   COL_TRC("Compare Returned: " << CompareResult);
   COL_TRC("1 means release is newer, -1 means release is older, 0 means release and current are the same.");
   *IsNewer = CompareResult == -1; // IsNewer refers to the current version being newer than the release version
   return CompareResult == 1;
}

static bool DBDUPcheckCrashReportServer(WEBresponse* pCrashReportResponse) {
   COL_FUNCTION(DBDUPcheckCrashReportServer);
   WEBrequest CrashReportRequest;
   CrashReportRequest.Url = DIRcrashServer();
   WEBrequestBlockingGet(CrashReportRequest, pCrashReportResponse);
   COL_VAR2(pCrashReportResponse->ReturnCode, pCrashReportResponse->Body);
   return pCrashReportResponse->ReturnCode == 200;
}

static bool DBDUPcheckUpgradeServer(WEBresponse* pVersionResponse){
   COL_FUNCTION(DBDUPcheckUpgradeServer);
   WEBrequest VersionRequest;
   VersionRequest.Url = DIRreleaseServer() + "release.json";
   WEBrequestBlockingGet(VersionRequest, pVersionResponse);
   COL_VAR2(pVersionResponse->ReturnCode, pVersionResponse->Body);
   return pVersionResponse->ReturnCode == 200;
}

// /release/info
void DBDUPreleaseInfo(const COLwebRequest& Request){
   COL_FUNCTION(DBDUPreleaseInfo);
   COLvar Response;
   Response["upgrade_available"] = false;
   bool IsNewer = false;
   WEBresponse VersionResponse, CrashReportResponse;
   const COLstring CheckInternet = "Internet connection may be down";
   if(!DBDUPcheckUpgradeServer(&VersionResponse)){
      Response["code"] = VersionResponse.ReturnCode;
      Response["error"] = VersionResponse.Body;
      Response["reasons"].push_back(CheckInternet);
      return COLrespondSuccess(Request.Address, Response);
   }
   if(!DBDUPcheckCrashReportServer(&CrashReportResponse)){
      Response["code"] = CrashReportResponse.ReturnCode;
      Response["error"] = CrashReportResponse.Body;
      Response["reasons"].push_back(CheckInternet);
      Response["reasons"].push_back("Firewall may be blocking access on port 7888");
      return COLrespondSuccess(Request.Address, Response);
   }
   Response["code"] = VersionResponse.ReturnCode;
   COLvar WebResponse;
   WebResponse.parse(VersionResponse.Body.c_str());
   COLstring ReleaseVersion = WebResponse["version"];
   COL_TRC("Found version " + ReleaseVersion + " on the release server");
   COLstring ForceUpgrade = getenv("DBD_UPGRADE");
   if (!ForceUpgrade.is_null()){
      COL_TRC("Warning DBD_UPGRADE defined so we will allow any upgrade/downgrade");
      Response["upgrade_available"] = true;
   } else if (DBDUPupgradeIniPresent()) {
      COL_TRC("Warning upgrade.ini found so we will allow any upgrade/downgrade");
      Response["upgrade_available"] = true;
   } else {
      Response["upgrade_available"] = DBDUPcompareVersion(ReleaseVersion, &IsNewer);
   }
   Response["newer"]   = IsNewer; // to specify that the local version is newer than the release version
   Response["version"] = ReleaseVersion;

   COLrespondSuccess(Request.Address, Response);
}