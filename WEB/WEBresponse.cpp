// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBresponse
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Sunday 21 May 2023 - 10:32AM
// ---------------------------------------------------------------------------

#include <WEB/WEBresponse.h>
#include <WEB/WEBmainThread.h>
#include <WEB/WEBserver.h>
#include <WEB/WEButils.h>

#include <SCK/SCKloop.h>

#include <GZIP/GZIPutils.h>

#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool WEBheaderGetValue(const COLmap<COLstring, COLstring>& Headers, const COLstring& Key, COLstring* pValue){
   COL_FUNCTION(WEBheaderGetValue);
   COL_TRC("Looking for " << Key);
   if (Headers.find(Key)) {
      COL_VAR(Headers[Key]);
      *pValue = Headers[Key];
      return true;   
   } 
   COLstring LowerCaseHeader = Key;
   LowerCaseHeader.toLowerCase();
   COL_TRC("Now trying " << LowerCaseHeader);
   if (Headers.find(LowerCaseHeader)) {
      COL_VAR(Headers[LowerCaseHeader]);
      *pValue = Headers[LowerCaseHeader];
      return true;
   }
   COLstring UpperCaseHeader = Key;
   UpperCaseHeader.toUpperCase();
   COL_TRC("Now trying " << UpperCaseHeader);
   if (Headers.find(UpperCaseHeader)) {
      COL_VAR(Headers[UpperCaseHeader]);
      *pValue = Headers[UpperCaseHeader];
      return true;
   }
   COL_TRC("Could not find header " << Key << " in camel case, lower case, or uppercase");
   return false;
}

COLostream& operator<<(COLostream& Stream, const WEBresponse& Response){
   Stream << "WEBresponse: code = " << Response.ReturnCode << newline
          << "Headers count = " << Response.Headers.size() << newline
          << "Body.size = " << Response.Body.size() << newline
          << Response.Body;
   return Stream;
}

void WEBheadersSetContentLength(int Size, COLstring* pOut){
   COL_FUNCTION(WEBheadersSetContentLength);
   *pOut += COL_T("Content-Length: ") + COLintToString(Size) + COL_T("\r\n");
}

void WEBheaderSetContentType(const COLstring& MimeType, COLstring* pOut){
   COL_FUNCTION(WEBheaderSetContentType);
   *pOut += COL_T("Content-Type: ") + MimeType + COL_T("; charset=utf-8\r\n");
}

// TODO - Need to spend some time to understand these options better - IX-3616

COLstring WEBcontentSecurityPolicy() {
   COL_FUNCTION(WEBcontentSecurityPolicy);
   COLstring Result = "script-src 'self' https://accounts.google.com https://apis.google.com "
                      "https://login.microsoftonline.com https://alcdn.msauth.net https://aadcdn.msauth.net; "
                      "connect-src 'self' https://accounts.google.com https://www.googleapis.com "
                      "https://login.microsoftonline.com https://graph.microsoft.com https://browser.pipe.aria.microsoft.com; "
                      "frame-src 'self' https://accounts.google.com https://login.microsoftonline.com; "
                      "img-src 'self' data:; "
                      "frame-ancestors 'self'; "
                      "form-action 'self';";
   return Result;
}

COLstring WEBsecurityHeaders() {
   COL_FUNCTION(WEBsecurityHeaders);
   COLstring Result = "";
   Result += COL_T("X-Content-Type-Options: nosniff\r\n");
   Result += COL_T("Referrer-Policy: strict-origin-when-cross-origin\r\n");
   Result += COL_T("X-Frame-Options: DENY\r\n");
   Result += COL_T("Content-Security-Policy: ") + WEBcontentSecurityPolicy() + COL_T("\r\n");
   return Result;
}

void WEBresponseBodyHeaders(int Size, COLstring* pResult){
   COL_FUNCTION(WEBresponseBodyHeaders);
   COLstring& Result = *pResult;
   // max-age expiry gives the number of seconds to cache this non dynamic content.
   // so 36000 is 10 hours. 
   Result = COL_T("HTTP/1.1 200 OK\r\n"); //Cache-Control: max-age=40000\r\n");
   Result += WEBsecurityHeaders();
   WEBheadersSetContentLength(Size, &Result);
}

void WEBresponseFile(const COLstring& MimeType, const COLstring& FileBody, COLstring* pResult){
   COL_FUNCTION(WEBresponseFile);
   WEBresponseBodyHeaders(FileBody.size(), pResult);
   WEBheaderSetContentType(MimeType, pResult);
   *pResult += COL_T("\r\n") + FileBody;
}

