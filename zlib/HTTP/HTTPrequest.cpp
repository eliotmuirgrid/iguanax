//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPrequest
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday, December 7th, 2006 @ 03:20:50 PM
//---------------------------------------------------------------------------
#include "HTTPprecomp.h"
#pragma hdrstop

#include "HTTPrequest.h"

#include <COL/COLsink.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


HTTPrequest::HTTPrequest(){
   COL_METHOD(HTTPrequest::HTTPrequest);
}

HTTPrequest::~HTTPrequest(){
   COL_METHOD(HTTPrequest::~HTTPrequest);
}

const COLstring& HTTPrequest::queryString() const{
   return m_QueryString;
}

void HTTPrequest::setQueryString(const COLstring& Value){
   COL_METHOD(HTTPrequest::setQueryString);
   COL_VAR(Value);
   m_QueryString = Value;
}

const COLstring& HTTPrequest::method() const
{
   return m_Method;
}

void HTTPrequest::setMethod(const COLstring& Method){
   COL_METHOD(HTTPrequest::setMethod);
   COL_VAR(Method);
   m_Method = Method;
}

const COLstring& HTTPrequest::location() const{
   return m_Location;
}

void HTTPrequest::setLocation(const COLstring& Location){
   COL_METHOD(HTTPrequest::setLocation);
   COL_VAR(Location);
   m_Location = Location;
}

COLstring HTTPrequest::requestString() const{
   COLstring RequestString;
   COLostream RequestStream( RequestString );
   RequestStream << method() << ' ' << location(); 
   if (!queryString().is_null())
   {
      RequestStream << '?' << queryString();   
   }   
   RequestStream << COL_T(" HTTP/") << header().version();
   return RequestString;
}

HTTPheader& HTTPrequest::header(){
   return m_Header;
}

const HTTPheader& HTTPrequest::header() const{
   return m_Header;
}

void HTTPgenerateRequest(COLostream& Stream, const HTTPrequest& Request){
   COL_FUNCTION(HTTPgenerateRequest);
   const HTTPheader& RequestHeader = Request.header();
   Stream << Request.method() << ' ' << Request.location();
   if (!Request.queryString().is_null()){
      Stream << '?' << Request.queryString();
   }
      
   Stream.sink()->write(" HTTP/", 6);
   Stream << RequestHeader.version();
   Stream.sink()->write("\r\n", 2);

   int CountOfHeader = RequestHeader.countOfHeader();
   for (int i = 0; i <  CountOfHeader; i++){
      Stream <<  RequestHeader.headerKey(i);
      Stream.sink()->write(": ", 2);
      Stream << RequestHeader.headerValue(i);
      Stream.sink()->write("\r\n", 2);
   }
   Stream.sink()->write("\r\n", 2);   
}

COLostream& operator<<(COLostream& Stream, const HTTPrequest& Request){
   HTTPgenerateRequest(Stream, Request);
   return Stream;
}
