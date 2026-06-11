//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDusage
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Mon Oct  7 09:53:32 EDT 2024
//---------------------------------------------------------------------------
#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>

#include <USG/USGusage.h>
#include <USG/USGusageCollector.h>

#include <SCK/SCKloop.h>

#include <INI/INIdata.h>

#include <GITU/GITUconfigGlobal.h>

#include <DIR/DIRurl.h>

#include <DBDVER/DBDVERversion.h>

#include <DBD/DBDusage.h>

#include <COL/COLclosure.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


static void DBDusageUploaded(COLstring Error) {
   COL_FUNCTION(DBDusageUploaded);
   if (Error.is_null()) {
      USGusages().updateUploadTime();
   } else {
      COL_VAR(Error);
      USGusages().setLastError(Error);
   }
}

static void DBDgetGitUserEmail(COLvar& Name, COLvar& Email){
   COL_FUNCTION(DBDgetGitUserEmail);
   INIdata Config;
   if (GITUconfigLoadUser("admin", &Config)) {
      Name  = GITUconfigUserName(Config);
      Email = GITUconfigUserEmail(Config);
   } else {
      Name  = "N/A - Git user not set for user 'admin'.";
      Email = "N/A - Git email not set for user 'admin'.";
   }
}

static COLstring DBDgetOS() {
#ifdef _WIN32
   return "Windows";
#elif __APPLE__
   return "MacOS";
#else
#if defined(__arm__) || defined(__aarch64__)
   return "Arm Linux";
#else
   return "Linux";
#endif
#endif
   return "Impossible OS";
}

static COLvar DBDpreparePayload(const USGusage& Usages) {
   COL_FUNCTION(DBDpreparePayload);
   COLvar Payload = Usages.toVar();
   DBDgetGitUserEmail(Payload["name"], Payload["email"]);
   Payload["version"] = DBD_STRING_VERSION;
   Payload["os"] = DBDgetOS();
   return Payload;
}

static int DBDperformUsageUpload(const COLstring& Data, COLstring* pError = nullptr) {
   COL_FUNCTION(DBDperformUsageUpload);
   COL_VAR(Data);
   WEBrequest  Request;
   WEBresponse Response;
   Request.Url  = DIRcrashServer() + "usage";
   Request.Body = Data;
   WEBrequestBlockingPost(Request, &Response);
   if (Response.ReturnCode == 503 && pError) {
      *pError = Response.Body;
   }
   return Response.ReturnCode;
}

// WORKER THREAD
static void DBDusageUploadWorker(SCKloop* pLoop, USGusage Usages) {
   COL_FUNCTION(DBDusageUploadWorker);
   COLstring Error;
   COLvar Payload = DBDpreparePayload(Usages);
   DBDperformUsageUpload(Payload.json(), &Error);
   pLoop->post(COLnewClosure0(&DBDusageUploaded, Error));
}

static void DBDuploadUsage(SCKloop* pLoop) {
   COL_FUNCTION(DBDuploadUsage);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&DBDusageUploadWorker, pLoop, USGusages()));
}

void DBDstartUsageCollection(SCKloop* pLoop) {
   COL_FUNCTION(DBDstartUsageCollection);
   USGstartUsageCollectionTimer(pLoop, COLnewClosure0(&DBDuploadUsage, pLoop));
}

void DBDstopUsageCollection() {
   COL_FUNCTION(DBDstopUsageCollection);
   USGstopUsageCollectionTimer();
}
