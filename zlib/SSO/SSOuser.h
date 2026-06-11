#ifndef _SSOUSER_H
#define _SSOUSER_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOuser.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLweb.h>
#include <SSO/SSOconfig.h>

class SCKloop;
class USRuser;
class WEBsession;

void SSOrunOnMain(COLauto<COLclosure3<bool, COLstring, const USRuser&>> pCallback, bool Success, COLstring Message,
                  USRuser User);
void SSOfinishWithError(SCKloop* pLoop, COLauto<COLclosure3<bool, COLstring, const USRuser&>> pCallback,
                        const COLstring& Error, const USRuser& User);
bool SSOsetupUser(const USRuser& User, SSOprovider Provider, COLmap<COLstring, USRuser>* pUserMap,
                  COLauto<COLclosure3<bool, COLstring, const USRuser&>> pCallback, SCKloop* pLoop, COLstring& Error);


#endif  // end of defensive include
