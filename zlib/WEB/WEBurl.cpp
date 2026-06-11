// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBurl
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 28 March 2023 - 12:53PM
// ---------------------------------------------------------------------------

#include <WEB/WEBurl.h>
#include <WEB/WEBresponse.h>

#include <stdlib.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool WEBurlParse(const COLstring& UrlIn, WEBurl* pResult){
   COL_FUNCTION(WEBurlParse);
   COL_VAR(UrlIn);
   COLstring Url = UrlIn.strip(COLstring::Both);
   COL_VAR(Url);
   if (Url.size() < 5){
      pResult->Host = "URL is too short";
      return false;
   }
   if (Url.substr(0,4) != "http"){
      pResult->Host = "URL is should start with http";
      return false;
   }
   pResult->IsHTTPS = Url.substr(0,5) == "https";
   
   COL_VAR(pResult->IsHTTPS);
   Url = Url.substr(pResult->IsHTTPS ? 8 : 7);
   COL_VAR(Url);
   COLstring Host;
   Url.split(Host, pResult->Path, "/");
   COL_VAR2(Host, pResult->Path);
   pResult->Path = "/" + pResult->Path;
   COLstring PortString;
   Host.split(pResult->Host, PortString, ":");
   COL_VAR2(pResult->Host, PortString);
   if (PortString.size() > 0){
      COL_TRC("Found port");
      pResult->Port = ::atoi(PortString.c_str());
   } else {
      pResult->Port = pResult->IsHTTPS ? 443 : 80;
   }
   COL_TRC(pResult->Port);
   return true;
}

bool WEBtryUrlParse(WEBurl* pUrl, const COLstring& RequestUrl, WEBresponse* pOut){
   COL_FUNCTION(WEBtryUrlParse);
   if (!WEBurlParse(RequestUrl, pUrl)){
      pOut->ReturnCode = 418; // 418 is I am a teapot.
      pOut->Body = "Cannot parse URL; " + pUrl->Host;
      return false;
   }
   return true;
}