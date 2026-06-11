//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOauthGet.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-09-24, 1:42p.m.
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <DRX/DRXutils.h>
#include <SSO/SSOauthGet.h>
#include <SSO/SSOconfig.h>
#include <SSO/SSOserialize.h>
COL_LOG_MODULE;

// /auth/get
void SSOauthGet(const COLwebRequest& Request) {
   COL_FUNCTION(SSOauthGet);
   COLvar Result;
   SSOconfigToVar(SSOgetConfig(), Result);
   Result["license"] = DRXsingleSignOnEnabled();
   COL_VAR(Result.json(1));
   COLrespondSuccess(Request.Address, Result);
}