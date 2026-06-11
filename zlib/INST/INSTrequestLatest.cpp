// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTrequestLatest
//
// Description:
//
// API to request upgrading to the latest release build
//
// Author: Matthew Sobkowski 
// Date:   Wednesday 13 November 2024 - 09:30AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <DIR/DIRurl.h>
#include <INST/INSTconfig.h>
#include <INST/INSTrequestLatest.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>
COL_LOG_MODULE;

// /upgrade/request/latest
void INSTupgradeRequestLatest(const COLwebRequest& Request) {
	COL_FUNCTION(INSTupgradeRequestLatest);
	COLstring Error;
	if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::UPGRADE, &Error)) {
		return COLrespondError(Request.Address, PRMsetError(ROLid::UPGRADE));
	}

	COLvar Result;
	if(!INSTgetServerConfig("admin", &Result, &Error)) {
		COL_TRC(Error);
		return COLrespondError(Request.Address, "Failed to load server configuration: " + Error);
	}
	WEBrequest	WebRequest;
	WEBresponse WebResponse;
	WebRequest.Url		 = DIRcrashServer() + "request/latest";
	WebRequest.Timeout = 2000;
	WebRequest.Body	 = Result.json();
	WEBrequestBlockingPost(WebRequest, &WebResponse);
	COL_VAR2(WebResponse.ReturnCode, WebResponse.Body);
	if(WebResponse.ReturnCode == 200) {
		COLrespondSuccess(Request.Address);
	} else {
		COLrespondError(Request.Address, "Failed to request the latest version. Code " + COLintToString(WebResponse.ReturnCode));
	}
}
