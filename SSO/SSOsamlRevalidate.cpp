//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOsamlRevalidate.cpp
//
//  Description: SAML revalidation - refresh user session and roles from IdP
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DRX/DRXutils.h>
#include <GUID/GUIDid.h>
#include <SCK/SCKworker.h>
#include <SSO/SSOconfig.h>
#include <SSO/SSOsamlRevalidate.h>
#include <SSO/SSOsamlUtils.h>
#include <SSO/SSOsession.h>
#include <WEB/WEBsession.h>
#include <time.h>
#include <zlib.h>
COL_LOG_MODULE;

static bool SSOsamlRevalidateLogin(const SSOprovider Provider, const SSOsamlConfig& Config, const COLstring& Hash,
                                   COLstring& RedirectUrl, COLstring& Error) {
   COL_FUNCTION(SSOsamlRevalidateLogin);
   if(Config.m_SsoLoginUrl.is_null()) {
      Error = "SAML login URL not configured";
      COL_TRC(Error);
      return false;
   }

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
      COL_TRC("SAML Revalidation AuthnRequest: " << SamlRequest);

      const COLstring RelayState = "revalidate:" + Hash;
      return SSOsamlBuildRedirectUrl(Provider, Config, SamlRequest, RelayState, RedirectUrl, Error);
   } catch(const COLerror& E) {
      Error = "Failed to initiate SAML revalidation: " + E.description();
      COL_TRC(Error);
      return false;
   }
}

static void SSOsamlRevalidateCallback(const COLvar Result, const COLaddress Address) {
   COL_FUNCTION(SSOsamlRevalidateCallback);
   if(!Result["success"].asBool()) { return COLrespondError(Address, Result["error"].asString()); }
   COLrespondSuccess(Address, Result["data"]);
}

static void SSOsamlRevalidateWorker(const COLvar& In, COLvar* pOut) {
   COL_FUNCTION(SSOsamlRevalidateWorker);
   COLvar&   Out = *pOut;
   COLstring Error;

   if(!DRXsingleSignOnEnabled()) {
      COL_TRC("SSO not licensed");
      Out["success"] = false;
      Out["error"]   = SSO_LICENSE_ERROR;
      return;
   }

   const SSOconfig Config = SSOgetConfig();
   COL_TRC("Retrieved SSO config for revalidation, mode: " << SSOmodeToString(Config.m_Mode));

   COLstring RedirectUrl;
   if(!SSOsamlRevalidateLogin(Config.m_Provider, Config.m_SamlConfig, In["hash"], RedirectUrl, Error)) {
      COL_TRC("Failed to initiate SAML revalidation: " << Error);
      Out["success"] = false;
      Out["error"]   = Error;
      return;
   }
   Out["success"]              = true;
   Out["data"]["redirect_url"] = RedirectUrl;
}

// /auth/saml/revalidate
void SSOsamlRevalidate(const COLwebRequest& Request, COLmap<COLstring, WEBsession>* pSessionMap, SCKloop* pLoop) {
   COL_FUNCTION(SSOsamlRevalidate);
   const COLstring SessionId = Request.SessionId;
   if(SessionId.is_null() || pSessionMap->count(SessionId) == 0) {
      COL_TRC("No valid session for revalidation");
      return COLrespondError(Request.Address, "No valid session");
   }
   const WEBsession& Session  = (*pSessionMap)[SessionId];
   const SSOprovider Provider = SSOproviderToEnum(Session.m_SsoProvider);
   if(Provider != MICROSOFT && Provider != OKTA && Provider != GOOGLE) {
      COL_TRC("Session is not SAML-based");
      return COLrespondError(Request.Address, "Not a SAML session");
   }
   WEBdeleteSession(pSessionMap, SessionId);
   COLvar Data;
   Data["username"] = Request.User;
   Data["hash"]     = COLvarGetString(Request.Data, "hash", "");
   SCKinvokeWorker(pLoop, SSOsamlRevalidateWorker, Data, COLnewClosure1(&SSOsamlRevalidateCallback, Request.Address));
}
