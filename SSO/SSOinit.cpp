//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOinit.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <SDB/SDBcapi.h>
#include <SSO/SSOconfig.h>
#include <SSO/SSOinit.h>
#include <SSO/SSOmicrosoft.h>
#include <SSO/SSOserialize.h>
COL_LOG_MODULE;

void SSOinit() {
   COL_FUNCTION(SSOinit);
   SSOconfig Config;
   COLstring Err;
   if(SSOload(Config, Err)) {
      Config.m_Mode == DISABLED ? SSOclearConfig() : SSOsetConfig(Config);
   } else {
      COL_TRC("Error loading sso config. " << Err);
      SDBIlogService("Failed to load SSO provider settings. " + Err);
   }
}
