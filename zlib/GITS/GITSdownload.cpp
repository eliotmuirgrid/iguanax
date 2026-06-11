//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITSdownloadImpl.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   5/3/23 10:53 AM
//  ---------------------------------------------------------------------------
#include <GITS/GITSdownload.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>

#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool GITSdownloadImpl(COLvar* pResult, COLstring* pError) {
   COL_FUNCTION(GITSdownloadImpl);
   WEBrequest Request;
   Request.Url                        = "https://github.com/git-for-windows/git/releases/download/v2.40.0.windows.1/Git-2.40.0-64-bit.exe";
   Request.Timeout                    = 10000;
   Request.Headers["Accept"]          = "*/*";
   Request.Headers["Accept-Encoding"] = "gzip, deflate, br";
   COL_VAR(Request);
   COL_TRC("Sending request...");
   WEBresponse Response;
   WEBrequestBlockingGet(Request, &Response);
   COL_TRC("Request finished with return code=" << Response.ReturnCode);
   if (Response.ReturnCode == 200) {
      COLstring InstallerLocation = FILpathAppend(FILworkingDir(), "install");
      COLstring InstallerPath     = FILpathAppend(InstallerLocation, "GitInstaller.exe");
      FILmakeFullDir(InstallerLocation, FIL_USER_RWX);
      COL_TRC("Writing " << InstallerPath);
      FILwriteFile(InstallerPath, Response.Body);
      (*pResult)["path"] = InstallerPath;    // TODO - it doesn't make much sense to take this from the backend and pass it to the front end to get it in the back end again.
      return true;
   }
   COL_TRC("Request failed -- printing response info");
   for(const auto& it : Response.Headers) { COL_VAR2(it->first, it->second); }
   *pError = Response.Body;
   return false;
}

// /git/download
void GITSdownload(const COLwebRequest& Request) {
   COL_FUNCTION(GITSdownload);
   COLstring Error;
   COLvar Result;
   bool      Success = GITSdownloadImpl(&Result, &Error);
   COL_VAR3(Success, Error, Result);
   Success ? COLrespondSuccess(Request.Address, Result) : COLrespondError(Request.Address, Error);
}
