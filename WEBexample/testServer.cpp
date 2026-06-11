// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testServer
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir
// Date:   Saturday 20 May 2023 - 09:34AM
// ---------------------------------------------------------------------------
#include "testServer.h"

#include <UNIT/UNITapp.h>

#include <WEB/WEBhandle.h>
#include <WEB/WEBrequestData.h>
#include <WEB/WEBcookie.h>
#include <WEB/WEBresponse.h>
#include <WEB/WEBrequest.h>
#include <WEB/WEBfileHandler.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// Thank you Chat GPT for this.
static const char* TestString =
                     "POST /session/heartbeat?AutomaticRequest=1&%26=+space+ HTTP/1.1\r\n"
                     "Host: 192.168.0.35:7654\r\n"
                     "Connection: keep-alive\r\n"
                     "Content-Length: 2\r\n"
                     "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/113.0.0.0 Safari/537.36\r\n"
                     "Content-Type: text/plain;charset=UTF-8\r\n"
                     "Accept: */*\r\n"
                     "Origin: http://192.168.0.35:7654\r\n"
                     "Referer: http://192.168.0.35:7654/index.html\r\n"
                     "Accept-Encoding: gzip, deflate\r\n"
                     "Accept-Language: en-US,en;q=0.9\r\n"
                     "Cookie: session-Dnw5UmlMWLjYqenBAIgFyi=; IguanaSession=AgvPTt1azc32xPlITkDS4n; f=3333;\r\n"
                     "\r\n";


static void testParseHeader(){
   COL_FUNCTION(testParseHeader);

   WEBrequestData PathStruct;
   WEBrequestParse(TestString, &PathStruct);
   COL_VAR(PathStruct.Headers);
   COL_VAR(PathStruct.Path);
   COL_VAR(PathStruct.Args);
   UNIT_ASSERT_EQUALS("/session/heartbeat",PathStruct.Path);
   UNIT_ASSERT_EQUALS(" space ", PathStruct.Args["&"]);
   UNIT_ASSERT_EQUALS("2", PathStruct.Headers["content-length"]);
}

static void testParseHeaderEncoding(){
   COL_FUNCTION(testParseHeaderEncoding);
   UNIT_ASSERT( WEBhasCompressionFormat("gzip",             "gzip"));
   UNIT_ASSERT( WEBhasCompressionFormat(" gzip",            "gzip"));
   UNIT_ASSERT( WEBhasCompressionFormat(" gzip ",           "gzip"));
   UNIT_ASSERT( WEBhasCompressionFormat(" gzip,",           "gzip"));
   UNIT_ASSERT( WEBhasCompressionFormat(",gzip",            "gzip"));
   UNIT_ASSERT( WEBhasCompressionFormat(",gzip,",           "gzip"));
   UNIT_ASSERT( WEBhasCompressionFormat(", gzip",           "gzip"));
   UNIT_ASSERT( WEBhasCompressionFormat(", gzip ,",         "gzip"));
   UNIT_ASSERT( WEBhasCompressionFormat("br, gzip ,",       "gzip"));
   UNIT_ASSERT( WEBhasCompressionFormat("br , gzip ,",      "gzip"));
   UNIT_ASSERT( WEBhasCompressionFormat(", gzip ,br",       "gzip"));
   UNIT_ASSERT( WEBhasCompressionFormat(", gzip , br",      "gzip"));
   UNIT_ASSERT( WEBhasCompressionFormat("br , gzip , br",   "gzip"));
   UNIT_ASSERT( WEBhasCompressionFormat("gzip",             "Gzip"));
   UNIT_ASSERT( WEBhasCompressionFormat("gzip",             "gZip"));
   UNIT_ASSERT( WEBhasCompressionFormat("gzip",             "GZip"));
   UNIT_ASSERT(!WEBhasCompressionFormat(" gzip.",           "gzip"));
   UNIT_ASSERT(!WEBhasCompressionFormat(".gzip",            "gzip"));
   UNIT_ASSERT(!WEBhasCompressionFormat(",.gzip,",           "gzip"));
   UNIT_ASSERT(!WEBhasCompressionFormat("bgzip",            "gzip"));
   UNIT_ASSERT(!WEBhasCompressionFormat("gzips,",           "gzip"));
   UNIT_ASSERT(!WEBhasCompressionFormat(",gzips",           "gzip"));
   UNIT_ASSERT(!WEBhasCompressionFormat(",gzips ",          "gzip"));
   UNIT_ASSERT(!WEBhasCompressionFormat(",gzips ,",         "gzip"));
   UNIT_ASSERT(!WEBhasCompressionFormat(", bgzip ,",        "gzip"));
   UNIT_ASSERT(!WEBhasCompressionFormat(", bgzips ,",       "gzip"));
   UNIT_ASSERT(!WEBhasCompressionFormat(", bGzips ,",       "Gzip"));
}

static void testParseCookie(){
   COL_FUNCTION(testParseCookie);
   WEBrequestData PathStruct;
   WEBrequestParse(TestString, &PathStruct);
   COLstring Cookie = WEBcookieGet(PathStruct.Headers, "IguanaSession");
   COL_VAR(Cookie);
   UNIT_ASSERT_EQUALS("AgvPTt1azc32xPlITkDS4n", Cookie);
}

