//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: IWEBserverSettings.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <IWEB/IWEBconfig.h>
#include <IWEBA/IWEBserverSettings.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKutils.h>
COL_LOG_MODULE;

// WORKER THREAD
static void IWEBpopulateHostNameAndIp(COLvar* pResult, COLaddress Address) {
   COL_FUNCTION(IWEBpopulateHostNameAndIp);
   COLvar& Result = *pResult;
   Result["host"]        = SCKlocalHostName();     // should be pretty fast
   Result["ip"]          = SCKlocalIpAsString();   // could be very slow on macOS
   COL_VAR(Result.json(1));
   COLrespondSuccess(Address, Result);
   delete pResult;
}

void IWEBpopulateServerInfo(COLvar* pResult, IWEBconfig* pWebConfig){
   COL_FUNCTION(IWEBpopulateServerInfo);
   COLvar& Result = *pResult;
   Result["description"] = pWebConfig->serverDescription();
   Result["color"]       = pWebConfig->serverColour();
   Result["port"]        = pWebConfig->webPort();
   Result["secure"]      = pWebConfig->useHttps();
   Result["cert_key"]    = pWebConfig->httpsCertificateKeyFile();
   Result["private_key"] = pWebConfig->httpsPrivateKeyFile();
}

// /web/server
void IWEBserverSettings(const COLwebRequest& Request, IWEBconfig* pWebConfig, SCKloop* pLoop) {
   COL_FUNCTION(IWEBserverSettings);
   const COLvar& Input = Request.Data;
   COL_VAR(Input.json(1));
   COLvar* pResult = new COLvar();
   IWEBpopulateServerInfo(pResult, pWebConfig);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&IWEBpopulateHostNameAndIp, pResult, Request.Address));
}