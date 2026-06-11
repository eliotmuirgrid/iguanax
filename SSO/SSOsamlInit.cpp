//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOsaml.cpp
//
//  Description: SAML SSO Authentication Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DRX/DRXutils.h>
#include <FIL/FILutils.h>
#include <GUID/GUIDid.h>
#include <SCK/SCKworker.h>
#include <SSO/SSOconfig.h>
#include <SSO/SSOsamlInit.h>
#include <SSO/SSOsamlUtils.h>
#include <time.h>
#include <zlib.h>
COL_LOG_MODULE;

static bool SSOsamlInitiateLogin(const SSOprovider Provider, const SSOsamlConfig& Config, COLstring& RedirectUrl,
                                 COLstring& Error) {
   COL_FUNCTION(SSOsamlInitiateLogin);
   if(Config.m_SsoLoginUrl.is_null()) {
      Error = "SAML login URL not configured";
      COL_TRC(Error);
      return false;
   }
   COL_VAR3(Config.m_SsoLoginUrl, Config.m_AcsUrl, Config.m_EntityId);

   try {
      SSOsamlRequestParams Params;
      Params.RequestId   = GUIDnewId();
      Params.Timestamp   = SSOsamlFormatIso8601(time(NULL));
      Params.Binding     = Provider == GOOGLE ? "Redirect" : "POST";
      Params.Destination = Config.m_SsoLoginUrl;
      Params.AcsUrl      = Config.m_AcsUrl;
      Params.EntityId    = Config.m_EntityId;
      Params.ForceAuthn  = false;
      Params.IsPassive   = false;

      const COLstring SamlRequest = SSOsamlBuildAuthnRequest(Params);
      COL_TRC("SAML AuthnRequest: " << SamlRequest);

      return SSOsamlBuildRedirectUrl(Provider, Config, SamlRequest, COLstring(), RedirectUrl, Error);
   } catch(const COLerror& E) {
      Error = "Failed to initiate SAML login: " + E.description();
      COL_TRC(Error);
      return false;
   }
}

static void SSOsamlInitCallback(const COLvar Result, const COLaddress Address) {
   COL_FUNCTION(SSOsamlInitCallback);
   if(!Result["success"].asBool()) { return COLrespondError(Address, Result["error"].asString()); }
   COLrespondSuccess(Address, Result["data"]);
}

static void SSOsamlInitWorker(const COLvar& In, COLvar* pOut) {
   COL_FUNCTION(SSOsamlInitWorker);
   COLvar&   Out = *pOut;
   COLstring Error;

   if(!DRXsingleSignOnEnabled()) {
      COL_TRC("SSO not licensed");
      Out["success"] = false;
      Out["error"]   = SSO_LICENSE_ERROR;
      return;
   }

   const SSOconfig Config = SSOgetConfig();
   COL_TRC("Retrieved SSO config, mode: " << SSOmodeToString(Config.m_Mode));

   COLstring RedirectUrl;
   if(!SSOsamlInitiateLogin(Config.m_Provider, Config.m_SamlConfig, RedirectUrl, Error)) {
      COL_TRC("Failed to initiate SAML login: " << Error);
      Out["success"] = false;
      Out["error"]   = Error;
      return;
   }
   Out["success"]              = true;
   Out["data"]["redirect_url"] = RedirectUrl;
}

// /auth/saml/initiate
void SSOsamlInit(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(SSOsamlInit);
   SCKinvokeWorker(pLoop, SSOsamlInitWorker, Request.Data, COLnewClosure1(&SSOsamlInitCallback, Request.Address));
}