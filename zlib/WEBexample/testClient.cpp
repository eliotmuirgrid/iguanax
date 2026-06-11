//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testClient
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wed 03/15/2023 
//---------------------------------------------------------------------------

#include "testClient.h"

#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>
#include <WEB/WEBasyncRequest.h>

#include <UNIT/UNITapp.h>

#include <SCK/SCKloop.h>

#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void ChunkedResult(const WEBresponse& Response, SCKloop* pLoop){
   COL_FUNCTION(ChunkedResult);
   COL_VAR3(Response.ReturnCode, Response.Body, Response.Headers.size());
   pLoop->shutdown();
   return;
}

static void testChunked(){
   COL_FUNCTION(testChunked);
   WEBrequest Request;
   Request.Timeout = 5000;
   Request.Url = "http://anglesharp.azurewebsites.net/Chunked"; // good for tests, responds two chunks 1s apart
   Request.Headers["User-Agent"] = "ifware/7.0";
   Request.Headers["Accept"] =  "*/*";
   COL_TRC("Request set up");
   WEBresponse Response;
   WEBrequestBlockingGet(Request, &Response);
   COL_VAR(Response);
}

static void testChunkedAsync(){
   COL_FUNCTION(testChunkedAsync);
   WEBrequest Request;
   Request.Timeout = 5000;
   Request.Url = "http://anglesharp.azurewebsites.net/Chunked";
   Request.Headers["User-Agent"] = "ifware/7.0";
   Request.Headers["Accept"] =  "*/*";
   COL_TRC("Request set up");
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   WEBrequestAsyncGet(Request, &Loop, COLnewClosure1(&ChunkedResult, &Loop));
   Loop.start();
}

static void testFormat(){
   COL_FUNCTION(testFormat);
   WEBrequest Request;
   Request.Timeout = 1000;
   Request.Url = "https://api.bitbucket.org/2.0/user";
   Request.UserName = "eliotmuir";
   Request.Password = "ATBBLZRPk3wV8BHNK7gYh96uYdvG8EADD214";
   Request.Headers["Accept"] = "application/json";

   WEBresponse Response;
   WEBrequestBlockingGet(Request, &Response);
   COL_VAR3(Response.ReturnCode, Response.Body, Response.Headers.size());
}

// The tests below have a common WEBrequest pattern, so just call this helper instead of repeating it in every test
static WEBrequest makeWebRequest(int Timeout, const COLstring& Url){
   WEBrequest Request;
   Request.Timeout = Timeout;
   Request.Url = Url;
   Request.Headers["Accept"] =  "*/*";
   return Request;
}

static void testRedirect(){
   COL_FUNCTION(testRedirect);
   WEBrequest Request = makeWebRequest(2000, "https://wikipedia.com");
   WEBresponse Response;
   WEBrequestBlockingGet(Request, &Response);
   COL_VAR3(Response.ReturnCode, Response.Body, Response.Headers.size());
}

static void AsyncWikiResponse(const WEBresponse& Response, SCKloop* pLoop){
   COL_FUNCTION(AsyncWikiResponse);
   COL_VAR3(Response.ReturnCode, Response.Body, Response.Headers.size());
   pLoop->shutdown();
   return;
}

static void testAsyncRedirect(){
   COL_FUNCTION(testAsyncRedirect);
   WEBrequest Request = makeWebRequest(2000, "https://wikipedia.com");
   COL_TRC("Request set up");
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   WEBrequestAsyncGet(Request, &Loop, COLnewClosure1(&AsyncWikiResponse, &Loop));
   Loop.start();
}

static void testAwsBasic(){
   COL_FUNCTION(testAwsBasic);
   WEBrequest Request = makeWebRequest(2000, "http://54.204.16.110:8181/basic");
   COL_TRC("Request set up");
   WEBresponse Response;
   WEBrequestBlockingGet(Request, &Response);
   UNIT_ASSERT_EQUALS("GET hello from AWS", Response.Body);
   WEBresponse PostResponse;
   WEBrequestBlockingPost(Request, &PostResponse);
   UNIT_ASSERT_EQUALS("POST hello from AWS", PostResponse.Body);
}

static void testAwsRedirect(){
   COL_FUNCTION(testAwsRedirect);
   WEBrequest Request = makeWebRequest(2000, "http://54.204.16.110:8181/redirect");
   WEBresponse GetResponse;
   WEBrequestBlockingGet(Request, &GetResponse);
   UNIT_ASSERT_EQUALS("GET redirect success", GetResponse.Body);
   WEBresponse PostResponse;
   WEBrequestBlockingPost(Request, &PostResponse);
   UNIT_ASSERT_EQUALS("POST redirect success", PostResponse.Body);
   WEBresponse DeleteResponse;
   WEBrequestBlockingDelete(Request, &DeleteResponse);
   UNIT_ASSERT_EQUALS("DELETE redirect success", DeleteResponse.Body);
}

