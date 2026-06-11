#ifndef _SSODISCOVERY_H
#define _SSODISCOVERY_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOdiscovery.h
//
// Description:
// Provides a lightweight cache of the selected SSO provider and the time it was
// last resolved, and a function to load/verify the provider's discovery document.
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <SSO/SSOconfig.h>
#include <ctime>

extern const COLstring SSO_MICROSOFT_URL;

struct SSOdiscoveryCache {
   time_t      m_CacheTime = 0;
   SSOprovider m_Provider  = NONE;
   COLvar      m_DiscoveryDoc;
};

COLvar SSOgetDiscoveryDoc();
bool SSOdocLoaded(const SSOprovider& Provider);
bool SSOdiscoveryDocLoaded(SSOprovider Provider, COLstring& Error);

#endif  // end of defensive include
