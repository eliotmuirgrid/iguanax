//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOauthSet.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-09-24, 1:32p.m.
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
#include <SSO/SSOauthSet.h>
#include <SSO/SSOconfig.h>
#include <SSO/SSOserialize.h>

#include <COL/COLthreadPool.h>
#include <memory>
COL_LOG_MODULE;

static void SSOsetRunCallbackOnMain(const bool Success, const COLstring Error, COLauto<COLclosure2<bool, COLstring>> pCallback) {
   COL_FUNCTION(SSOsetRunCallbackOnMain);
   pCallback->run(Success, Error);
}

static void SSOsetCallback(const bool Success, const COLstring Error, std::shared_ptr<SSOconfig> pConfig,
                           const COLaddress Address) {
   COL_FUNCTION(SSOsetCallback);
   COL_VAR2(Success, Error);
   if(!Success) { return COLrespondError(Address, Error); }
   pConfig->m_Mode == DISABLED ? SSOclearConfig() : SSOsetConfig(*pConfig);
   COLrespondSuccess(Address);
}

static void SSOsetWorker(std::shared_ptr<SSOconfig> pConfig, COLauto<COLclosure2<bool, COLstring>> pCallback, SCKloop* pLoop) {
   COL_FUNCTION(SSOsetWorker);
   COLstring Error;
   COLvar ConfigVar;
   SSOconfigToVar(*pConfig, ConfigVar);
   if(!FILsave(DIRfileConfigSso(), ConfigVar.json(1), &Error)) {
      return pLoop->post(COLnewClosure0(&SSOsetRunCallbackOnMain, false, Error, pCallback));
   }
   if(pConfig->m_Mode == SAML) {
      if(!SSOload(*pConfig, Error)) { // have to call load so we are able to parse metadata
         return pLoop->post(COLnewClosure0(&SSOsetRunCallbackOnMain, false, Error, pCallback));
      }
   }
   pLoop->post(COLnewClosure0(&SSOsetRunCallbackOnMain, true, Error, pCallback));
}

// /auth/set
void SSOauthSet(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(SSOauthSet);
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", EDIT_AUTHENTICATION, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(EDIT_AUTHENTICATION));
   }
   auto   pConfig = std::make_shared<SSOconfig>();
   COLvar ConfigVar;
   SSOconfigFromVar(Request.Data, *pConfig);
   COLauto<COLclosure2<bool, COLstring>> pCallback = COLnewClosure2(&SSOsetCallback, pConfig, Request.Address);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&SSOsetWorker, pConfig, pCallback, pLoop));
}