//  ---------------------------------------------------------------------------
//   Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//   Module: CRHupload.cpp
//
//   Description: Implementation
//
//   Author: Matthew Sobkowski
//   Date:   13/10/23 1:18 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <CRH/CRHupload.h>
#include <DBDVER/DBDVERversion.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>
COL_LOG_MODULE;

static COLstring CRHuploadedFile() { return DIRcrashDir() + "uploaded.json"; }

static void CRHupdateLastJson(const COLstring& FileName) {
   COL_FUNCTION(CRHupdateLastJson);
   COLvar Data; // in case we want to add more keys in future
   Data["last_upload"] = FileName;
   try                      { FILwriteFile(CRHuploadedFile(), Data.json(1)); }
   catch(const COLerror& E) { COL_ERR(E.description()); }
}

static void CRHreadCrashFile(const COLstring& FileName, COLstring* pContent) {
   COL_FUNCTION(CRHreadCrashFile);
   try                      { FILreadFile(FileName, pContent); }
   catch(const COLerror& E) { COL_ERR(E.description()); }
   COL_VAR(*pContent);
}

COLstring CRHlastUpload() {
   COL_FUNCTION(CRHlastUpload);
   COLstring File = CRHuploadedFile();
   COLstring LastUpload, Data;
   if(!FILfileExists(File)) { return LastUpload; }
   try {
      FILreadFile(File, &Data);
      COLvar Json; // json in case we want to add more keys to this in future
      Json.parse(Data);
      LastUpload = Json["last_upload"];

   } catch(const COLerror& E) { COL_ERR(E.description()); }
   return LastUpload;
}

static COLstring CRHnewestCrash() {
   COL_FUNCTION(CRHnewestCrash);
	const COLstring CrashDir = DIRcrashDir();
   FILdirEnumerator Dirs(CrashDir + "*.crash", false, true);
   COLstring FileName;
   while (Dirs.getFile(FileName)){
      FileName = CrashDir + FileName;
      COL_VAR(FileName);
   }
   return FileName;
}


static bool CRHonLatest() {
   COL_FUNCTION(CRHonLatest);
   WEBrequest  Request;
   WEBresponse Response;
   Request.Url  = "https://downloads.interfaceware.com/release.json";
   Request.Timeout = 3000;
   WEBrequestBlockingGet(Request, &Response);
   COL_VAR(Response.Body);
   COLvar LatestVersion;
   LatestVersion.parse(Response.Body);
   if(Response.ReturnCode != 200) { return false; } // in case release server is down
   return DBD_STRING_VERSION >= LatestVersion["version"].asString();
}

int CRHuploadCrashReport(COLstring* pFilename) {
   COL_FUNCTION(CRHuploadCrashReport);
   COLstring Newest = CRHnewestCrash();
   if (Newest.is_null()){
      COL_TRC("No crash files - yay!");
      return 0;
   }
   COLstring Last   = CRHlastUpload();
   COL_VAR2(Newest, Last);
   if(Newest.equals(Last)) { return 0; }
   COLstring CrashContent;
   CRHreadCrashFile(Newest, &CrashContent);
   int Code = CRHupload(CrashContent);
   if (Code == 200) {
      CRHupdateLastJson(Newest);
      if (pFilename) { *pFilename = Newest; }
   }
   return Code;
}

int CRHupload(const COLstring& Content) {
   COL_FUNCTION(CRHupload);
   COL_VAR(CRHonLatest()); // This checks whether the current running Iguana version matches the latest release
   if(!CRHonLatest()) { return -99; }
   WEBrequest  Request;
   WEBresponse Response;
   Request.Url  = "https://crashreport.interfaceware.com:7888/crash";
   Request.Body = Content;
   WEBrequestBlockingPost(Request, &Response);
   return Response.ReturnCode;
}