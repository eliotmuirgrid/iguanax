// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCallUrls
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Tuesday 25 June 2024 - 02:12PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <GITC/GITCallUrls.h>
#include <GSV/GSVlink.h>
COL_LOG_MODULE;

static void GITCallUrlsImpl(const COLstring& InputUrl, const COLaddress& Address) {
	COL_FUNCTION(GITCallUrlsImpl);
	COLvar Result;
	GSVlink Link;
	bool Success	= GSVconstructLink(InputUrl, &Link);
	COLvar Links	= GSVlinkToVar(Link);
	if(Success) {
		Result = Links;
		COLrespondSuccess(Address, Result);
	} else {
		Result["error"] = "Error creating link for " + InputUrl;
		COLrespondError(Address, Result["error"]);
	}
}

// /git/all_urls
void GITCallUrls(const COLwebRequest& Request) {
	COL_FUNCTION(GITCallUrls);
	const COLstring Url = COLvarRequiredString(Request.Data, "url");
	GITCallUrlsImpl(Url, Request.Address);
}
