#ifndef MAIN2PROJECT_SSOCONFIG_H
#define MAIN2PROJECT_SSOCONFIG_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOconfig.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLvar.h>

extern const COLstring SSO_LICENSE_ERROR;

enum SSOmode {
   DISABLED,
   CLIENT,
   SAML,
};

enum SSOprovider {
   NONE,
   GOOGLE,
   MICROSOFT,
   OKTA,
};

struct SSOclientConfig {
   COLstring   m_ClientId                  = "";
   COLstring   m_OrganizationId            = "";
};

struct SSOsamlConfig {
   // Service Provider (our) settings
   COLstring m_EntityId     = "";  // Our Entity ID
   COLstring m_AcsUrl       = "";  // Our Assertion Consumer Service URL
   COLstring m_MetadataPath = "";  // Path to Federation Metadata XML file

   // Identity Provider settings (loaded from metadata)
   COLstring m_IdpEntityId  = "";  // IDP Entity ID
   COLstring m_SsoLoginUrl  = "";  // Single Sign-On Service URL
   COLstring m_SloLogoutUrl = "";  // Single Logout Service URL (optional)
   COLstring m_Certificate  = "";  // X509 Certificate for signature verification
   COLstring m_ProviderName = "";  // Display name
};

struct SSOconfig {
   SSOmode         m_Mode     = DISABLED;
   SSOprovider     m_Provider = NONE;
   SSOclientConfig m_ClientConfig;
   SSOsamlConfig   m_SamlConfig;
   int             m_TokenRevalidationInterval = 60;
};

SSOconfig SSOgetConfig();
int SSOgetValidationInterval();

void SSOclearConfig();
void SSOsetConfig(const SSOconfig& Config);

COLstring SSOmodeToString(const SSOmode& Mode);
SSOmode   SSOmodeToEnum(const COLstring& Mode);
COLstring SSOproviderToString(const SSOprovider& Provider);
SSOprovider SSOproviderToEnum(const COLstring& Provider);

#endif  // end of defensive include
