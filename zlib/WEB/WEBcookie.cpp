// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBcookie
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Saturday 20 May 2023 - 02:13PM
// ---------------------------------------------------------------------------

#include <WEB/WEBcookie.h>

#include <COL/COLarray.h>
#include <COL/COLsplit.h>
#include <COL/COLmap.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLstring g_HttpCookieName  = "IguanaSession";
static COLstring g_HttpsCookieName = "IguanaSessionS";

void WEBsetCookieHost(const COLstring& HostId) {
   if (HostId.is_null()) { return; }
   g_HttpCookieName  = "IguanaSession-"  + HostId;
   g_HttpsCookieName = "IguanaSessionS-" + HostId;
}

COLstring WEBdefaultCookieName(bool Https) {
   return Https ? g_HttpsCookieName : g_HttpCookieName;
}

static void WEBparseCookies(const COLstring& Cookies, COLmap<COLstring, COLstring>* pCookieJar) {
   COL_FUNCTION(WEBparseCookies);
   COL_VAR(Cookies);
   COLarray<COLstring> CookieList;
   COLstring CookieName, CookieValue;
   COLsplit(&CookieList, Cookies, ";");
   for (int i = 0; i < CookieList.size(); ++i) {
      if (CookieList[i].split(CookieName, CookieValue, "=")) {
         (*pCookieJar)[CookieName.strip(COLstring::Both)] = CookieValue.strip(COLstring::Both);
      } else {
         COL_ERR("Failed to parse cookie '" << CookieList[i] << "'");
      }
   }
}

COLstring WEBcookieGet(const COLmap<COLstring, COLstring>& Headers, const COLstring& CookieName){
   COL_FUNCTION(WEBcookieGet);
   COL_VAR(CookieName);
   auto Cookies = Headers.find("cookie");    // normalized to lower case.
   if (Headers.end() == Cookies) {
      COL_TRC("No cookies present.");
      return "";
   }
   COLmap<COLstring, COLstring> CookieJar;
   WEBparseCookies(Cookies.value(), &CookieJar);
   auto Cookie = CookieJar.find(CookieName);
   if (CookieJar.end() == Cookie) {
      COL_TRC("Cookie not found.");
      return "";
   }
   COL_TRC(Cookie.key() << " = " << Cookie.value());
   return Cookie.value();
}
