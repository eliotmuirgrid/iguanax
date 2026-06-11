//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOclientVerify.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DRX/DRXutils.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
#include <SSO/SSOclientVerify.h>
#include <SSO/SSOconfig.h>
#include <SSO/SSOdiscovery.h>
#include <SSO/SSOmicrosoft.h>
#include <SSO/SSOsession.h>
#include <SSO/SSOuser.h>
#include <USG/USGusage.h>
#include <USR/USRmap.h>
#include <WEB/WEBcookie.h>
#include <WEB/WEBheartbeat.h>
#include <WEB/WEBsession.h>
COL_LOG_MODULE;

struct SSOinfo {
   COLstring   m_SessionId;
   COLstring   m_Credential;
   COLstring   m_Token;
   SSOprovider m_Provider;
};

static COLstring SSOcreateSessionResponse(const COLstring& SessionId, const COLstring& Username, const bool IsHttps) {
   COL_FUNCTION(SSOcreateSessionResponse);
   COLvar Response;
   Response["success"]         = true;
   Response["data"]["session"] = SessionId;
   Response["data"]["user"]    = Username;
   WEBsessionInfo(&Response["data"], Username);
   const COLstring JsonBody     = Response.json();
   COLstring       HttpResponse = "HTTP/1.1 200 OK\r\n";
   HttpResponse += "Content-Type: application/json\r\n";
   HttpResponse += "Content-Length: " + COLintToString(JsonBody.size()) + "\r\n";
   HttpResponse += "Set-Cookie: " + WEBdefaultCookieName(IsHttps) + "=" + SessionId + "; path=/; HttpOnly; SameSite=Strict";
   if(IsHttps) { HttpResponse += "; Secure"; }
   HttpResponse += "\r\n\r\n";
   HttpResponse += JsonBody;
   return HttpResponse;
}

static void SSOgetSessionAndRespond(const COLstring& Username, const COLstring& DisplayName, const SSOinfo& Info,
                                    const bool IsHttps, const int ConnectionId,
                                    COLmap<COLstring, WEBsession>* pSessionMap) {
   COL_FUNCTION(SSOgetSessionAndRespond);
   COLstring SessionId = Info.m_SessionId;
   if(pSessionMap->count(SessionId) > 0) {
      pSessionMap->removeKey(SessionId);
   } else {
      SDBIlogService(DisplayName + "' has logged in via " + SSOproviderToString(Info.m_Provider) + " SSO.", "#login");
   }
   SessionId = WEBcreateSession(pSessionMap, Username);
   SSOcreateNewSession((*pSessionMap)[SessionId], CLIENT, Info.m_Provider, Username);
   COLsendWebResponse(ConnectionId, SSOcreateSessionResponse(SessionId, Username, IsHttps));
   USGusages().LoginCount++;
}

static void SSOwriteCallback(const bool Success, const COLstring Message, const USRuser& User, const bool IsHttps,
                             const SSOinfo Info, COLmap<COLstring, USRuser>* pUserMap,
                             COLmap<COLstring, WEBsession>* pSessionMap, const COLaddress Address) {
   COL_VAR2(Success, Message);
   if(!Success) { return COLrespondError(Address, Message); }
   (*pUserMap)[User.username()] = User;
   SSOgetSessionAndRespond(User.username(), User.displayName(), Info, IsHttps, Address.ConnectionId, pSessionMap);
}

static void SSOverifyCallback(const bool Success, const COLstring Message, const USRuser& User, const bool IsHttps,
                              const SSOinfo Info, COLmap<COLstring, USRuser>* pUserMap,
                              COLmap<COLstring, WEBsession>* pSessionMap, SCKloop* pLoop, const COLaddress Address) {
   COL_FUNCTION(SSOverifyCallback);
   COL_VAR2(Success, Message);
   if(!Success) {
      COL_TRC("SSO verification failed: " << Message);
      SDBIlogService("Failed to verify token for " + User.username() + ". Ending session.", "#sso");
      return COLrespondError(Address, Message);
   }
   COLauto<COLclosure3<bool, COLstring, const USRuser&>> pCallback = COLnewClosure3(&SSOwriteCallback, IsHttps, Info, pUserMap, pSessionMap, Address);
   COLstring Error;
   if(!SSOsetupUser(User, Info.m_Provider, pUserMap, pCallback, pLoop, Error)) { COLrespondError(Address, Error); }
}

static void SSOverifyImpl(const SSOinfo Info, SCKloop* pLoop,
                          COLauto<COLclosure3<bool, COLstring, const USRuser&>> pCallback) {
   COL_FUNCTION(SSOverifyImpl);
   COLstring Error;
   USRuser   User;

   if(!SSOdiscoveryDocLoaded(Info.m_Provider, Error)) {
      COL_TRC("Failed to load discovery document: " + Error);
      Error = "Failed to initialize authentication services: " + Error;
      return SSOfinishWithError(pLoop, pCallback, Error, User);
   }

   if(Info.m_Provider == MICROSOFT) {
      if(!SSOmicrosoftExtractUserInfo(Info.m_Credential, Info.m_Token, User, Error)) {
         COL_TRC("Failed to extract Microsoft user info: " + Error);
         return SSOfinishWithError(pLoop, pCallback, Error, User);
      }
   } else {
      Error = "Unsupported provider.";
      return SSOfinishWithError(pLoop, pCallback, Error, User);
   }
   pLoop->post(COLnewClosure0(&SSOrunOnMain, pCallback, true, Error, User));
}

static bool SSOvalidateRequest(const COLwebRequest& Request, SSOinfo& Info, COLstring& Error) {
   COL_FUNCTION(SSOvalidateRequest);
   Info.m_SessionId = Request.SessionId;
   Info.m_Provider  = SSOproviderToEnum(COLvarRequiredString(Request.Data, "provider"));
   if(Info.m_Provider == NONE) {
      Error = "Provider not set or not supported";
      return false;
   }
   Info.m_Credential = COLvarRequiredString(Request.Data, "credential");
   Info.m_Token      = COLvarRequiredString(Request.Data, "token");
   return true;
}

// /auth/verify
void SSOclientVerify(const COLwebRequest& Request, COLmap<COLstring, USRuser>* pUserMap,
               COLmap<COLstring, WEBsession>* pSessionMap, SCKloop* pLoop) {
   COL_FUNCTION(SSOclientVerify);
   COL_VAR(Request.Data.json(1));
   COLstring Error;
   SSOinfo   Info;
   if(!DRXsingleSignOnEnabled()) { return COLrespondError(Request.Address, SSO_LICENSE_ERROR); }
   if(!SSOvalidateRequest(Request, Info, Error)) { return COLrespondError(Request.Address, Error); }
   COLauto<COLclosure3<bool, COLstring, const USRuser&>> pCallback =
       COLnewClosure3(&SSOverifyCallback, Request.IsSecure, Info, pUserMap, pSessionMap, pLoop, Request.Address);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&SSOverifyImpl, Info, pLoop, pCallback));
}
