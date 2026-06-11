// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCsetUsername
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 04 April 2024 - 12:57PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <GITC/GITCsetUsername.h>
#include <GSV/GSVconfig.h>
COL_LOG_MODULE;

static void GITCsetUsernameImpl(const COLstring& Username, const COLstring& Name, const COLaddress& Address) {
   COL_FUNCTION(GITCsetUsernameImpl);
   GSVconfig Config;
   GSVconfigLoadUser(Username, &Config);
   Config.m_Username = Name;
   COL_TRC("Set username to " + Config.m_Username);
   COLstring ErrorMsg;
   if (!GSVconfigSaveUser(Username, Config, &ErrorMsg)) { return COLrespondError(Address, ErrorMsg); }
	return COLrespondSuccess(Address);
}

// /git/username/set
void GITCsetUsername(const COLwebRequest& Request) {
   COL_FUNCTION(GITCsetUsername);
   const COLvar& Data = Request.Data;
   COL_VAR(Data);
   COLstring Username = Request.User;
   COLstring Name  = COLvarRequiredString(Data, "username");
   GITCsetUsernameImpl(Username, Name, Request.Address);
}