// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCserver
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 10 June 2024 - 3:21PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <GITC/GITCserver.h>
#include <GSV/GSVconfig.h>
#include <GSV/GSVlink.h>
#include <GSV/GSVmap.h>
COL_LOG_MODULE;

// /git/server
void GITCserver(const COLwebRequest& Request) {
	COL_FUNCTION(GITCserver);
	GSVconfig Config;
	GSVconfigLoadUser(Request.User, &Config);
	COLstring ServerUrl = Config.m_ServerUrl;
	if(!GSVmapExists(ServerUrl)) {
		COLstring Error;  
		if(ServerUrl.isWhitespace()) Error = "Git Server URL is not set";
		else 								  Error = "<b>" + ServerUrl + " </b> not found in Git Server list";
		return COLrespondError(Request.Address, Error);
	}
	const GSVitem Item  = GSVmapItem(ServerUrl);
	COLstring ServerLink;
	if(!GSVconstructServerLink(ServerUrl, &ServerLink)){
		COL_TRC("Could not make a link to the server: " + ServerUrl);
	}
	COLvar Response	  = GSVitemToVar(Item);
	Response["address"] = ServerUrl;
	Response["cloud"]   = GSVserviceIsCloud(Item.m_Type);
	Response["web"]     = ServerLink;
	COLrespondSuccess(Request.Address, Response);
}