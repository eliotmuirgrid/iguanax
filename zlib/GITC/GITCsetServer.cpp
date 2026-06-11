// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCsetServer
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 10 June 2024 - 3:21PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <GITC/GITCsetServer.h>
#include <GSV/GSVconfig.h>
COL_LOG_MODULE;

static void GITCsetserverImpl(const COLstring& Username, const COLstring& Server, const COLaddress& Address) {
   COL_FUNCTION(GITCsetserverImpl);
   GSVconfig Config;
   GSVconfigLoadUser(Username, &Config);
   COLstring ServerUrl = Server; 
   if(ServerUrl.size() > 2 && ServerUrl[ServerUrl.size() - 1] == '/' ) {
      COL_TRC("Standardizing the server to not end in /");
      ServerUrl = ServerUrl.substr(0, ServerUrl.size() - 1);
   }
	Config.m_ServerUrl = ServerUrl;
   COLstring ErrorMsg;
   if (!GSVconfigSaveUser(Username, Config, &ErrorMsg)) { return COLrespondError(Address, ErrorMsg); }
	return COLrespondSuccess(Address);

}

// /git/server/set
void GITCsetServer(const COLwebRequest& Request) {
   COL_FUNCTION(GITCsetServer);
   const COLvar& Data = Request.Data;
   COL_VAR(Data);
   COLstring Username = Request.User;
   COLstring Server  = COLvarRequiredString(Data, "server");
   GITCsetserverImpl(Username, Server, Request.Address);
}