// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTrequestSpecial
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Thursday 09 November 2023 - 07:26AM
// ---------------------------------------------------------------------------

#include <INST/INSTrequestSpecial.h>
#include <INST/INSTconfig.h>

#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>

#include <DIR/DIRurl.h>

#include <PRM/PRMcallbacks.h>

#include <ROL/ROLid.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


// /upgrade/request/special
void INSTupgradeRequestSpecial(const COLwebRequest& Request){
   COL_FUNCTION(INSTrequestSpecial);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::UPGRADE, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::UPGRADE));
   }

   COLvar Result;
   if (!INSTgetServerConfig("admin", &Result, &Error)) {
      COL_TRC(Error);
      return COLrespondError(Request.Address, "Failed to load server configuration: " + Error);
   }
   WEBrequest WebRequest;
   WEBresponse WebResponse;
   WebRequest.Url = DIRcrashServer() + "request/beta";
   WebRequest.Timeout = 2000;
   WebRequest.Body = Result.json();
   WEBrequestBlockingPost(WebRequest, &WebResponse);
   COL_VAR2(WebResponse.ReturnCode, WebResponse.Body);
   if (WebResponse.ReturnCode == 200) {
      COLrespondSuccess(Request.Address);
   } else {
      COLrespondError(Request.Address, "Failed to request the latest beta build. Code " + COLintToString(WebResponse.ReturnCode));
   }
}