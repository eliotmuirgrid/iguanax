//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBlogin
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thu  1 Sep 2022 12:00:18 EDT
//---------------------------------------------------------------------------
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
#include <USG/USGusage.h>
#include <USR/USRuser.h>
#include <WEB/WEBcookie.h>
#include <WEB/WEBheartbeat.h>
#include <WEB/WEBlogin.h>
#include <WEB/WEBrequestData.h>
#include <WEB/WEBresponse.h>
#include <WEB/WEBsessionHandler.h>
COL_LOG_MODULE;

void WEBresponseWithIguanaSessionCookie(const COLvar& Data, const COLstring& SessionId, bool Secure,COLstring* pResult){
   COL_FUNCTION(WEBresponseWithIguanaSessionCookie);
   COLstring Body = Data.json();
   WEBresponseBodyHeaders(Body.size(), pResult);
   (*pResult) += "Set-Cookie: " + WEBdefaultCookieName(Secure) +  "=" + SessionId + "; path=/; HttpOnly; SameSite=Strict";
   if (Secure) { (*pResult) += "; Secure"; }    // Extra nonsense for HTTPS.
   if (SessionId.is_null()) { (*pResult) += "; Expires=Thu, 01 Jan 1970 00:00:00 GMT"; }     // remove cookie from browser
   (*pResult) += "\r\n\r\n";
   (*pResult) += Body;
}

static void WEBcreateSessionAndRespond(COLmap<COLstring, WEBsession>* pSessionMap, COLaddress Address, bool IsHttps, COLstring Username) {
   COLstring SessionId = WEBcreateSession(pSessionMap, Username);
   COLvar Response;
   Response["success"] = true;
   Response["data"]["session"] = SessionId;
   WEBsessionInfo(&Response["data"], Username);
   COL_VAR(Response.json(1));
   if (Address.pInternalResponseCallback) {
      COL_TRC("This is an internal request from SCKcall().");
      Address.pInternalResponseCallback->runAndDelete(Response);
      return;
   }
   COLstring Results;
   WEBresponseWithIguanaSessionCookie(Response, SessionId, IsHttps, &Results);
   SDBIlogService("User '" + Username + "' has logged in.", "#login");
   COL_VAR(Results);
   COLsendWebResponse(Address.ConnectionId, Results);
   USGusages().LoginCount++;
}

static bool WEBaddLdapUserToMap(const COLvar& Roles, const COLstring& Username, const COLstring& Password, COLmap<COLstring, USRuser>* pUserMap, COLstring* pError){
   COL_FUNCTION(WEBaddLdapUserToMap);
   if (Roles.isNull() || Roles.type() != COLvar::Array) {
      *pError = "Failed to determine roles for the external user.";
      return false;
   }
   if (pUserMap->count(Username)) {
      if ((*pUserMap)[Username].isExternalUser()){
         COL_TRC("An externally authenticated user is signing in again - remove the old entry and re-add them"); // this is to update the password
         pUserMap->removeKey(Username);
      } else {
         *pError = "A user with this username already exists. Must use a unique username.";
         return false;
      }
   }
   COLarray<COLstring> Tags;
   for (int i = 0; i < Roles.size(); ++i) {
      Tags.push_back(Roles[i]);
   }
   COL_VAR(Roles);
   pUserMap->add(Username, USRuser(Username, Password, Tags, true));
   return true;
}

static void WEBdoRegularLogin(COLmap<COLstring, WEBsession>* pSessionMap, const COLstring& Username, const COLstring& Password, COLaddress Address, bool IsHttps){
   COL_FUNCTION(WEBdoRegularLogin);
   if (!WEBauthenticate(Username, Password)) {
      return COLrespondError(Address, "Invalid username or password.");
   }
   WEBcreateSessionAndRespond(pSessionMap, Address, IsHttps, Username);
}

static void WEBloginHandleExtAuthResult(COLvar Result, COLmap<COLstring, WEBsession>* pSessionMap, COLmap<COLstring, USRuser>* pUserMap, COLaddress Address, bool IsHttps, COLstring Username, COLstring Password) {
   COL_FUNCTION(WEBloginHandleExtAuthResult);
   COL_VAR(Result);
   COLASSERT(Result.exists("authenticated"));   // make sure DBDextAuth always returns a properly formatted Result
   if (Result["authenticated"].asBool()) {
      COLstring Error;
      if (!WEBaddLdapUserToMap(Result["roles"], Username, Password, pUserMap, &Error)) {
         COL_VAR(Error);
         SDBIlogService("Failed to authorize the external user '" + Username + "':\n" + Error, "#login");
      } else {
         COL_TRC("Successful external authentication");
         return WEBcreateSessionAndRespond(pSessionMap, Address, IsHttps, Username);
      }
   }
   COL_TRC("External auth failed. Try local");
   WEBdoRegularLogin(pSessionMap, Username, Password, Address, IsHttps);
}

// /session/login
void WEBlogin(const COLwebRequest& Request, COLmap<COLstring, WEBsession>* pSessionMap, COLmap<COLstring, USRuser>* pUserMap, COLclosure3<COLstring, COLstring, COLclosure1<COLvar>*>* pExternalAuthenticator) {
   COL_FUNCTION(WEBlogin);
   COL_VAR(Request);
   const COLstring& Username = COLvarRequiredString(Request.Data, "username");
   const COLstring& Password = COLvarRequiredString(Request.Data, "password");
   COL_VAR2(Username, Password);
   if ( Username.is_null() || Password.is_null() ) {
      return COLrespondError(Request.Address, "Please enter your username and password.");
   }
   COLclosure1<COLvar>* pCallback = COLnewClosure1(&WEBloginHandleExtAuthResult, pSessionMap, pUserMap, Request.Address, Request.IsSecure, Username, Password);
   pExternalAuthenticator->run(Username, Password, pCallback);
}