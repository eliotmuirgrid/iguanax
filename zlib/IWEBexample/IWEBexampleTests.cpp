//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IWEBexampleTests.cpp
//
// Description:
//
// A set of utilities to test the IWEB
//
// Author: Shuo Hou
// Date:   March 12, 2021
//
//
//---------------------------------------------------------------------------
#include "IWEBexampleTests.h"

#include <IWEBA/IWEBserverSet.h>
#include <IWEBA/IWEBserverSettings.h>
#include <IWEBA/IWEBserverSettingsCert.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <FIL/FILatomic.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <IWEB/IWEBconfig.h>
#include <IWEB/IWEBload.h>
#include <IWEBA/IWEBserverSettingsPort.h>
#include <IWEB/IWEButils.h>
#include <SCK/SCKloop.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

void IWEBresponder(const COLvar& Response, COLvar* pOutput) {
   COL_FUNCTION(IWEBresponder);
   *pOutput = Response;
   COL_VAR(*pOutput);
}

static void IWEBresponderWithLoop(const COLvar& Response, COLvar* pOutput, SCKloop* pLoop) {
   COL_FUNCTION(IWEBresponderWithLoop);
   *pOutput = Response;
   COL_VAR(*pOutput);
   COL_TRC("Shutting down loop");
   pLoop->shutdown();
}

void IWEBtestSaveConfigToFile() {
   COL_FUNCTION(IWEBtestSaveConfigToFile);
   IWEBconfig WebConfig;
   WebConfig.setServerDescription("testingSave");
   WebConfig.setWebPort("9876");
   COLvar Config = IWEBsetupConfigForSave(WebConfig);
   UNIT_ASSERT(Config["description"].asString() == "testingSave");
   UNIT_ASSERT(Config["port"].asString() == "9876");
}

void IWEBtestLoadConfigFromFile() {
   COL_FUNCTION(IWEBtestLoadConfigFromFile);
   COLvar Config;
   Config["description"] = "testingLoad";
   Config["port"] = "6789";
   FILserializeSaveConfig(Config, FILpathAppend(".", "web.json.load"));
   IWEBconfig WebConfig;
   COLstring Error;
   UNIT_ASSERT(IWEBload(&WebConfig, &Error));
   FILremoveWithThrow("web.json.load", true);
}

void IWEBtestSettingsApiGet() {
   COL_FUNCTION(IWEBtestSettingsApiGet);
   IWEBconfig WebConfig; // use default settings
   COLvar Output;
   IWEBpopulateServerInfo(&Output, &WebConfig);
   COL_VAR(Output);
   // UNIT_ASSERT(Output["session_timeout"].asString() == "60");
   UNIT_ASSERT(Output["port"].asString() == "7654");
   UNIT_ASSERT(Output["secure"].asString() == "false");
}

static void IWEBresponderBadPort(const COLvar& Response, COLvar* pOutput, SCKloop* pLoop) {
   COL_FUNCTION(IWEBresponderBadPort);
   *pOutput = Response;
   COL_VAR(*pOutput);
   COL_TRC("Shutting down loop");
   pLoop->shutdown();
   UNIT_ASSERT((*pOutput)["success"].asBool() == false); // error code
   UNIT_ASSERT((*pOutput).exists("error"));
}

void IWEBtestSettingsApiPostInvalidPort() {
   COL_FUNCTION(IWEBtestSettingsApiPostInvalidPort);
   IWEBconfig WebConfig;
   COLvar Input;
   Input["port"] = "0"; // bad port
   Input["secure"] = false; // don't bother checking cert/private key
   Input["color"] = "#FFFF00";
   COLvar Output, Config;
   COLthreadPool ThreadPool;
   ThreadPool.start();
   SCKloop Loop(&ThreadPool);
   // We return before saving since the config is bad.
   COLwebRequest Request;
   Request.Data = Input;
   Request.Address.ConnectionId = -1;
   Request.Address.pInternalResponseCallback = COLnewClosure1(IWEBresponderBadPort, &Output, &Loop);
   IWEBserverSetImpl(Request, &WebConfig, &Loop);
   COL_TRC("Starting SCKloop");
   Loop.start();
   COL_TRC("Exited SCKloop");
}

// void IWEBtestSettingsApiPostInvalidSessionTimeout(){
//    COL_FUNCTION(IWEBtestSettingsApiPostInvalidSessionTimeout);
//    IWEBconfig WebConfig;
//    COLvar Input;
//    Input["method"] = "POST";
//    Input["data"]["session_timeout"] = -1;
//    COLvar Output;
//    IWEBserverSettings(Input,COLnewClosure1(IWEBresponder,&Output),&WebConfig);
//    UNIT_ASSERT(Output["success"].asBool() == false);
//    UNIT_ASSERT(Output["session_timeout"].asString() == "60");   // timeout unchanged
//    UNIT_ASSERT(Output.exists("session_error"));
// }

