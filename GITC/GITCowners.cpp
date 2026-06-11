// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCowners
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Tuesday 09 April 2024 - 12:28PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <GITC/GITCowners.h>
#include <GITC/GITCorganizations.h>
#include <GITE/GITEutils.h>
#include <GSV/GSVconfig.h>
#include <GSV/GSVmap.h>
COL_LOG_MODULE;

// /git/owners
void GITCowners(const COLwebRequest& Request) {
	COL_FUNCTION(GITCowners);
	COLvar Result;
	// First check for organizations.json file. If its not empty then use this instead of the owners.json file.
	COLvar OrganizationsData;
	if (GITCorgsFileContent(Request.User, &OrganizationsData)){
		Result["organizations"] = true;
		Result["exists"] = true;
		Result["owners"] = OrganizationsData;
		return COLrespondSuccess(Request.Address, Result);
	}
	// organizations.json doesn't exist or is empty. Continue with owners.json.
	GSVconfig Config;
	GSVconfigLoadUser(Request.User, &Config);
	COLstring Error;
	if(!GSVmapExists(Config.m_ServerUrl)) {
		return COLrespondError(Request.Address, Config.m_ServerUrl + " not found in server list");
	}
	const GITUserver Server = GSVmapItem(Config.m_ServerUrl).m_Type;
	Result["host"]			  = GSVserviceName(Server);
	bool Success			  = GITElistOwners(Request.User, &Result, &Error);
	if(Success) {
		COLrespondSuccess(Request.Address, Result);
	} else {
		COL_TRC(Error);
		COLrespondError(Request.Address, Error);
	}
}