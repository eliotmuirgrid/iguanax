//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testHTTPutils
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, December 12th, 2006 @ 02:40:10 PM
//
//---------------------------------------------------------------------------
#include "HTTPexamplePrecomp.h"
#pragma hdrstop

#include "testHTTPutils.h"

#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <HTTP/HTTPheader.h>
#include <HTTP/HTTPcookie.h>
#include <HTTP/HTTPresponse.h>
#include <HTTP/HTTPresponseParser.h>
#include <HTTP/HTTPrequest.h>
#include <HTTP/HTTPrequestParser.h>
#include <HTTP/HTTPsession.h>
#include <HTTP/HTTPsessionHandler.h>
#include <HTTP/HTTPutils.h>

void HTTPtestParseRequest();
void HTTPtestHeaderParser();   
void HTTPtestBodyParser();
void HTTPtestHeaderParserErrorHandling();

// TODO add tests for HTTPmimeLookup

void testHTTPutils::testHTTPsession()
{
   HTTPsessionHandler SessionHandler;
   HTTPsession& Session1 = SessionHandler.createSession();
   HTTPsession& Session2 = SessionHandler.createSession();

   COL_TRC("Session 1 Name = " << Session1.name());
   COL_TRC("Session 2 Name = " << Session2.name());

   TST_ASSERT(Session1.name() != "");
   TST_ASSERT(Session2.name() != "");
   TST_ASSERT(Session2.name() != Session1.name());

   Session1.setVariable("Fred", "Mack");
   TST_ASSERT(Session1.variable("Fred") != NULL);
   TST_ASSERT(*Session1.variable("Fred") == "Mack");
   TST_ASSERT(Session1.variable("NoExist") == NULL);

   HTTPrequest Request;
   COLstring SessionCookie = "SessionId=" + Session1.name();
   Request.header().addHeader("Cookie", 6, SessionCookie.c_str(), SessionCookie.size());
   TST_ASSERT(HTTPsessionHandlerFindSession(SessionHandler, Request) != NULL);

   TST_ASSERT_EQUALS(false, Session1.isTransactionValid("Test"));
   TST_ASSERT_EQUALS(false, Session1.closeTransaction("Test"));
   COLstring TransactionId = Session1.createTransaction();
   TST_ASSERT(TransactionId != "");
   TST_ASSERT_EQUALS(true, Session1.isTransactionValid(TransactionId));
   TST_ASSERT_EQUALS(true, Session1.closeTransaction(TransactionId));
   TST_ASSERT_EQUALS(false, Session1.closeTransaction(TransactionId));
   TST_ASSERT_EQUALS(false, Session1.isTransactionValid(TransactionId));
   TST_ASSERT(TransactionId != Session1.createTransaction());

   HTTPsessionTouch(Session1,(unsigned)-1);

   COLstring ExpiredSessionId = Session1.name();
      
   SessionHandler.removeExpiredSessions();   

   TST_ASSERT(SessionHandler.session(ExpiredSessionId) == NULL);
   TST_ASSERT(SessionHandler.session(Session2.name()) == &Session2);
}

void testHTTPutils::testCookies()
{
   COLdateTime DateTime(2007, 12, 1, 12, 0, 0);
   COLstring CookieString = HTTPmakeCookieString("Monster", "Love's Cookies", &DateTime, "/monster/rock", "www.interfaceware.com", false);

   TST_ASSERT(CookieString == "Monster=Love's Cookies; expires=Sat, 01-Dec-2007 12:00:00; path=/monster/rock; domain=www.interfaceware.com; HttpOnly");  

   CookieString = HTTPmakeCookieString("Monster", "Love's Cookies", &DateTime, "/monster/rock", "", false);

   TST_ASSERT(CookieString == "Monster=Love's Cookies; expires=Sat, 01-Dec-2007 12:00:00; path=/monster/rock; HttpOnly");  

   CookieString = HTTPmakeCookieString("Monster", "Love's Cookies", &DateTime, "", "", true);

   TST_ASSERT(CookieString == "Monster=Love's Cookies; expires=Sat, 01-Dec-2007 12:00:00; path=/; HttpOnly; Secure");  

   CookieString = HTTPmakeCookieString("Monster", "Love's Cookies", NULL, "", "", true);

   TST_ASSERT(CookieString == "Monster=Love's Cookies; path=/; HttpOnly; Secure");  

   CookieString = HTTPmakeCookieString("Monster", "Love's Cookies", NULL, "/blah", "guardian.com", false);

   TST_ASSERT(CookieString == "Monster=Love's Cookies; path=/blah; domain=guardian.com; HttpOnly");  

   HTTPresponse Response;
   HTTPsetCookie(Response, "Monster", "Love's Cookies", NULL, "/blah", "guardian.com", false);   
   COLuint32 HeaderIndex = Response.header().findHeader("Set-Cookie");
   CookieString = Response.header().headerValue(HeaderIndex);
   TST_ASSERT(CookieString == "Monster=Love's Cookies; path=/blah; domain=guardian.com; HttpOnly");  
}

