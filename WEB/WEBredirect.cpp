// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBredirect
//
// Description:
//
// Redirect HTTP client to the HTTPS site when HTTPS is enabled
//
// Author: John Q
// Date:   Thu 31 Aug 2023 13:36:22 PDT
// ---------------------------------------------------------------------------

#include <WEB/WEBredirect.h>
#include <WEB/WEBresponse.h>

#include <SCK/SCKconnection.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool WEBisHttpRequest(const COLstring& Buffer) {
   COL_FUNCTION(WEBisHttpRequest);
   // basically we are searching to the HTTP request line
   // our web server doesn't support HTTP/2.0 so limiting to 1.0 and 1.1 for now
   return npos != Buffer.find(" HTTP/1.");
}

static const COLstring& WEBredirectHtml(const COLstring& RedirectAddress) {
   COL_FUNCTION(WEBredirectHtml);
   static COLstring Html(R"(
<html>
   <head>
      <title>Non-HTTPS request to an HTTPS-enabled Iguana instance</title>
   </head>
   <body>
      <h1>Non-HTTPS request to an HTTPS-enabled Iguana instance</h1>
      <p>
         This Iguana instance is running with HTTPS encryption.<br><br>
         A plain HTTP (non-HTTPS) request was made.
         Try changing the request to use a 'https://' prefix instead, or
         <a id='thelink' href=")" + RedirectAddress + R"(">continue to</a>
      </p>
   </body>
</html>
)");
   return Html;
}

static bool WEBextractPath(const COLstring& Buffer, COLstring* pPath) {
   COL_FUNCTION(WEBextractPath);
   COLstring Method, Rest, Path;
   if (!Buffer.split(Method, Rest, " ")) { return false; }
   if (!Rest.split(Path, Rest, " ")) { return false; }
   if (npos != Path.find(" ")) { return false; }
   COL_VAR3(Method, Path, Rest);
   Path.swap(pPath);
   return true;
}

static bool WEBextractHost(const COLstring& Buffer, COLstring* pHost) {
   COL_FUNCTION(WEBextractHost);
   size_t Start = Buffer.find("Host:");
   if (npos == Start) {
      Start = Buffer.find("host:");
      if (npos == Start) { return false; }
   }
   COLstring Host;
   const char* pChar = Buffer.data() + Start + 5;
   const char* pEnd  = Buffer.data() + Buffer.size();
   while (pChar != pEnd && '\r' != *pChar) {
      Host.append(pChar, 1);
      pChar++;
   }
   Host = Host.strip(COLstring::Both);
   if (Host.is_null()) { return false; }
   if (npos != Host.find(" ")) { return false; }
   COL_VAR(Host);
   Host.swap(pHost);
   return true;
}

static bool WEBextractUrl(const COLstring& Buffer, COLstring* pUrl) {
   COL_FUNCTION(WEBextractUrl);
   COLstring Host, Path;
   if (!WEBextractHost(Buffer, &Host)) { return false; }
   if (!WEBextractPath(Buffer, &Path)) { return false; }
   if (0 == Host.toLowerCase().find("https://")) {
      Host = Host.substr(8);
   }
   if (0 == Host.toLowerCase().find("http://")) {
      Host = Host.substr(7);
   }
   COLstring Url = Host + Path;
   COL_VAR(Url);
   Url.swap(pUrl);
   return true;
}

bool WEBautoRedirect(const COLstring& Buffer, bool ToHTTPS, SCKconnection* pConnection) {
   COL_FUNCTION(WEBautoRedirect);
   COLstring Url;
   if (!WEBextractUrl(Buffer, &Url)) { return false; }
   Url.prepend(ToHTTPS ? "https://" : "http://");
   COL_VAR(Url);
   COLstring Response = COL_T("HTTP/1.1 301 Moved Permanently\r\n");
   Response += COL_T("Location: ") + Url + "\r\n\r\n";
   COL_VAR(Response);
   pConnection->rawWrite(Response.data(), Response.size());
   return true;
}

// Forcing a redirect to / since they will have to log in to get a valid session
static COLstring WEBhttpsRedirectAddress(const COLstring& Buffer){
   COL_FUNCTION(WEBhttpsRedirectAddress);
   COLstring Url;
   if (!WEBextractHost(Buffer, &Url)) { return ""; }
   Url.prepend("https://");
   Url.append("/");
   COL_VAR(Url);
   return Url; 
} 

void WEBserveRedirectHtml(SCKconnection* pConnection, const COLstring& Data) {
   COL_FUNCTION(WEBserveRedirectHtml);
   COLstring RedirectAdress = WEBhttpsRedirectAddress(Data);
   COLstring Response;
   WEBresponseFile("text/html", WEBredirectHtml(RedirectAdress), &Response);
   pConnection->rawWrite(Response.data(), Response.size());
}