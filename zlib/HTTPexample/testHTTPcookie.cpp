//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testHTTPcookie.cpp
//
// Description:
//
// This is a test of HTTPcookie
//
// Author: Shuo Hou
// Date:   Tue 15 Dec 2020 11:32:00 EST
//---------------------------------------------------------------------------

#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>
#include <TST/TSThelpers.h>

#include <HTTP/HTTPcookie.h>
#include <HTTP/HTTPheader.h>
#include <HTTP/HTTPrequest.h>
#include <HTTP/HTTPresponse.h>

#include <FIL/FILutils.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLhashmap.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>

static void testCookieFieldNameLowerCase(){
   HTTPrequest Request;
   Request.header().addHeader("cookie", HTTPmakeCookieString("Monster", "Love's Cookies", NULL, "/blah", "guardian.com", false));
   COLhashmap<COLstring,COLstring> CookieJar;
   HTTPextractCookies(Request,CookieJar);

   COLhashmapPlace Cookies = CookieJar.find("Monster");
   if (!Cookies){
       TST_ASSERT_MESSAGE(false,"Could not find Cookies when Cookie field name is lowercased");
   }
   COLstring CookieValue = CookieJar[Cookies];
   TST_ASSERT(CookieValue == "Love's Cookies");


   HTTPresponse Response;
   Response.header().addHeader("set-cookie", HTTPmakeCookieString("Monster", "Love's Cookies", NULL, "/blah", "guardian.com", false));
   COLhashmap<COLstring,COLstring> ResponseCookieJar;
   HTTPextractCookies(Response,ResponseCookieJar);

   COLhashmapPlace ResponseCookies = ResponseCookieJar.find("Monster");
   if (!ResponseCookies){
       TST_ASSERT_MESSAGE(false,"Could not find Cookies when Cookie field name is lowercased");
   }
   COLstring ResponseCookieValue = ResponseCookieJar[ResponseCookies];
   TST_ASSERT(ResponseCookieValue == "Love's Cookies");
}

static void testCookieFieldNameUpperCase(){
   HTTPrequest Request;
   Request.header().addHeader("Cookie", HTTPmakeCookieString("Monster", "Love's Cookies", NULL, "/blah", "guardian.com", false));
   COLhashmap<COLstring,COLstring> CookieJar;
   HTTPextractCookies(Request,CookieJar);

   COLhashmapPlace Cookies = CookieJar.find("Monster");
   if (!Cookies){
       TST_ASSERT_MESSAGE(false,"Could not find Cookies when Cookie field name is uppercased");
   }
   COLstring CookieValue = CookieJar[Cookies];
   TST_ASSERT(CookieValue == "Love's Cookies");

   HTTPresponse Response;
   Response.header().addHeader("Set-Cookie", HTTPmakeCookieString("Monster", "Love's Cookies", NULL, "/blah", "guardian.com", false));
   COLhashmap<COLstring,COLstring> ResponseCookieJar;
   HTTPextractCookies(Response,ResponseCookieJar);

   COLhashmapPlace ResponseCookies = ResponseCookieJar.find("Monster");
   if (!ResponseCookies){
       TST_ASSERT_MESSAGE(false,"Could not find Cookies when Cookie field name is uppercased");
   }
   COLstring ResponseCookieValue = ResponseCookieJar[ResponseCookies];
   TST_ASSERT(ResponseCookieValue == "Love's Cookies");
   
}

TSTtestable* testHTTPcookie(){
   TST_START_COLLECTION("HTTPcookie")
      TST_ADD_TEST_FUNCTION(testCookieFieldNameLowerCase);
      TST_ADD_TEST_FUNCTION(testCookieFieldNameUpperCase);
   TST_END_COLLECTION
}