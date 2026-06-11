#ifndef _SSOSAMLUTILS_H
#define _SSOSAMLUTILS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOsamlUtils.h
//
//  Description: Shared SAML utility functions
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLstring.h>
#include <SSO/SSOconfig.h>
#include <time.h>

// SAML request generation
struct SSOsamlRequestParams {
   COLstring RequestId;
   COLstring Timestamp;
   COLstring Destination;
   COLstring Binding;
   COLstring AcsUrl;
   COLstring EntityId;
   bool      ForceAuthn;
   bool      IsPassive;
};

COLstring SSOsamlFormatIso8601(time_t Time);

void SSOsamlDeflate(const COLstring& Input, COLstring* pOutput);

// HTML utilities
COLstring SSOsamlHtmlEscape(const COLstring& Input);
COLstring SSOsamlHtmlErrorPageWithScript(const COLstring& SafeError);
COLstring SSOsamlHtmlSuccessPageWithRedirect(const COLstring& RedirectPath);

COLstring SSOsamlBuildAuthnRequest(const SSOsamlRequestParams& Params);
bool      SSOsamlBuildRedirectUrl(SSOprovider Provider, const SSOsamlConfig& Config, const COLstring& SamlRequest,
                                  const COLstring& RelayState, COLstring& RedirectUrl, COLstring& Error);

#endif  // end of defensive include
