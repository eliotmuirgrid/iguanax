//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBsessionHandler
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Thu 21 May 2020 10:25:45 EDT
//---------------------------------------------------------------------------

#include <WEB/WEBsessionHandler.h>
#include <WEB/WEBheartbeat.h>
#include <WEB/WEBrequestData.h>
#include <WEB/WEBcookie.h>

#include <COL/COLauto.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool WEBsessionIsSso(const COLstring& Location) {
   COL_FUNCTION(WEBsessionIsSso);
   return Location == "/auth/get" || Location == "/auth/verify" || Location == "/auth/saml/initiate" ||
          Location == "/saml/verify";
}

bool WEBsessionNologin(const COLstring& Location) {
   COL_FUNCTION(WEBsessionNologin);
   return               Location == "/mode"              // Mode of server which determines the web APIs.
                     || Location == "/file_browser/sep"  // This gives up the directory separator for the operating system of the server
                     || Location == "/session/login"     // Login
                     || Location == "/session/logout"    // Logout
                     || Location == "/user/admin/unsafe" // Admin password is password
                     || Location == "/iguana/version"
                     || Location == "/header"
                     || Location == "/os"
                     || Location == "/hash"
                     || Location == "/health"
                     || Location == "/peers/server_guid"
                     || WEBsessionIsSso(Location);
}

COLauto< COLclosure3<COLstring, COLstring, bool*> > s_pAuthenticateCallback;

void WEBauthenticateSet(COLclosure3<COLstring, COLstring, bool*>* pCallback){
   COL_FUNCTION(WEBauthenticateSet);
   s_pAuthenticateCallback = pCallback;
}

bool WEBauthenticate(const COLstring& UserName, const COLstring& Password){
   COL_FUNCTION(WEBauthenticate);
   bool Result = false;
   if (s_pAuthenticateCallback.get()){
      s_pAuthenticateCallback->run(UserName, Password, &Result);
   }
   COL_VAR(Result);
   return Result;
}

COLauto< COLclosure3<COLstring, COLvar*, bool*> > s_pPermissionCallback;

void WEBpermissionsSet(COLclosure3<COLstring, COLvar*, bool*>* pCallback){
   COL_FUNCTION(WEBpermissionsSet);
   s_pPermissionCallback = pCallback;
}

// TODO gross interface
bool WEBpermissions(const COLstring& Username, COLvar* pInfo){
   COL_FUNCTION(WEBpermissions);
   bool Result = false;
   if (s_pPermissionCallback.get()){
      s_pPermissionCallback->run(Username, pInfo, &Result);
   }
   COL_VAR2(Result, *pInfo);
   return Result;
}

bool WEBwho(const COLmap<COLstring, WEBsession>& SessionMap, const WEBrequestData& Data, COLstring* pUser, bool* pAdminUser, COLvar* pPermissions){
   COL_FUNCTION(WEBwho);
   COLstring SessionId = WEBcookieGet(Data.Headers, WEBdefaultCookieName(Data.IsSecure));
   COL_VAR(SessionId);
   if (SessionMap.count(SessionId) == 0){
      COL_TRC("This session is not valid.");
      return false;
   }
   COLstring UserName = SessionMap[SessionId].m_Username;
   COL_VAR(UserName);
   if (UserName.is_null()) {
      return false;
   }
   // TODO fix gross squishly COLvar interface
   *pAdminUser = WEBpermissions(UserName, pPermissions);
   *pUser= UserName;
   return true;
}

bool WEBsessionAuthenticate(const WEBrequestData& Data, const COLmap<COLstring, WEBsession>& SessionMap, COLstring* pUser, bool* pAdminUser){
   COL_FUNCTION(WEBsessionAuthenticate);
   // TODO: pass permission along with User and AdminUser
   COLvar Permissions;
   bool Authenticated = WEBwho(SessionMap, Data, pUser, pAdminUser, &Permissions);
   COL_VAR(Authenticated);
   if (!Authenticated){
      COL_TRC("Failed authentication and endpoint requires authentication.");
      return false;
   }
   COL_TRC("Passed authentication, or requested endpoint does not require authentication.");
   return true;
}