static void IWEBresponderBadCert(const COLvar& Response, COLvar* pOutput, SCKloop* pLoop) {
   COL_FUNCTION(IWEBresponderBadCert);
   *pOutput = Response;
   COL_VAR(*pOutput);
   COL_TRC("Shutting down loop");
   pLoop->shutdown();
   UNIT_ASSERT((*pOutput)["success"].asBool() == false);
   UNIT_ASSERT((*pOutput).exists("certificate_error"));
   UNIT_ASSERT((*pOutput).exists("key_error"));
}

void IWEBtestSettingsApiPostInvalidCert() {
   COL_FUNCTION(IWEBtestSettingsApiPostInvalidCert);
   IWEBconfig WebConfig;
   COLvar Input;
   COLvar Output, Config;
   Input["port"] = 8888;  // hopefully this isn't in use...
   Input["secure"] = true;  // use https, but cert and private key are invalid
   Input["cert_key"] = "DoesNotExist";
   Input["private_key"] = "DoesNotExist";
   Input["color"] = "#FFFF00";
   COLthreadPool ThreadPool;
   ThreadPool.start();
   SCKloop Loop(&ThreadPool);
   // We return before saving since the config is bad.
   COLwebRequest Request;
   Request.Data = Input;
   Request.Address.ConnectionId = -1;
   Request.Address.pInternalResponseCallback = COLnewClosure1(IWEBresponderWithLoop, &Output, &Loop);
   IWEBserverSetImpl(Request, &WebConfig, &Loop);
   COL_TRC("Starting SCKloop");
   Loop.start();
   COL_TRC("Exited SCKloop");
}

// void IWEBtestCheckSessionTimeout(){
//    COL_FUNCTION(IWEBtestCheckSessionTimeout);
//    COLvar Input;
//    COLvar Output;
//    // bad input
//    Input["data"]["session_timeout"] = -1;
//    IWEBcheckSessionTimeout(Input,COLnewClosure1(IWEBresponder,&Output));
//    UNIT_ASSERT(Output["success"].asBool() == false);
//    UNIT_ASSERT(Output.exists("session_error"));
//    // good input
//    Input["data"]["session_timeout"] = 500;
//    IWEBcheckSessionTimeout(Input,COLnewClosure1(IWEBresponder,&Output));
//    UNIT_ASSERT(Output["success"].asBool() == true);
// }

void IWEBtestCheckWebPort() {
   COL_FUNCTION(IWEBtestCheckWebPort);
   IWEBconfig WebConfig;
   COLvar Input;
   COLvar Output;
   // bad input
   Input["port"] = "-1";
   COLwebRequest Request;
   Request.Data = Input;
   Request.Address.ConnectionId = -1;
   Request.Address.pInternalResponseCallback = COLnewClosure1(IWEBresponder, &Output);
   IWEBserverPort(Request, &WebConfig);
   COL_VAR(Output);
   UNIT_ASSERT(Output["success"].asBool() == false);
   UNIT_ASSERT(Output.exists("error"));
   // good input
   Input["port"] = 2468;
   COLwebRequest Request2;
   Request2.Data = Input;
   Request2.Address.ConnectionId = -1;
   Request2.Address.pInternalResponseCallback = COLnewClosure1(IWEBresponder, &Output);
   IWEBserverPort(Request2, &WebConfig);
   UNIT_ASSERT(Output["success"].asBool() == true);
}

void IWEBtestCheckCertificate() {
   COL_FUNCTION(IWEBtestCheckCertificate);
   COLvar Input;
   COLvar Output;
   // bad input
   Input["file"] = "";
   COLwebRequest Request;
   Request.Data = Input;
   Request.Address.pInternalResponseCallback = COLnewClosure1(IWEBresponder, &Output);
   IWEBserverCertificate(Request);
   COL_VAR(Output);
   UNIT_ASSERT(Output["success"].asBool() == false);
   UNIT_ASSERT(Output.exists("error"));

   // need to put in cert key file to test success

   // Input["data"]["cert_key"] = "";
   // COL_VAR(Input);
   // IWEBcheckCertificate(Input,&Output);
   // UNIT_ASSERT(Output["code"].asString() == "200");
}

void IWEBtestPlace(UNITapp* pApp) {
   pApp->add("IWEBtestPlace/SaveConfig",        &IWEBtestSaveConfigToFile);
   pApp->add("IWEBtestPlace/LoadConfig",        &IWEBtestLoadConfigFromFile);
   pApp->add("IWEBtestPlace/GetApi",            &IWEBtestSettingsApiGet);
   pApp->add("IWEBtestPlace/PostApiBadPort",    &IWEBtestSettingsApiPostInvalidPort);
   pApp->add("IWEBtestPlace/PostApiBadCert",    &IWEBtestSettingsApiPostInvalidCert);
   pApp->add("IWEBtestPlace/ValidatePort",      &IWEBtestCheckWebPort);
   pApp->add("IWEBtestPlace/ValidateCert",      &IWEBtestCheckCertificate);
}