void WEBresponseFileGzip(const COLstring& MimeType, const COLstring& UncompressedFileBody, COLstring* pResult){
   COL_FUNCTION(WEBrequestDataGzip);
   COLstring FileBody;
   GZIPcompress(UncompressedFileBody, &FileBody);
   WEBresponseBodyHeaders(FileBody.size(), pResult);
   WEBheaderSetContentType(MimeType, pResult);
   *pResult += COL_T("Content-Encoding: gzip\r\n\r\n") + FileBody;
}

void WEBresponseJson(const COLvar& Data, COLstring* pResult){
   COL_FUNCTION(WEBresponseJson);
   COLstring& Result = *pResult;
   COLstring Body = Data.json();
   WEBresponseBodyHeaders(Body.size(), pResult);
   WEBheaderSetContentType("application/json", pResult);
   Result += "\r\n";
   Result += Body;
   COL_VAR(Result);
}

void WEBresponseErrorGen(const COLstring& Error, COLstring* pResult){
   COL_FUNCTION(WEBresponseErrorGen);
   COLvar R;
   R["success"] = false;
   R["error"] = Error;
   WEBresponseJson(R, pResult);
}

void WEBresponseError(int ConnectionId, const COLstring& Error){
   COL_FUNCTION(WEBresponseError);
   COLstring Http;
   WEBresponseErrorGen(Error, &Http);
   WEBresponseSend(ConnectionId, Http);
}

void WEBresponseHttpError(int BadErrorCode, const COLstring& ErrorMessage, COLstring* pResult){
   COL_FUNCTION(WEBresponseHttpError);
   *pResult = COL_T("HTTP/1.1 ") + COLintToString(BadErrorCode) + " " + WEBlookupReason(BadErrorCode) + COL_T("\r\n");
   WEBheadersSetContentLength(ErrorMessage.size(), pResult);
   *pResult += COL_T("\r\n") + ErrorMessage;
   COL_VAR(*pResult);
}

// TODO this stuff is a separate concern - should shift to it's own CPP.
WEBserver* s_pServer = NULL;

static void WEBresponseSend(COLuint64 ConnectionId, const char* pData, int Size){ // This will send the data to the registered web server.
   COL_FUNCTION(WEBresponseSend);
   COL_VAR(ConnectionId);
   COL_DUMP("Response:", pData, Size);
   if (s_pServer){
      s_pServer->write(ConnectionId, pData, Size);
   } else {
      COL_ERR("There is no webserver set.");
   }
}

void WEBrequestAsyncPostResponse(COLuint64 ConnectionId, COLstring Data){
   COL_FUNCTION(WEBrequestAsyncPostResponse);
   WEBresponseSend(ConnectionId, Data.c_str(), Data.size());
}

void WEBresponseSetServer(WEBserver* pServer){
   COL_FUNCTION(WEBresponseSetServer);
   s_pServer = pServer;
}

void WEBresponseSend(COLuint64 ConnectionId, const COLstring& Data){
   COL_FUNCTION(WEBresponseSend);
   if (WEBisMainThread(COLthreadGetCurrentThreadID())) {
      COL_TRC("On main thread already, can just respond");
      WEBresponseSend(ConnectionId, Data.c_str(), Data.size());
   } else {
      COL_TRC("On worker thread will have to post back to main first");
      if (s_pServer){
         s_pServer->m_pLoop->post(COLnewClosure0(WEBrequestAsyncPostResponse, ConnectionId, Data));
      }
   }
}

static COLstring WEBresponseSerializeHeaders(WEBresponse* pResponse) {
   COL_FUNCTION(WEBresponseSerializeHeaders);
   if (!pResponse->Headers.count("Content-Length")){
      COL_TRC("Content-Length header not found, adding it");
      pResponse->Headers["Content-Length"] = COLintToString(pResponse->Body.size());
   }
   COLstring Headers;
   COLostream Out(Headers);
   auto it = pResponse->Headers.cbegin();
   for (it; it!= pResponse->Headers.cend(); it++){
      Out << it->first << ": " << it->second << "\r\n";
   }
   Out << "\r\n"; //end of headers
   return Headers;
}

COLstring WEBresponseSerialize(WEBresponse* pResponse){
   COL_FUNCTION(WEBresponseSerialize);
   COLstring Out;
   Out = COL_T("HTTP/1.1 ") + COLintToString(pResponse->ReturnCode) + " " + WEBlookupReason(pResponse->ReturnCode) + COL_T("\r\n"); 
   Out += WEBresponseSerializeHeaders(pResponse);
   Out += pResponse->Body;
   COL_VAR(Out);
   return Out;
}