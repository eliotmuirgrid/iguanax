// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTupgradeAccess
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Thursday 09 November 2023 - 07:26AM
// ---------------------------------------------------------------------------

#include <INST/INSTupgradeAccess.h>

#include <DIR/DIRurl.h>
#include <SDB/SDBcapi.h>

#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


// /upgrade/access
void INSTupgradeAccess(const COLwebRequest& Request){
   COL_FUNCTION(INSTupgradeAccess);
   COLstring Url = DIRurlDownloads();
   WEBrequest WebRequest;
   WEBresponse WebResponse;
   WebRequest.Url = Url;
   WebRequest.Timeout = 2000;
   WEBrequestBlockingGet(WebRequest, &WebResponse);
   SDBIlogService("Checking for network access to: " + Url + " as requested by user '" + Request.User + "' for trying to do an upgrade special.", "#upgrade");
   COL_VAR2(WebResponse.ReturnCode, WebResponse.Body);
   COLvar Response;
   int Code = WebResponse.ReturnCode;
   bool Success = Code == 200;
   if(Success){
      SDBIlogService("Successfully connected to upgrade special server: " + Url + ".", "#upgrade");
   }
   else{
      COLstring ErrorMessage = WebResponse.Body;
      SDBIlogService("Failed to connect to upgrade special server: " + Url + ". " + ErrorMessage, "#upgrade");
      Response["error"] = ErrorMessage;
   }
   Response["code"] = Code;
   Response["url"] = Url;
   COLrespondSuccess(Request.Address, Response);
}