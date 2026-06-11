//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: IWEBserverSet.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILutils.h>
#include <GINS/GINSaddAndCommit.h>
#include <IWEB/IWEBconfig.h>
#include <IWEB/IWEButils.h>
#include <IWEBA/IWEBAutils.h>
#include <IWEBA/IWEBserverSettingsPort.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
#include <WEB/WEBserver.h>
COL_LOG_MODULE;

// throws if required inputs are absent
static void IWEBserverSetParseInputs(const COLvar& Inputs, IWEBconfig* pWebConfig) {
   COL_FUNCTION(IWEBserverSetParseInputs);
   pWebConfig->setServerDescription(COLvarGet(Inputs, "description", ""));
   pWebConfig->setServerColour(COLvarRequiredString(Inputs, "color"));
   pWebConfig->setWebPort(COLintToString(COLvarRequiredInt(Inputs, "port")));
   pWebConfig->setUseHttps(COLvarRequiredBool(Inputs, "secure"));
   if(pWebConfig->useHttps()) {  // only save HTTPS info if HTTPS is selected
      pWebConfig->setHttpsCertificateKeyFile(COLvarRequiredString(Inputs, "cert_key"));
      pWebConfig->setHttpsPrivateKeyFile(COLvarRequiredString(Inputs, "private_key"));
   }
}

static bool IWEBserverSetCheckInputs(const IWEBconfig& NewWebConfig, const IWEBconfig& OldWebConfig, COLvar* pResult) {
   COL_FUNCTION(IWEBserverSetCheckInputs);
   bool Success = true;
   if(NewWebConfig.serverDescription().is_null()) {
      Success                         = false;
      (*pResult)["description_error"] = "Server description cannot be empty.";
   }
   // TODO use IWEBconfig::m_WebPort is an int, we could avoid the double conversion below
   if(!IWEBcheckPortRangeAndUse(atoi(NewWebConfig.webPort().data()), OldWebConfig, pResult)) {
      Success = false;
      // port_error is set by IWEBcheckPortRangeAndUse()
   }
   if(NewWebConfig.useHttps()) {
      COLstring Error;
      if(!IWEBtestSslCertFileValid(NewWebConfig.httpsCertificateKeyFile(), &Error)) {
         Success                         = false;
         (*pResult)["certificate_error"] = Error;  // So arbitrary - ugh (Eliot)
      }
      if(!IWEBtestSslKeyFileValid(NewWebConfig.httpsPrivateKeyFile(), &Error)) {
         Success                 = false;
         (*pResult)["key_error"] = Error;  // So arbitrary - ugh (Eliot)
      }
   }
   return Success;
}

bool IWEBserverSetSaveConfig(const IWEBconfig& WebConfig, COLstring* pError) {
   COL_FUNCTION(IWEBserverSetSaveConfig);
   COLvar ToSave = IWEBsetupConfigForSave(WebConfig);
   COL_VAR(ToSave);
   try {
      FILcreateDirectoryTreeToFile(DIRfileConfigWeb(), FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
      FILwriteFile(DIRfileConfigWeb(), ToSave.json(1));
      COL_TRC("Web config saved.");
      return true;
   } catch(const COLerror& Error) {
      *pError = Error.description();
      COL_ERR("Error: " << *pError);
      return false;
   }
}

static bool IWEBserverSetRequireRestart(const IWEBconfig& NewWebConfig, const IWEBconfig& OldWebConfig) {
   COL_FUNCTION(IWEBserverSetRequireRestart);
   if(NewWebConfig.webPort() != OldWebConfig.webPort()) {
      COL_TRC("Need to restart: port changed");
      return true;
   }
   if(NewWebConfig.useHttps() != OldWebConfig.useHttps()) {
      COL_TRC("Need to restart: changed server from plain to secure or vice versa");
      return true;
   }
   if(NewWebConfig.useHttps()) {
      if(NewWebConfig.httpsCertificateKeyFile() != OldWebConfig.httpsCertificateKeyFile()) {
         COL_TRC("Need to restart: server certificate changed");
         return true;
      }
      if(NewWebConfig.httpsPrivateKeyFile() != OldWebConfig.httpsPrivateKeyFile()) {
         COL_TRC("Need to restart: server private key changed");
         return true;
      }
   }
   return false;
}

// WORKER THREAD
static void IWEBserverSetProcess(COLwebRequest Request, IWEBconfig WebConfig, SCKloop* pLoop,
                                 COLclosure3<COLstring, COLvar, IWEBconfig>* pCallback) {
   COL_FUNCTION(IWEBserverSetProcess);
   const COLvar& Input = Request.Data;
   IWEBconfig    NewWebConfig(WebConfig);
   COLstring     ErrMsg;
   COLvar        Result;
   do {
      try {
         IWEBserverSetParseInputs(Input, &NewWebConfig);
      } catch(const COLerror& Error) {
         ErrMsg = Error.description();
         break;
      }
      // Intentionally not returning on first error here, the popup displays all error feedback at once.
      if(!IWEBserverSetCheckInputs(NewWebConfig, WebConfig, &Result)) {
         ErrMsg = "There was an issue configuring the web server with the provided values.";
         break;
      }
      if(!IWEBserverSetSaveConfig(NewWebConfig, &ErrMsg)) { break; }
      if(IWEBserverSetRequireRestart(NewWebConfig, WebConfig)) { Result["restart"] = true; }
   } while(false);
   COLstring Error;
   COLvar    Components;
   COLvar    In;
   In["user"]        = Request.User;
   In["message"]     = "Saved the web server settings.";
   In["allow_empty"] = true;
   In["settings"]    = true;
   GINSaddAndCommitImpl(In, &Error);
   pLoop->post(COLnewClosure0(pCallback, &COLclosure3<COLstring, COLvar, IWEBconfig>::runAndDelete, ErrMsg, Result,
                              NewWebConfig));
}

static void IWEBserverSetRespond(COLstring ErrMsg, COLvar Result, IWEBconfig NewWebConfig, COLaddress Address,
                                 IWEBconfig* pWebConfig) {
   COL_FUNCTION(IWEBserverSetRespond);
   if(ErrMsg.is_null()) {
      COL_TRC("Server settings saved.");
      *pWebConfig = NewWebConfig;  // update in-memory web config
      return COLrespondSuccess(Address, Result);
   }
   Result.isNull() ? COLrespondError(Address, ErrMsg) : COLrespondError(Address, ErrMsg, Result);
}

void IWEBserverSetImpl(const COLwebRequest& Request, IWEBconfig* pWebConfig, SCKloop* pLoop) {
   COL_FUNCTION(IWEBserverSetImpl);
   COLclosure3<COLstring, COLvar, IWEBconfig>* pCallback = COLnewClosure3(&IWEBserverSetRespond, Request.Address, pWebConfig);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&IWEBserverSetProcess, Request, *pWebConfig, pLoop, pCallback));
}

// /web/server/set
void IWEBserverSet(const COLwebRequest& Request, WEBserver* pServer, IWEBconfig* pWebConfig, SCKloop* pLoop) {
   COL_FUNCTION(IWEBserverSet);
   COL_VAR(Request);
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::EDIT_WEBSERVER, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_WEBSERVER));
   }
   if(pServer->m_pOnRestartGrid) { pServer->m_pOnRestartGrid->run(); }
   IWEBserverSetImpl(Request, pWebConfig, pLoop);
}
