//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOsession.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <SSO/SSOconfig.h>
#include <SSO/SSOsession.h>
#include <WEB/WEBsession.h>

#include <ctime>
COL_LOG_MODULE;

void SSOcreateNewSession(WEBsession& Session, const SSOmode Mode, const SSOprovider& Provider, const COLstring& Username) {
   COL_FUNCTION(SSOcreateNewSession);
   Session.m_SsoMode        = SSOmodeToString(Mode);
   Session.m_SsoProvider    = SSOproviderToString(Provider);
   Session.m_LastValidation = time(NULL);
   Session.m_Username       = Username;
}