static void testAwsInfiniteRedirect(){
   COL_FUNCTION(testAwsInfiniteRedirect);
   WEBrequest Request = makeWebRequest(2000, "http://54.204.16.110:8181/inf_redirect");
   WEBresponse Response;
   WEBrequestBlockingGet(Request, &Response);
   UNIT_ASSERT_EQUALS("Infinite redirect loop detected", Response.Body);
   WEBrequestBlockingPost(Request, &Response);
   UNIT_ASSERT_EQUALS("Infinite redirect loop detected", Response.Body);
   WEBrequestBlockingDelete(Request, &Response);
   UNIT_ASSERT_EQUALS("Infinite redirect loop detected", Response.Body);
   COL_VAR3(Response.ReturnCode, Response.Body, Response.Headers.size());
}

static void AwsAsyncBasicResponse(const WEBresponse& Response, SCKloop* pLoop, int* pCount){
   COL_FUNCTION(AwsAsyncBasicResponse);
   int& Count = *pCount;
   COL_VAR3(Response.ReturnCode, Response.Body, Response.Headers.size());
   if (Response.Body == "POST hello from AWS" || Response.Body == "GET hello from AWS"){
      Count--;
      COL_VAR(Count);
   }
   if (Count == 0){
      COL_TRC("Received all expected responses - shutting down loop.");
      pLoop->shutdown(); 
   }
   return;
}

static void testAwsAsyncBasic(){
   COL_FUNCTION(testAwsAsyncBasic);
   WEBrequest Request = makeWebRequest(2000, "http://54.204.16.110:8181/basic");
   COL_TRC("Request set up");
   COLthreadPool Pool;
   int Count = 2; // We make 2 requests, store this and decrement in callback so we know when to shutdown the loop
   Pool.start();
   SCKloop Loop(&Pool);
   WEBrequestAsyncGet (Request, &Loop, COLnewClosure1(&AwsAsyncBasicResponse, &Loop, &Count));
   WEBrequestAsyncPost(Request, &Loop, COLnewClosure1(&AwsAsyncBasicResponse, &Loop, &Count));
   Loop.start();
   UNIT_ASSERT_EQUALS(0, Count);
}

static void AwsAsyncRedirectResponse(const WEBresponse& Response, SCKloop* pLoop, int* pCount){
   COL_FUNCTION(AwsAsyncRedirectResponse);
   int& Count = *pCount;
   COL_VAR3(Response.ReturnCode, Response.Body, Response.Headers.size());
   if (Response.Body == "POST redirect success" || Response.Body == "GET redirect success"){
      Count--;
      COL_VAR(Count);
   }
   if (Count == 0){
      COL_TRC("Received all expected responses - shutting down loop.");
      pLoop->shutdown(); 
   }
   return;
}

static void testAwsAsyncRedirect(){
   COL_FUNCTION(testAwsAsyncRedirect);
   WEBrequest Request = makeWebRequest(2000, "http://54.204.16.110:8181/redirect");
   COL_TRC("Request set up");
   COLthreadPool Pool;
   int Count = 2; // We make 2 requests, store this and decrement in callback so we know when to shutdown the loop
   Pool.start();
   SCKloop Loop(&Pool);
   WEBrequestAsyncGet (Request, &Loop, COLnewClosure1(&AwsAsyncRedirectResponse, &Loop, &Count));
   WEBrequestAsyncPost(Request, &Loop, COLnewClosure1(&AwsAsyncRedirectResponse, &Loop, &Count));
   Loop.start();
   UNIT_ASSERT_EQUALS(0, Count);
}

void testClient(UNITapp* pApp){
   pApp->add("simple/format",             &testFormat);
   pApp->add("client/sync/chunk",         &testChunked); 
   pApp->add("client/sync/redirect",      &testRedirect);
   pApp->add("client/async/chunk",        &testChunkedAsync);
   pApp->add("client/async/redirect",     &testAsyncRedirect);
   // These tests rely on the Unit Test Sever component on the AWS windows build machine
   pApp->add("client/sync/aws/basic",     &testAwsBasic);
   pApp->add("client/sync/aws/redirect",  &testAwsRedirect);
   pApp->add("client/sync/aws/inf",       &testAwsInfiniteRedirect);
   pApp->add("client/async/aws/basic",    &testAwsAsyncBasic);
   pApp->add("client/async/aws/redirect", &testAwsAsyncRedirect);
}
