//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: WEBasyncRequest
//
// Author: Vismay Shah
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------
#include <WEB/WEBasyncRequest.h>
#include <WEB/WEBclient.h>
#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>
#include <WEB/WEBurl.h>
#include <WEB/WEButils.h>

#include <SCK/SCKloop.h>
#include <SCKS/SCKSsslConnection.h>

#include <COL/COLclosure.h>
#include <COL/COLstring.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// We have all the data or ran into an error, so we can delete the client and data string from heap and callback
static void WEBreportBack(const WEBresponse& Response, COLclosure1<const WEBresponse&>* pCallback){ 
   COL_FUNCTION(WEBreportBack);
   pCallback->runAndDelete(Response);
}

static void WEBinfiniteLoopWebResponse(WEBresponse* pResponse){
   COL_FUNCTION(WEBinfiniteLoopWebResponse);
   COLstring Error = "Infinite redirect loop detected";
   pResponse->ReturnCode                = 500;
   pResponse->Body                      = Error;
   pResponse->Headers["Content-Type"]   = "text/html";
   pResponse->Headers["Content-Length"] = COLintToString(Error.size());
}

static void WEBpopulateWebResponseHeaders(const COLvar& HeaderVar, WEBresponse* pResponse){
   COL_FUNCTION(WEBpopulateWebResponseHeaders);
   if(!HeaderVar.isMap()) { return; }
   COLmap<COLstring, COLstring> Headers;
   for (auto it = HeaderVar.map().cbegin(); it != HeaderVar.map().cend(); it++){
      Headers[it->first] = it->second;
   }
   pResponse->Headers = Headers;
}

// Get the new url to call from the redirect header and re-submit the request with the new url
static void WEBgotRedirect(const WEBresponse& Result, SCKloop* pLoop, COLclosure1<const WEBresponse&>* pCallback,
                           WEBrequest& Request, COLstring Method){
   COL_FUNCTION(WEBgotRedirect);
   COLstring RedirectUrl;
   WEBheaderGetValue(Result.Headers, "Location", &RedirectUrl);
   COL_TRC("Got HTTP redirect, re-submitting request to: " << RedirectUrl);
   if(RedirectUrl.equals(Request.Url)) {
      // respond with a simple error response
      WEBresponse Response;
      WEBinfiniteLoopWebResponse(&Response);
      WEBreportBack(Response, pCallback);
      return;
   }
   // update the url and send the request to the new location
   Request.Url = RedirectUrl;
   if (Method == "GET"){ 
      WEBrequestAsyncGet(Request, pLoop, pCallback);
   } else { // treating POST as default
      WEBrequestAsyncPost(Request, pLoop, pCallback);
   } 
}

static void WEBhandleRedirect(const WEBresponse& Result, SCKloop* pLoop, COLclosure1<const WEBresponse&>* pCallback, 
                           WEBrequest Request, COLstring Method){
   COL_FUNCTION(WEBhandleRedirect);
   if (WEBisRedirectCode(Result.ReturnCode)) { 
      COL_TRC("We got a redirect");
      WEBgotRedirect(Result, pLoop, pCallback, Request, Method); 
   } else {
      COL_TRC("We have the data from the web request");
      WEBreportBack(Result, pCallback);
   }
}

static void WEBrequestAsyncImpl(const WEBurl& Url, const WEBrequest& Request, const COLstring& RequestData, 
                                 WEBclientConnection* pClientConnection, WEBcallbackFunc pCallback){
   COL_FUNCTION(WEBrequestAsyncImpl);
   if (Url.IsHTTPS){
      pClientConnection->_SslContext.init();
      pClientConnection->_pSocket = new SCKSsslConnection(&pClientConnection->_SslContext, *pClientConnection->_pLoop);
   } else {
      pClientConnection->_pSocket = new SCKconnection(*pClientConnection->_pLoop, -1, 10000);
   }  
   WEBsetCallbacks(pClientConnection->_pSocket, pClientConnection);
   pClientConnection->_pSocket->connect(Url.Host, Url.Port);
   pClientConnection->_pSocket->writeString(RequestData);
}

static bool WEBisBadUrl(WEBurl* pUrl, const WEBrequest& Request, COLclosure1<const WEBresponse&>* pResponseCallback, WEBclientConnection* pClientConnection){
   WEBresponse BadUrlResult;
   if (!WEBtryUrlParse(pUrl, Request.Url, &BadUrlResult)){
      pClientConnection->cancelTimeout();
      pClientConnection->sendResult(BadUrlResult);
      delete pClientConnection;
      delete pResponseCallback;
      return true;
   }
   return false;
}

static void WEBrequestAsyncWrapper(const WEBrequest& Request, SCKloop* pLoop, COLclosure1<const WEBresponse&>* pCallback, const COLstring& Method){
   COL_FUNCTION(WEBrequestAsyncWrapper);
   COL_TRC("Setting up connection and callback");
   int Timeout = Request.Timeout ? Request.Timeout : 5000;
   WEBcallbackFunc pResponseCallback      = COLnewClosure1(&WEBhandleRedirect, pLoop, pCallback, Request, Method); // This gets cleaned up by the destruction of pClientConneciton or by WEBisBadUrl 
   WEBclientConnection* pClientConnection = new WEBclientConnection(pResponseCallback, Timeout, pLoop); // This gets cleaned up by WEBclose or by WEBisBadUrl
   COL_TRC("Checking url");
   WEBurl Url;
   if (WEBisBadUrl(&Url, Request, pResponseCallback, pClientConnection)) { 
      COL_TRC("Failed to parse Url, returning http response with code 418"); 
      return; 
   }
   COL_TRC("Formatting request data");
   COLstring RequestData;
   if (Method == "GET") { WEBformatGetRequest (Request, Url, &RequestData); }
   else                 { WEBformatPostRequest(Request, Url, &RequestData); }
   COL_TRC("Setting up socket and writing data");
   WEBrequestAsyncImpl(Url, Request, RequestData, pClientConnection, pResponseCallback);
}

void WEBrequestAsyncGet (const WEBrequest& Request, SCKloop* pLoop, COLclosure1<const WEBresponse&>* pCallback){
   COL_FUNCTION(WEBrequestAsyncGet);
   WEBrequestAsyncWrapper(Request, pLoop, pCallback, COL_T("GET"));
}

void WEBrequestAsyncPost(const WEBrequest& Request, SCKloop* pLoop, COLclosure1<const WEBresponse&>* pCallback){
   COL_FUNCTION(WEBrequestAsyncPost);
   WEBrequestAsyncWrapper(Request, pLoop, pCallback, COL_T("POST"));
}