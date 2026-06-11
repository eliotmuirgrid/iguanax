//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: IWEBserverRestart.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <IWEBA/IWEBserverRestart.h>
#include <IWEBA/IWEBserver.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
#include <WEB/WEBserver.h>
COL_LOG_MODULE;

static void IWEBstart(WEBserver* pWebServer, IWEBconfig* pWebConfiguration) {
   COL_FUNCTION(IWEBstart);
   if(pWebServer->m_Connections.size() > 0) {
      COL_TRC("Shutdown not completed.  Reschedule start a second");
      pWebServer->m_pLoop->addTimer(COLnewClosure0(&IWEBstart, pWebServer, pWebConfiguration), 1000);
      return;
   }
   COLstring Error;
   if(!IWEBserverInit(*pWebServer, *pWebConfiguration, false, &Error)) {
      SDBIlogService("Failed to start web server:\n" + Error, "#error #startup #web");
      throw COLerror(Error);  // This throw will kill the server.
   }
   COL_TRC("...web server has restarted");
   SDBIlogService("...web server has restarted", "#startup #web");
}

static void IWEBrestart(WEBserver* pWebServer, IWEBconfig* pWebConfiguration) {
   COL_FUNCTION(IWEBrestart);
   WEBserverSetResponseCompleteCallback(NULL);
   COL_TRC("Restarting web server...");
   SDBIlogService("Restarting web server...", "#web");
   pWebServer->shutdown();
   IWEBstart(pWebServer, pWebConfiguration);
}

static void IWEBrestartWrapper(SCKloop* pLoop, WEBserver* pWebServer, IWEBconfig* pWebConfiguration) {
   COL_FUNCTION(IWEBrestartWrapper);
   pLoop->addTimer(COLnewClosure0(IWEBrestart, pWebServer, pWebConfiguration));
}

// /server/restart
void IWEBserverRestart(const COLwebRequest& Request, WEBserver* pWebServer, IWEBconfig* pWebConfiguration,
                       SCKloop* pLoop) {
   COL_FUNCTION(IWEBserverRestart);
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::EDIT_WEBSERVER, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_WEBSERVER));
   }

   COLvar Result;
   Result["Message"] = "WebServer restarting...";
   // "schedule" for restart after sending the response
   WEBserverSetResponseCompleteCallback(COLnewClosure0(&IWEBrestartWrapper, pLoop, pWebServer, pWebConfiguration));
   COLrespondSuccess(Request.Address, Result);
   COL_TRC("About to run restarter.");
   WEBserverRunResponseCompleteCallback();
}
