//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testParse
//
// Description: Implementation
//
// Author: Vismay Shah
// Date:   Tue 29 Aug 2023 1:56pm
//---------------------------------------------------------------------------

#include "testParse.h"

#include <UNIT/UNITapp.h>

#include <SCK/SCKloop.h>

#include <WEB/WEBhandle.h>
#include <WEB/WEBrequest.h>
#include <WEB/WEBrequestData.h>
#include <WEB/WEBresponse.h>

#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE; 

static void testBuffer(){
   COL_FUNCTION(testBuffer);
   COLstring MyBuffer = "123helloworld123";
   COLstring Search   = "foo";

   MyBuffer.get_buffer()[MyBuffer.size()] = 'a';
   COL_TRC(strlen(MyBuffer.c_str()));

   const char* pFound = WEBsafeStrStr(MyBuffer.get_buffer(), Search.c_str(), MyBuffer.size());
   COL_VAR(pFound[1]);
}

static void testBadHeader(){
   COL_FUNCTION(testBadHeader);
   // Say we somehow don't have \r\n
   COLstring Headers = "Host: www.example.com\r\n"
                       "User-Agent: MyHttpClient/1.0\r\n"
                       "Accept: text/html,application/xhtml+xml";
   COLmap<COLstring, COLstring> ParsedHeaders;
   const char* pStart = Headers.c_str();
   const char* pEnd   = Headers.c_str() + Headers.size();

   bool Success = WEBrequestParseHeaders(pStart, pEnd, &ParsedHeaders);
   UNIT_ASSERT_EQUALS(Success, false);
}

static void testNoColon(){
   COL_FUNCTION(testNoColon);
   // Say we somehow don't have :
   COLstring Headers = "Host www.example.com\r\n"
                       "User-Agent MyHttpClient/1.0\r\n"
                       "Accept text/html,application/xhtml+xml";
   COLmap<COLstring, COLstring> ParsedHeaders;
   const char* pStart = Headers.c_str();
   const char* pEnd   = Headers.c_str() + Headers.size();

   bool Success = WEBrequestParseHeaders(pStart, pEnd, &ParsedHeaders);
   UNIT_ASSERT_EQUALS(Success, false);
}

static void testHttp2(){
   COL_FUNCTION(testHttp2);
   // Say we somehow don't have \r\n
   COLstring Headers = "PRI * HTTP/2.0\r\n\r\n"
                       "SM\r\n\r\n"
                       "\000\000\024\004\000\000\000\000\000\000\002\000\000\000\000\000\004\000\000Bh\000\006\000\004\000\000\000\003\000\000\000\n";
   COLmap<COLstring, COLstring> ParsedHeaders;
   const char* pStart = Headers.c_str();
   const char* pEnd   = Headers.c_str() + Headers.size();

   WEBrequestParseHeaders(pStart, pEnd, &ParsedHeaders);
}

static void testRequestNoNullTerminator(){
   COLstring Headers = "Host: www.example.com\r\n"
                       "User-Agent: MyHttpClient/1.0\r\n"
                       "Accept: text/html,application/xhtml+xml\r\n\r\n";
   COLmap<COLstring, COLstring> ParsedHeaders;
   Headers.get_buffer()[Headers.size()] = 'a';
   const char* pStart = Headers.get_buffer();
   const char* pEnd   = Headers.c_str() + Headers.size();

   WEBrequestParseHeaders(pStart, pEnd, &ParsedHeaders);
}

static void testParseByteSequenceGood(){
   COL_FUNCTION(testParseByteSequenceGood);
   COLstring ValidRequest   = "GET record?progress=100%25&done=true HTTP/1.1\r\n"
                              "Host: www.example.com\r\n"
                              "User-Agent: MyHttpClient/1.0\r\n\r\n";
   WEBrequestData Data;
   COL_TRC("Trying valid request");
   WEBrequestParse(ValidRequest, &Data);
   UNIT_ASSERT_EQUALS(2, Data.Args.size());
   UNIT_ASSERT(Data.Args.count("progress"));
   UNIT_ASSERT_EQUALS("100%", Data.Args["progress"]);
   UNIT_ASSERT(Data.Args.count("done"));
   UNIT_ASSERT_EQUALS("true", Data.Args["done"]);
}

