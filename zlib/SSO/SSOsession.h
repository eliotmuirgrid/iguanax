#ifndef _SSOSESSION_H
#define _SSOSESSION_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOsession.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLstring.h>
#include <SSO/SSOconfig.h>

class WEBsession;

void SSOcreateNewSession(WEBsession& Session, SSOmode Mode, const SSOprovider& Provider, const COLstring& Username);

#endif  // end of defensive include