// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTconfig
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Wednesday 09 August 2023 - 12:36PM
// ---------------------------------------------------------------------------

#include <IWEBA/IWEBserverSet.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <GITC/GITCcredentials.h>
#include <GITC/GITCsetCredentials.h>
#include <IWEB/IWEBconfig.h>
#include <IWEB/IWEBload.h>
COL_LOG_MODULE;

bool INSTgetServerConfig(const COLstring& Username, COLvar* pConfig, COLstring* pError) {
   COL_FUNCTION(INSTgetServerConfig);
   IWEBconfig WebConfig;
   if (!IWEBload(&WebConfig, pError)) { return false; }
   (*pConfig)["server_name"] = WebConfig.serverDescription();
   GITCgetUserEmail("admin", pConfig);
   return true;
}

// /install/config/get
void INSTconfigGet(const COLwebRequest& Request) {
   COL_FUNCTION(INSTconfigGet);
   COLvar Result;
   COLstring Error;
   if (!INSTgetServerConfig("admin", &Result, &Error)) {
      return COLrespondError(Request.Address, "Failed to load server configuration: " + Error);
   }
   COLrespondSuccess(Request.Address, Result);
}

// /install/config/set
void INSTconfigSet(const COLwebRequest& Request) {
   COL_FUNCTION(INSTconfigSet);
   const COLvar& Data = Request.Data;
   COLstring Error;
   IWEBconfig WebConfig;
   if (!IWEBload(&WebConfig, &Error)) {
      return COLrespondError(Request.Address, "Failed to load server configuration: " + Error);
   }
   WebConfig.setServerDescription(COLvarRequiredString(Data, "server_name"));
   if (!IWEBserverSetSaveConfig(WebConfig, &Error)) {
      return COLrespondError(Request.Address, "Failed to save server name: " + Error);
   }
   GITCsetUserEmail("admin", COLvarRequiredString(Data, "name"), COLvarRequiredString(Data, "email"));
   COLrespondSuccess(Request.Address);
}
