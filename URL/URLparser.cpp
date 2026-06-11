//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: URLparser
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, May 5th, 2009 @ 09:44:28 AM
//---------------------------------------------------------------------------
#include <URL/URLparser.h>

#include <stdlib.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

URLtype URLurlType(const COLstring& Url){
   COL_FUNCTION(URLurlType);
   COL_VAR(Url);
   if (Url.is_null()){
      return URL_UNKNOWN;
   }
   COLstring UrlType, Path;
   if (!Url.split(UrlType, Path, "://")){
      return URL_UNKNOWN;
   }
   COL_VAR2(UrlType, Path);
   if ("file" == UrlType) { return URL_FILE; }
   if ("ftp"  == UrlType) { return URL_FTP; }
   if ("http" == UrlType) { return URL_HTTP; }
   if ("https"== UrlType) { return URL_HTTP; }
   
   return URL_UNKNOWN;
}

bool URLparseHttpUrl(const COLstring& UrlIn, bool* pIsHTTPS, COLstring* pHost, COLstring* pPath, int* pPort){
   COL_FUNCTION(URLparseHttpUrl);
   COL_VAR(UrlIn);
   COLstring Url = UrlIn.strip(COLstring::Both);
   COL_VAR(Url);
   if (Url.size() < 5){
      *pHost = "URL is too short";
      return false;
   }
   if (Url.substr(0,4) != "http"){
      *pHost = "URL is should start with http";
      return false;
   }
   *pIsHTTPS = Url.substr(0,5) == "https";
   *pPort = *pIsHTTPS ? 443 : 80;

   COL_VAR(*pIsHTTPS);
   Url = Url.substr(*pIsHTTPS ? 8 : 7);
   COL_VAR(Url);
   COLstring Host;
   Url.split(Host, *pPath, "/");
   COL_VAR2(Host, *pPath);
   *pPath = "/" + *pPath;
   COLstring PortString;
   Host.split(*pHost, PortString, ":");
   COL_VAR2(*pHost, PortString);
   if (PortString.size() > 0){
      COL_TRC("Found port");
      *pPort = ::atoi(PortString.c_str());
   } else {
      *pPort = *pIsHTTPS ? 443 : 80;
   }
   COL_TRC(*pPort);
   return true;
}


bool URLparseFileUrl(const COLstring& Url, COLstring* pPath){
   COL_FUNCTION(URLparseFileUrl);
   COLstring UrlType;
   if (!Url.split(UrlType, *pPath, "://")){
      return false;
   }
   COL_VAR2(Url, *pPath);  
   return true;
}
