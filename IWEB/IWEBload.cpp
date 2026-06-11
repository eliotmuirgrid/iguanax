// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IWEBload
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Sunday 18 September 2022 - 06:29PM
// ---------------------------------------------------------------------------
#include <IWEB/IWEBload.h>
#include <IWEB/IWEBconfig.h>
#include <IWEB/IWEBsave.h>

#include <FIL/FILatomic.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <DIR/DIRpath.h>

#include <CRT/CRTselfSigned.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool IWEBcreateDefaultCertKey() {
   COL_FUNCTION(IWEBcreateDefaultCertKey);
   if (FILfileExists(DIRfileConfigWebCert()) && FILfileExists(DIRfileConfigWebKey())) {
      return true;
   }
   COLstring Error;
   bool Success = CRTgenerateSelfSignedCertKey(&Error);
   if (!Success) {
      COL_ERR(Error);
      COLcout << Error << newline;
   }
   return Success;
}

bool IWEBloadImp(IWEBconfig *pConfig, const COLstring& FileName){
   COL_FUNCTION(IWEBloadImp);
   COL_VAR(FileName);
   IWEBconfig &Config = *pConfig;
   COLvar ConfigVar;
   COL_TRC("loading web config");
   if (!FILserializeLoadConfig(&ConfigVar, FileName)){
      COL_ERR("No file loaded so we are using defaults.");
      IWEBsave(Config);
      return true;
   } else {
      COL_TRC("Loaded the configuration file.");
      COL_VAR(ConfigVar);
   }
   // use default setting for bad config value
   if (ConfigVar.exists("description"))
      Config.setServerDescription(ConfigVar["description"]);
   if (ConfigVar.exists("color"))
      Config.setServerColour(ConfigVar["color"]);
   if (ConfigVar.exists("guid")){
      Config.setUniqueId(ConfigVar["guid"].asString());
   }
   // if(ConfigVar.exists("session_timeout") && ConfigVar["session_timeout"].asInt() >= 300) Config.setWebSessionTimeout(ConfigVar["session_timeout"].asInt());
   if (ConfigVar.exists("port") && ConfigVar["port"].asInt() >= 0){
      Config.setWebPort(ConfigVar["port"]);
   }
   if (ConfigVar.exists("secure")){
      Config.setUseHttps(ConfigVar["secure"]);
   }
   if (ConfigVar.exists("cert_key")){
      Config.setHttpsCertificateKeyFile(ConfigVar["cert_key"]);
   }
   if (ConfigVar.exists("private_key")){
      Config.setHttpsPrivateKeyFile(ConfigVar["private_key"]);
   }
   return true;
}

bool IWEBload(IWEBconfig *pConfig, COLstring* pError){
   COL_FUNCTION(IWEBload);
   try{
      IWEBloadImp(pConfig, DIRfileConfigWeb());
      return true;
   } catch (const COLerror& Error){
      *pError = Error.description();
      return false;
   }
}