static void testParseCookies(){
   COL_FUNCTION(testParseCookies);
   COLmap<COLstring, COLstring> Headers;
   Headers["cookie"] = "IguanaSessionS-Dnw5UmlMWLjYqenBAIgFyi=; IguanaSession=AgvPTt1azc32xPlITkDS4n; IguanaSessionS-zzObBPfuSabcH7tDtOclLr = aepie3iechai4ahgaaCie4 ; ";
   UNIT_ASSERT_EQUALS("AgvPTt1azc32xPlITkDS4n", WEBcookieGet(Headers, "IguanaSession"));
   UNIT_ASSERT_EQUALS("", WEBcookieGet(Headers, "IguanaSessionS-Dnw5UmlMWLjYqenBAIgFyi"));
   UNIT_ASSERT_EQUALS("aepie3iechai4ahgaaCie4", WEBcookieGet(Headers, "IguanaSessionS-zzObBPfuSabcH7tDtOclLr"));
}

static const char* LogHeaders =
                     "POST /log/component HTTP/1.1\r\n"
                     "Host: 192.168.0.35:7654\r\n"
                     "Connection: keep-alive\r\n"
                     "Content-Length: 50\r\n"
                     "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/113.0.0.0 Safari/537.36\r\n"
                     "Content-Type: text/plain;charset=UTF-8\r\n"
                     "Accept: */*\r\n"
                     "Origin: http://192.168.0.35:7654\r\n"
                     "Referer: http://192.168.0.35:7654/index.html\r\n"
                     "Accept-Encoding: gzip, deflate\r\n"
                     "Accept-Language: en-US,en;q=0.9\r\n"
                     "Cookie: session-Dnw5UmlMWLjYqenBAIgFyi=; IguanaSession=AgvPTt1azc32xPlITkDS4n\r\n"
                     "\r\n";

static const char* LogBody = "{\"time\":0,\"component\":\"ProjectDir_qTwmZtOxXSALxn\"}";

static const char* LogResult = "{\n"
                         "    \"data\": {\n"
                         "        \"component\": \"ProjectDir_qTwmZtOxXSALxn\",\n"
                         "        \"time\": 0\n"
                         "    },\n"
                         "    \"location\": \"/log/component\",\n"
                         "    \"who\": {\n"
                         "        \"host\": \"/log/component\",\n"
                         "        \"is_admin\": true,\n"
                         "        \"permissions\": [\n"
                         "            \"admin\"\n"
                         "        ],\n"
                         "        \"user\": \"admin\"\n"
                         "    }\n"
                         "}";

void WEBrequestApiMap(const WEBrequestData& Path, const COLstring& Body, COLvar* pResult){
   COL_FUNCTION(WEBrequestApiMap);
   COLvar& Result = *pResult;
   Result["data"].parse(Body);  // ha why not?  We can parse directly into a subkey.
   Result["location"] = Path.Path;
}

static void testLogRequest(){
   COL_FUNCTION(testLogRequest);
   WEBrequestData PathStruct;
   WEBrequestParse(LogHeaders, &PathStruct);
   COLvar ExpectedResult;
   ExpectedResult.parse(LogResult);

   COLvar Result;
   WEBrequestApiMap(PathStruct, LogBody, &Result);

   COL_VAR(Result.json(1));
   COL_VAR(ExpectedResult.json(1));
}

static void testJsonResponse(){
   COL_FUNCTION(testJsonResponse);
   COLvar Json;
   Json["foo"] = "bar";
   COLstring Result;
   WEBresponseJson(Json, &Result);
   COL_VAR(Result);
}

static void testHttpError(){
   COL_FUNCTION(testHttpError);
   COLstring Result;
   WEBresponseHttpError(401, "Forbidden", &Result);
   COL_VAR(Result);
}

static const char* LongString =
                     "Host: 192.168.0.35:7654\r\n"
                     "Connection: keep-alive\r\n"
                     "Content-Length: 2\r\n"
                     "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/113.0.0.0 Safari/537.36\r\n"
                     "Content-Type: text/plain;charset=UTF-8\r\n"
                     "Accept: */*\r\n"
                     "Origin: http://192.168.0.35:7654\r\n"
                     "Referer: http://192.168.0.35:7654/index.html\r\n"
                     "Accept-Encoding: gzip, deflate\r\n"
                     "Accept-Language: en-US,en;q=0.9\r\n"
                     "Cookie: session-Dnw5UmlMWLjYqenBAIgFyi=; IguanaSession=AgvPTt1azc32xPlITkDS4n; f=3333;\r\n"
                     "\r\nThis is some body.";

static void testParseAllHeaders(){
   COL_FUNCTION(testParseAllHeaders);
   COLmap<COLstring, COLstring> Headers;
   WEBrequestParseHeaders(LongString, LongString + strlen(LongString), &Headers);
}


void testServer(UNITapp* pApp) {
   pApp->add("server/parse/header", &testParseHeader);
   pApp->add("server/parse/header/encoding", &testParseHeaderEncoding);
   pApp->add("server/parse/cookie", &testParseCookie);
   pApp->add("server/parse/cookies", &testParseCookies);
   pApp->add("server/test/dispatch", &testLogRequest);
   pApp->add("server/response/json", &testJsonResponse);
   pApp->add("server/response/error", &testHttpError);
   //pApp->add("server/test/headers", &testParseAllHeaders);
}
