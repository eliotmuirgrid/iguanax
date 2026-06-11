// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCusername
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 04 April 2024 - 01:52PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <GITC/GITCusername.h>
#include <GSV/GSVconfig.h>
COL_LOG_MODULE;

// /git/username
void GITCusername(const COLwebRequest& Request) {
   COL_FUNCTION(GITCusername);
	GSVconfig Config;
	GSVconfigLoadUser(Request.User, &Config);
	COLvar Response;
	Response["username"] = Config.m_Username;
	return COLrespondSuccess(Request.Address, Response);
}