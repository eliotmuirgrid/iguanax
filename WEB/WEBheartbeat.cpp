//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBheartbeat
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thu  1 Sep 2022 12:00:18 EDT
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <SDB/SDBcapi.h>
#include <WEB/WEBcookie.h>
#include <WEB/WEBrequestData.h>
#include <WEB/WEBresponse.h>
#include <WEB/WEBsessionHandler.h>
COL_LOG_MODULE;

static bool WEBssoSessionExpired(const WEBsession& Session) {
   COL_FUNCTION(WEBssoSessionExpired);
   if(Session.m_SsoProvider == "None") {
      COL_TRC("Not an SSO session");
      return false;
   }
   const int    Interval = 3600; // 1hr
   const time_t Diff     = time(NULL) - Session.m_LastValidation;
   COL_TRC("Interval: " << Interval << ", timeDiff: " << Diff << ", lastValidation: " << Session.m_LastValidation);
   return Diff > Interval;
}

static void WEBsessionAddSsoData(const WEBsession& Session, COLvar& Response) {
   COL_FUNCTION(WEBsessionAddSsoData);
   Response["sso_mode"]          = Session.m_SsoMode;
   Response["sso_provider"]      = Session.m_SsoProvider;
   Response["should_revalidate"] = WEBssoSessionExpired(Session);
}

void WEBsessionInfo(COLvar* pOut, const COLstring& Username){
   COL_FUNCTION(WEBsessionInfo);
   COLvar& Response = *pOut;
   bool IsAdmin = WEBpermissions(Username, &Response["permissions"]);
   Response["username"] = Username;
   Response["is_admin"] = IsAdmin;
   Response["valid"] = true;
   COL_VAR(Response.json(1));
}

void WEBsessionTouch(WEBsession* pSession){
   COL_FUNCTION(WEBsessionTouch);
   pSession->m_LastActivity = time(NULL);
   COL_VAR2(pSession->m_SessionId, pSession->m_LastActivity);
}
// Why do the special logic with cookies and headers?  It gives a convenient way of authenticating APIs in browsers.
// /session/heartbeat
void WEBheartbeat(const WEBrequestData& Data, COLmap<COLstring, WEBsession>* pSessionMap) {
   COL_FUNCTION(WEBheartbeat);
   COL_VAR(Data);
   const COLstring SessionId = WEBcookieGet(Data.Headers, WEBdefaultCookieName(Data.IsSecure));
   COL_VAR(SessionId);
   if (SessionId.is_null() || pSessionMap->count(SessionId) == 0){
      COL_TRC("Empty session ID " << Data.Headers);
      COL_VAR(Data);
      WEBresponseError(Data.ConnectionId, "No valid login session.");
      return;
   }
   WEBsession& Session = (*pSessionMap)[SessionId];
   COL_VAR(Session);

   WEBsessionTouch(&Session);
   COLvar Response;
   Response["success"] = true;
   WEBsessionInfo(&Response["data"], Session.m_Username);
   WEBsessionAddSsoData(Session, Response["data"]);
   COL_VAR(Response.json(1));
   COLstring Result;
   WEBresponseJson(Response, &Result);
   WEBresponseSend(Data.ConnectionId, Result);
}
