//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOconfig.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <SSO/SSOconfig.h>
COL_LOG_MODULE;

const COLstring SSO_LICENSE_ERROR = "Your license type does not support SSO. SSO features require an enterprise license.";

static SSOconfig s_SSOconfig;

SSOconfig SSOgetConfig()       { return s_SSOconfig; }
int SSOgetValidationInterval() { return s_SSOconfig.m_TokenRevalidationInterval; }

static void SSOclearClientConfig() {
   s_SSOconfig.m_ClientConfig.m_ClientId       = "";
   s_SSOconfig.m_ClientConfig.m_OrganizationId = "";
}

static void SSOclearSamlConfig() {
   s_SSOconfig.m_SamlConfig.m_AcsUrl       = "";
   s_SSOconfig.m_SamlConfig.m_EntityId     = "";
   s_SSOconfig.m_SamlConfig.m_MetadataPath = "";
}

void SSOclearConfig() {
   s_SSOconfig.m_Mode     = DISABLED;
   s_SSOconfig.m_Provider = NONE;
   SSOclearClientConfig();
   SSOclearSamlConfig();
}

void SSOsetConfig(const SSOconfig& Config) {
   s_SSOconfig = Config;
   s_SSOconfig.m_TokenRevalidationInterval = s_SSOconfig.m_Mode == SAML ? 3600 : 300;
}

COLstring SSOmodeToString(const SSOmode& Mode) {
   switch(Mode) {
      case CLIENT: return "Client";
      case SAML:   return "SAML";
      default:     return "Disabled";
   }
}

SSOmode SSOmodeToEnum(const COLstring& Mode) {
   if(COLlowerCase(Mode) == "client") { return CLIENT; }
   if(COLlowerCase(Mode) == "saml")   { return SAML; }
   return DISABLED;
}

COLstring SSOproviderToString(const SSOprovider& Provider) {
   switch(Provider) {
      case GOOGLE:    return "Google";
      case MICROSOFT: return "Microsoft";
      case OKTA:      return "Okta";
      default:        return "None";
   }
}

SSOprovider SSOproviderToEnum(const COLstring& Provider) {
   if(COLlowerCase(Provider) == "google")    { return GOOGLE; }
   if(COLlowerCase(Provider) == "microsoft") { return MICROSOFT; }
   if(COLlowerCase(Provider) == "okta")      { return OKTA; }
   return NONE;
}