void testHTTPutils::testRequestParsing()
{
   HTTPtestParseRequest();
}

void testHTTPutils::testHeaderParsing()
{
   HTTPtestHeaderParser();
}

void testHTTPutils::testHeaderParsingErrorHandling()
{
   HTTPtestHeaderParserErrorHandling();
}

void testHTTPutils::testBodyParsing()
{
   HTTPtestBodyParser();
}

void testHTTPutils::testResponseObject()
{
   HTTPresponse Response;
   HTTPheader& ResponseHeader = Response.header();
   Response.setCode(HTTPresponse::HTTP_STATUS_BAD_GATEWAY);
   Response.setEntityType("text/html");
   Response.setNoCaching();
   ResponseHeader.addHeader("Now", "Then");
   HTTPheaderSetContentLength(ResponseHeader,122);
   
   COLstring Flatwire;
   COLostream Stream(Flatwire);
   HTTPgenerateResponse(Stream, Response);
   COL_VAR(Flatwire);

   HTTPresponse Response2;
   HTTPheader& ResponseHeader2 = Response2.header();
   HTTPresponseParser Parser(Response2);
   HTTPparseResult Result = Parser.parse(Flatwire.c_str(), Flatwire.size());
   TST_ASSERT(HTTP_PARSE_COMPLETE == Result);
   TST_ASSERT(Response2.code() == HTTPresponse::HTTP_STATUS_BAD_GATEWAY);
   TST_ASSERT(HTTPheaderGetContentLength(ResponseHeader2) == 122);
   TST_ASSERT(ResponseHeader2.headerValue(ResponseHeader2.findHeader("Now")) == "Then");
   TST_ASSERT(ResponseHeader2.headerValue(ResponseHeader2.findHeader("Content-Type")).find("text/html") != npos);
   TST_ASSERT(ResponseHeader2.headerValue(ResponseHeader2.findHeader("Cache-Control")) == "max-age=0");

   COLstring Flatwire2;
   COLostream Stream2(Flatwire2);
   HTTPgenerateResponse(Stream2, Response2);
   COL_VAR(Flatwire2);
   TST_ASSERT(Flatwire == Flatwire2);

   HTTPresponse Response3;
   HTTPheader& ResponseHeader3 = Response3.header();
   Parser.reset(Response3);
   Result = Parser.parse(Flatwire2.c_str(), Flatwire2.size());
   TST_ASSERT(HTTP_PARSE_COMPLETE == Result);
   TST_ASSERT(Response3.code() == HTTPresponse::HTTP_STATUS_BAD_GATEWAY);
   TST_ASSERT(HTTPheaderGetContentLength(ResponseHeader3) == 122);
   TST_ASSERT(ResponseHeader3.headerValue(ResponseHeader2.findHeader("Now")) == "Then");
   TST_ASSERT(ResponseHeader3.headerValue(ResponseHeader2.findHeader("Content-Type")).find("text/html") != npos);
   TST_ASSERT(ResponseHeader3.headerValue(ResponseHeader2.findHeader("Cache-Control")) == "max-age=0");

   COLstring Flatwire3;
   COLostream Stream3(Flatwire3);
   HTTPgenerateResponse(Stream3, Response3);
   COL_VAR(Flatwire3);
   TST_ASSERT(Flatwire2 == Flatwire3);
}

void testHTTPutils::testRequestObject()
{
   HTTPrequest Request;
   Request.setMethod("GET");
   Request.setLocation("/blah.html");
   Request.header().addHeader("Dog", "Ball");
   Request.setQueryString("life=fun&dog=ball");
   HTTPheaderSetContentLength(Request.header(),1234);
   COLstring Flatwire;
   COLostream Stream(Flatwire);

   HTTPgenerateRequest(Stream, Request);

   COL_VAR(Flatwire);

   HTTPrequest Request2;
   HTTPrequestParser Parser(Request2);

   HTTPparseResult Result = Parser.parse(Flatwire.c_str(), Flatwire.size());

   TST_ASSERT(Result == HTTP_PARSE_COMPLETE);

   TST_ASSERT(HTTPheaderGetContentLength(Request2.header()) == 1234);
   TST_ASSERT(Request2.header().headerValue(Request2.header().findHeader("Dog")) == "Ball");
   TST_ASSERT(Request2.method() == "GET");
   TST_ASSERT(Request2.location() == "/blah.html");
   TST_ASSERT(Request2.queryString() == "life=fun&dog=ball");
   
   COLstring Flatwire2;
   COLostream Stream2(Flatwire2);
   HTTPgenerateRequest(Stream2, Request2);
   COL_VAR(Flatwire2);
   TST_ASSERT(Flatwire == Flatwire2);

   HTTPrequest Request3;
   Parser.reset(Request3);
   Result = Parser.parse(Flatwire2.c_str(), Flatwire2.size());
   TST_ASSERT(Result == HTTP_PARSE_COMPLETE);

   TST_ASSERT(HTTPheaderGetContentLength(Request3.header()) == 1234);
   TST_ASSERT(Request3.header().headerValue(Request2.header().findHeader("Dog")) == "Ball");
   TST_ASSERT(Request3.method() == "GET");
   TST_ASSERT(Request3.location() == "/blah.html");
   TST_ASSERT(Request3.queryString() == "life=fun&dog=ball");
}

