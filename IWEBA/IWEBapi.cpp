//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: IWEBapi.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <IWEB/IWEBconfig.h>
#include <IWEBA/IWEBapi.h>
#include <IWEBA/IWEBserverRestart.h>
#include <IWEBA/IWEBserverSet.h>
#include <IWEBA/IWEBserverSettings.h>
#include <IWEBA/IWEBserverSettingsCert.h>
#include <IWEBA/IWEBserverSettingsPort.h>
#include <IWEBA/IWEBserverSettingsTimeout.h>
COL_LOG_MODULE;

void IWEBapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, IWEBconfig* pWebConfig, WEBserver* pServer) {
   COL_FUNCTION(IWEBapi);
   ApiMap.add("/web/server",                    COLnewClosure1(&IWEBserverSettings, pWebConfig, pLoop));
   ApiMap.add("/web/server/set",                COLnewClosure1(&IWEBserverSet,  pServer, pWebConfig, pLoop));
   ApiMap.add("/web/server/port",               COLnewClosure1(&IWEBserverPort, pWebConfig));
   ApiMap.add("/web/server/certificate/check",  COLnewClosure1(&IWEBserverCertificate));
   // ApiMap.add("/web/server/session_timeout", COLnewClosure2(&IWEBcheckSessionTimeout)); // TODO: not yet implemented on the front end
   ApiMap.add("/server/restart",                COLnewClosure1(&IWEBserverRestart, pServer, pWebConfig, pLoop));
}
