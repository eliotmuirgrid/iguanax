//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOapi.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-09-22, 10:44a.m.
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <SSO/SSOapi.h>
#include <SSO/SSOauthGet.h>
#include <SSO/SSOauthSet.h>
#include <SSO/SSOclientVerify.h>
#include <SSO/SSOsamlInit.h>
#include <USR/USRuser.h>
#include <WEB/WEBsession.h>
COL_LOG_MODULE;

#include <SSO/SSOsamlRevalidate.h>

void SSOapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap,
            COLmap<COLstring, USRuser>* pUserMap, COLmap<COLstring, WEBsession>* pSessionMap, SCKloop* pLoop) {
   COL_FUNCTION(SSOapi);
   ApiMap.add("/auth/verify",           COLnewClosure1(&SSOclientVerify, pUserMap, pSessionMap, pLoop));
   ApiMap.add("/auth/get",              COLnewClosure1(&SSOauthGet));
   ApiMap.add("/auth/set",              COLnewClosure1(&SSOauthSet, pLoop));
   ApiMap.add("/auth/saml/initiate",    COLnewClosure1(&SSOsamlInit, pLoop));
   ApiMap.add("/auth/saml/revalidate",  COLnewClosure1(&SSOsamlRevalidate, pSessionMap, pLoop));
}