void testHTTPutils::testResponseUtils()
{  
   HTTPresponse Response;
   HTTPheader& ResponseHeader = Response.header();
   HTTPsetupEntity(Response, 100, "text/html");
   TST_ASSERT(HTTPheaderGetContentLength(ResponseHeader) == 100);
   TST_ASSERT(ResponseHeader.headerValue(ResponseHeader.findHeader("Content-Type")).find("text/html") != npos);

   HTTPresponse Response2;
   HTTPheader& ResponseHeader2 = Response2.header();
   COLstring MyString="ABC";
   HTTPsetupStringEntity(Response2, MyString, "text/plain");
   TST_ASSERT(HTTPheaderGetContentLength(ResponseHeader2) == MyString.size());
   TST_ASSERT(ResponseHeader2.headerValue(ResponseHeader.findHeader("Content-Type")).find("text/plain") != npos);
}

void testHTTPutils::testBadParse()
{
   COLstring BadRequest;
   COLostream Stream(BadRequest);
   Stream << "GET /blah HTTP/1.1\r\n"
          << "Host: localhost\r\n" 
          << "User-Agent:Bogus\r\n\r\n";

   HTTPrequest Request;
   HTTPrequestParser Parser(Request);

   HTTPparseResult Result = Parser.parse(BadRequest.c_str(), BadRequest.size());

   TST_ASSERT(Result == HTTP_PARSE_ERROR);
   TST_ASSERT(Parser.lastParseError() == "Expected space after header : character");

   COLstring Error;
   COLostream ErrorStream(Error);
   ErrorStream.setNewLine("\r\n");


   HTTPshowParseErrorLocation(BadRequest.c_str(), BadRequest.size(), Parser.lastParseIndex(), ErrorStream);
   COL_VAR(Error);

   COLstring ExpectedError;
   COLostream Stream2(ExpectedError);
   Stream2 << "GET /blah HTTP/1.1\r\n"
          << "Host: localhost\r\n" 
          << "User-Agent:Bogus\r\n"
          << "           ^\r\n";
   TST_ASSERT(ExpectedError == Error);
}

void testHTTPutils::testBadResponseParse()
{
   COLstring BadResponse;
   COLostream Stream(BadResponse);

   Stream << "HTTP/1.1 502 Bad Gateway\r\n"
          << "Content-Length: 122\r\n" 
          << "Content-Type:text/html\r\n"
          << "Cache-Control: max-age=0\r\n"
          << "Now: Then\r\n\r\n";

   HTTPresponse Response;
   HTTPresponseParser Parser(Response);

   HTTPparseResult Result = Parser.parse(BadResponse.c_str(), BadResponse.size());

   TST_ASSERT(Result == HTTP_PARSE_ERROR);
   TST_ASSERT(Parser.lastParseError() == "Expected space after header : character");

   COLstring Error;
   COLostream ErrorStream(Error);
   ErrorStream.setNewLine("\r\n");

   HTTPshowParseErrorLocation(BadResponse.c_str(), BadResponse.size(), Parser.lastParseIndex(), ErrorStream);
   COL_VAR(Error);

   COLstring ExpectedError;
   COLostream Stream2(ExpectedError);
   Stream2 << "HTTP/1.1 502 Bad Gateway\r\n"
           << "Content-Length: 122\r\n" 
           << "Content-Type:text/html\r\n"           
           << "             ^\r\n";

   TST_ASSERT(ExpectedError == Error);
}



void testHTTPutils::testRequestNoHeaders()
{
   COLstring RequestString;
   COLostream RequestStream(RequestString);
   RequestStream << "GET /blah HTTP/1.1\r\n"
          << "\r\n";

   HTTPrequest Request;
   HTTPrequestParser Parser(Request);
   HTTPparseResult Result = Parser.parse(RequestString.c_str(), RequestString.size());

   TST_ASSERT(Result == HTTP_PARSE_COMPLETE);
}


TSTtestable* testHTTPutils::testCollection()
{
   TST_START_FIXTURE(testHTTPutils)
      TST_ADD_TEST_METHOD(testHTTPsession);
      TST_ADD_TEST_METHOD(testHeaderParsingErrorHandling);
      TST_ADD_TEST_METHOD(testCookies);
      TST_ADD_TEST_METHOD(testRequestParsing);
      TST_ADD_TEST_METHOD(testBodyParsing);
      TST_ADD_TEST_METHOD(testHeaderParsing);
      TST_ADD_TEST_METHOD(testResponseObject);       
      TST_ADD_TEST_METHOD(testRequestObject);       
      TST_ADD_TEST_METHOD(testResponseUtils);  
      TST_ADD_TEST_METHOD(testBadParse);  
      TST_ADD_TEST_METHOD(testBadResponseParse);
      TST_ADD_TEST_METHOD(testRequestNoHeaders);
   TST_END_FIXTURE
}
