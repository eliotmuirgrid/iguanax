// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTdownload
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Monday 28 August 2023 - 10:11AM
// ---------------------------------------------------------------------------
#include <INST/INSTdownload.h>

#include <DIR/DIRurl.h>
#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>
#include <PRM/PRMcallbacks.h>

#include <ROL/ROLid.h>

#include <SDB/SDBcapi.h>

#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>

#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLstring INSTinstallerName() {
   COL_FUNCTION(INSTinstallerName);
#ifdef _WIN32
   return "iguana_setup.zip";
#elif __APPLE__
   return "IguanaX.zip";
#else // linux
#if defined(__arm__) || defined(__aarch64__)
   return "IguanaX-Arm.zip";
#else
   return "IguanaX.zip";
#endif
#endif
}

// Arm build server does not append -Arm to binary so we need separate installer helper function to resolve name
static COLstring INSTinstallerNameSpecial() {
   COL_FUNCTION(INSTinstallerNameSpecial);
#ifdef _WIN32
   return "iguana_setup.zip";
#elif __APPLE__
   return "IguanaX.zip";
#else // linux
   return "IguanaX.zip";
#endif
}

static void INSTaddZip(COLstring* pPath){
   COL_FUNCTION(INSTaddZip);
   *pPath += ".zip";
}

static void INSTclearDownloads() {
   COL_FUNCTION(INSTclearDownloads);
   COL_TRC("Clearing " << INSTdownloadDir());
   COLstring Error;
   if (!FILremoveFullDirNewSafe(INSTdownloadDir(), &Error)) { COL_ERR(Error); }
}

COLstring INSTdownloadDir(){
   COL_FUNCTION(INSTdownloadDir);
   COLstring Dir = FILpathAppend(FILworkingDir(), "upgrades/");
   COL_VAR(Dir);
   return Dir;
}

COLstring INSTdownloadFilePath(const COLstring& Version){
   COL_FUNCTION(INSTdownloadFilePath);
   COLstring VersionSan(Version);
   VersionSan.replace('.', '_');
   COLstring Filename = INSTdownloadDir() + VersionSan + "_iguana";
   INSTaddZip(&Filename);
   COL_VAR(Filename);
   return Filename;
}

static COLstring INSTdownloadUrlSpecial(const COLstring& Guid){
   COL_FUNCTION(INSTdownloadUrlSpecial);
   COLstring Url = DIRurlDownloads() + Guid + "/" + INSTinstallerNameSpecial();
   COL_VAR(Url);
   return Url;
}

static COLstring INSTdownloadUrl(const COLstring& Version){
   COL_FUNCTION(INSTdownloadUrl);
   COLstring Url = DIRreleaseServer() + Version + "/";
#ifdef _WIN32
   Url = Url + "windows/" + INSTinstallerName();
#elif  __APPLE__
   Url = Url + "macosx/"  + INSTinstallerName();
#else
   Url = Url + "linux/"   + INSTinstallerName();
#endif
   COL_VAR(Url);
   return Url;
}

bool INSTdownloaded(const COLstring& Version){
   COL_FUNCTION(INSTdownloaded);
   COLstring Filename = INSTdownloadFilePath(Version);
   return FILfileExists(Filename);
}

bool INSTdownload(const COLstring& Version, COLstring* pError){
   COL_FUNCTION(INSTdownload);
   INSTclearDownloads();
   COLstring Filename = INSTdownloadFilePath(Version);
   WEBrequest Request;
   WEBresponse Response;
   Request.Url = INSTdownloadUrl(Version);
   WEBrequestBlockingGet(Request, &Response);
   if (Response.ReturnCode == 200){
      if (!FILfileExists("upgrades")){
         FILmakeDir("upgrades", FIL_USER_RWX);
      }
      COL_TRC("Writing result " << Filename << " size = " << Response.Body.size());
      FILwriteFile(Filename, Response.Body);
      return true;
   } else {
      *pError = "Could not find that version.";
      return false;
   }
}

bool INSTdownloadSpecial(const COLstring& Guid, COLstring* pError){
   COL_FUNCTION(INSTdownloadSpecial);
   INSTclearDownloads();
   COLstring Filename = INSTdownloadFilePath(Guid);
   WEBrequest Request;
   WEBresponse Response;
   Request.Url = INSTdownloadUrlSpecial(Guid);
   COL_VAR(Request.Url);
   WEBrequestBlockingGet(Request, &Response);
   if (Response.ReturnCode == 200){
      if (!FILfileExists("upgrades")){
         FILmakeDir("upgrades", FIL_USER_RWX);
      }
      COL_TRC("Writing result " << Filename << " size = " << Response.Body.size());
      FILwriteFile(Filename, Response.Body);
      return true;
   } else {
      *pError = "Could not find that version.";
      return false;
   }
}

// /upgrade/download
void INSTupgradeDownload(const COLwebRequest& Request){
   COL_FUNCTION(INSTupgradeDownload);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::UPGRADE, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::UPGRADE));
   }
   COLstring Version = COLvarRequiredString(Request.Data, "version");
   SDBIlogService("Downloading IguanaX version " + Version + " as requested by user '" + Request.User + "'.", "#upgrade");
   bool Success = INSTdownload(Version, &Error);
   COL_VAR(Success);
   if (Success){
      SDBIlogService("Successfully downloaded IguanaX version " + Version + ".", "#upgrade");
      COLrespondSuccess(Request.Address);
   } else {
      SDBIlogService("Failed to download IguanaX version " + Version + ".", "#error #upgrade");
      COLrespondError(Request.Address, Error);
   }
}

// /upgrade/download/special
void INSTupgradeDownloadSpecial(const COLwebRequest& Request){
   COL_FUNCTION(INSTupgradeDownloadSpecial);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::UPGRADE, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::UPGRADE));
   }
   COLstring Guid = COLvarRequiredString(Request.Data, "guid");
   Guid = COLstripWhiteSpace(Guid, true, true);
   SDBIlogService("Downloading IguanaX version " + Guid + " as requested by user '" + Request.User + "'.", "#upgrade");
   bool Success = INSTdownloadSpecial(Guid, &Error);
   if (Success){
      SDBIlogService("Successfully downloaded IguanaX version " + Guid + ".", "#upgrade");
      COLrespondSuccess(Request.Address);
   } else {
      SDBIlogService("Failed to download IguanaX version " + Guid + ".", "#error #upgrade");
      COLrespondError(Request.Address, Error);
   }
}