static void testParseByteSequencePercent(){
   COL_FUNCTION(testParseByteSequencePercent);
   // Note the 100% not correctly encoded in the following request
   COLstring UnencodedPercent = "GET /record?progress=100%&done=true HTTP/1.1\r\n"
                               "Host: www.example.com\r\n"
                               "User-Agent: MyHttpClient/1.0\r\n\r\n";
   WEBrequestData Data;
   COL_TRC("Trying request with unencoded %");
   WEBrequestParse(UnencodedPercent, &Data);
   UNIT_ASSERT_EQUALS(2, Data.Args.size());
   UNIT_ASSERT(Data.Args.count("progress"));
   UNIT_ASSERT_EQUALS("100%", Data.Args["progress"]);
   UNIT_ASSERT(Data.Args.count("done"));
   UNIT_ASSERT_EQUALS("true", Data.Args["done"]);
}

static void testParseByteSequenceBad(){
   COL_FUNCTION(testParseByteSequenceBad);
   COLstring BadValue = "GET /record?progress=100%!3asd&done=true HTTP/1.1\r\n"
                               "Host: www.example.com\r\n"
                               "User-Agent: MyHttpClient/1.0\r\n\r\n";
   COLstring BadKey = "GET /record?prog%!$ress=100%25&done=true HTTP/1.1\r\n"
                               "Host: www.example.com\r\n"
                               "User-Agent: MyHttpClient/1.0\r\n\r\n";
   COLstring BadKeyAndValue = "GET /record?prog%!$ress=100%!3asd&done=true HTTP/1.1\r\n"
                               "Host: www.example.com\r\n"
                               "User-Agent: MyHttpClient/1.0\r\n\r\n";
   WEBrequestData Data;
   COL_TRC("Trying request with bad \%sequence in value");
   WEBrequestParse(BadValue, &Data);
   COL_VAR(Data);
   UNIT_ASSERT_EQUALS(2, Data.Args.size());
   UNIT_ASSERT(Data.Args.count("progress"));
   UNIT_ASSERT_EQUALS("100%!3asd", Data.Args["progress"]);
   UNIT_ASSERT(Data.Args.count("done"));
   UNIT_ASSERT_EQUALS("true", Data.Args["done"]);

   COL_TRC("Trying request with bad \%sequence in key");
   WEBrequestParse(BadKey, &Data);
   COL_VAR(Data);
   UNIT_ASSERT_EQUALS(2, Data.Args.size());
   UNIT_ASSERT(Data.Args.count("prog%!$ress"));
   UNIT_ASSERT_EQUALS("100%", Data.Args["prog%!$ress"]);
   UNIT_ASSERT(Data.Args.count("done"));
   UNIT_ASSERT_EQUALS("true", Data.Args["done"]);

   COL_TRC("Trying request with bad \%sequence in key and value");
   WEBrequestParse(BadKeyAndValue, &Data);
   COL_VAR(Data);
   UNIT_ASSERT_EQUALS(2, Data.Args.size());
   UNIT_ASSERT(Data.Args.count("prog%!$ress"));
   UNIT_ASSERT_EQUALS("100%!3asd", Data.Args["prog%!$ress"]);
   UNIT_ASSERT(Data.Args.count("done"));
   UNIT_ASSERT_EQUALS("true", Data.Args["done"]);
}

void testParse(UNITapp* pApp){
   pApp->add("buffer",     &testBuffer);
   pApp->add("badNull",    &testRequestNoNullTerminator);
   // pApp->add("http2",      &testHttp2);
   // The next two are effectively the same problem, manifesting differently
   pApp->add("badHeader",  &testBadHeader);
   pApp->add("noColon",    &testNoColon);
   pApp->add("byteSequence/good",    &testParseByteSequenceGood);
   pApp->add("byteSequence/percent", &testParseByteSequencePercent);
   pApp->add("byteSequence/bad",     &testParseByteSequenceBad);
}
