//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITCserversSave.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   04/07/24 1:34 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <GITC/GITCserversSave.h>
#include <GSV/GSVmap.h>
#include <PRM/PRMcallbacks.h>
COL_LOG_MODULE;

// git/servers/save
void GITCserversSave(const COLwebRequest& Request) {
	COL_FUNCTION(GITCserversSave);
	COLstring Error;
	if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::EDIT_GITSERVERS, &Error)) {
		return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_GITSERVERS));
	}
	bool Success = GSVmapSet(Request.Data);
	if(!Success) { return COLrespondError(Request.Address, "Invalid server data"); }
	const COLvar ServerConfig = GSVmaptoVar();
	COLstring Err;
	Success = FILsave(DIRfileConfigServers(), GSVmaptoVar().json(1), &Err);
	Success ? COLrespondSuccess(Request.Address) : COLrespondError(Request.Address, Err);
}