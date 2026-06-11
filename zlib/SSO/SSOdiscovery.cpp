//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOdiscovery.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <SSO/SSOdiscovery.h>
#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>
COL_LOG_MODULE;

const COLstring SSO_MICROSOFT_URL = "https://login.microsoftonline.com/common/v2.0/.well-known/openid-configuration";

static SSOdiscoveryCache s_DocCache;
static const int         DOC_CACHE_DURATION = 86400;  // 24 hours in seconds

COLvar SSOgetDiscoveryDoc() { return s_DocCache.m_DiscoveryDoc; }
bool SSOdocLoaded(const SSOprovider& Provider) {
   COL_VAR(s_DocCache.m_DiscoveryDoc);
   if(!s_DocCache.m_DiscoveryDoc.isMap()) { return false; }
   switch(Provider) {
      case MICROSOFT: return s_DocCache.m_DiscoveryDoc["issuer"].asString().find("https://login.microsoftonline.com") == 0;
      default: return true;  // if it is not a provider we recognize no point trying to load a new doc
   }
}

static bool SSOmakeDiscoveryRequest(const COLstring& Url, WEBresponse* pResponse, COLstring* pError) {
   COL_FUNCTION(SSOmakeDiscoveryRequest);
   WEBrequest Request;
   Request.Url                   = Url;
   Request.Headers["Accept"]     = "application/json";
   Request.Headers["User-Agent"] = "IguanaX-SSO";
   Request.Timeout               = 10000;
   WEBrequestBlockingGet(Request, pResponse);
   COL_VAR2(pResponse->ReturnCode, pResponse->Body.size());
   if(pResponse->ReturnCode != 200) {
      *pError = "Failed to fetch discovery document. HTTP " + COLint32ToString(pResponse->ReturnCode);
      COL_ERR(*pError);
      COL_VAR(pResponse->Body);
      return false;
   }
   return true;
}

static bool SSOparseDiscoveryResponse(const COLstring& ResponseBody, COLvar* pDiscoveryDoc, COLstring* pError) {
   COL_FUNCTION(SSOparseDiscoveryResponse);
   if(!pDiscoveryDoc->parse(ResponseBody)) {
      *pError = "Failed to parse discovery document JSON";
      COL_ERR(*pError);
      COL_VAR(ResponseBody);
      return false;
   }
   return true;
}

static bool SSOvalidateDiscoveryDocument(const COLvar& DiscoveryDoc, COLstring* pError) {
   COL_FUNCTION(SSOvalidateDiscoveryDocument);
   if(!DiscoveryDoc.exists("issuer") || !DiscoveryDoc.exists("jwks_uri")) {
      *pError = "Invalid discovery document - missing required fields";
      COL_TRC(*pError);
      COL_VAR(DiscoveryDoc.json(1));
      return false;
   }
   return true;
}

static bool SSOloadDiscoveryDocument(const COLstring& Url, COLvar* pDiscoveryDoc, COLstring* pError) {
   COL_FUNCTION(SSOloadDiscoveryDocument);
   try {
      WEBresponse Response;
      if(!SSOmakeDiscoveryRequest(Url, &Response, pError)) { return false; }
      if(!SSOparseDiscoveryResponse(Response.Body, pDiscoveryDoc, pError)) { return false; }
      if(!SSOvalidateDiscoveryDocument(*pDiscoveryDoc, pError)) { return false; }
      COL_TRC("Successfully loaded discovery document");
      COL_VAR(pDiscoveryDoc->json(1));
      return true;
   } catch(const COLerror& e) {
      *pError = "Exception loading discovery document: " + e.description();
      COL_TRC(*pError);
      return false;
   }
}

bool SSOdiscoveryDocLoaded(const SSOprovider Provider, COLstring& Error) {
   COL_FUNCTION(SSOdiscoveryDocLoaded);
   const COLstring Url          = SSO_MICROSOFT_URL;
   const COLstring ProviderName = SSOproviderToString(Provider);
   const time_t    Now          = time(NULL);
   const bool      UseCache = s_DocCache.m_Provider == Provider && Now - s_DocCache.m_CacheTime < DOC_CACHE_DURATION &&
                         !SSOdocLoaded(Provider);

   if(UseCache) {
      COL_TRC("Using cached discovery document for " << ProviderName);
      return true;
   }

   COL_TRC("Discovery document not cached or expired, fetching from server for " << ProviderName);
   COLvar Doc;
   if(!SSOloadDiscoveryDocument(Url, &Doc, &Error)) {
      COL_ERR("Failed to load discovery document for " << ProviderName << ": " << Error);
      return false;
   }

   s_DocCache.m_DiscoveryDoc = Doc;
   s_DocCache.m_CacheTime    = Now;
   s_DocCache.m_Provider     = Provider;
   COL_TRC("Successfully loaded and cached discovery document for " << ProviderName);
   return true;
}