//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPresponse
//
// Description:
//
// Implementation
//
// Author: Rob Moyse (Eliot Muir)
// Date:   Friday, December 8th, 2006 @ 02:11:52 PM
//
//---------------------------------------------------------------------------
#include "HTTPprecomp.h"
#pragma hdrstop

#include "HTTPresponse.h"
#include "HTTPheader.h"

#include <COL/COLsink.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


class HTTPresponsePrivate {
public:
   HTTPresponsePrivate() : StatusCode(HTTPresponse::HTTP_STATUS_OK) {
      COL_METHOD(HTTPresponsePrivate::HTTPresponsePrivate);
   }
   ~HTTPresponsePrivate() {
      COL_METHOD(HTTPresponsePrivate::~HTTPresponsePrivate);
   }
   HTTPheader Header;
   int StatusCode;
};

HTTPresponse::HTTPresponse() {
   COL_METHOD(HTTPresponse::HTTPresponse);
   pMember = new HTTPresponsePrivate;
}

HTTPresponse::~HTTPresponse() {
   COL_METHOD(HTTPresponse::~HTTPresponse);
   delete pMember;
}

void HTTPresponse::setCode(int iStatusCode) {
   COL_METHOD(HTTPresponse::setCode);
   COL_VAR(iStatusCode);
   pMember->StatusCode = iStatusCode;
}

int HTTPresponse::code() const {
   COL_METHOD(HTTPresponse::code);
   return pMember->StatusCode;
}

void HTTPresponse::setEntityType(const COLstring& EntityType) {
   COL_METHOD(HTTPresponse::setEntityType);
   HTTP_PRECONDITION(npos == header().findHeader("Content-Type"));
   if(EntityType.find("text/") != npos && EntityType.find("charset") == npos) {
      header().addHeader("Content-Type", EntityType + "; charset=utf-8");
   } else {
      header().addHeader("Content-Type", EntityType);
   }
}

void HTTPresponse::setNoCaching() {
   COL_METHOD(HTTPresponse::setNoCaching);
   HTTP_PRECONDITION(npos == header().findHeader("Cache-Control"));
   header().addHeader("Cache-Control", "max-age=0");
}

void HTTPlookupReasonPhrase(int StatusCode, COLostream& Stream) {
   COL_FUNCTION(HTTPlookupReasonPhrase);
   switch (StatusCode)
   {
   case HTTPresponse::HTTP_STATUS_OK:                  
      Stream.sink()->write("OK",2);
      break;
   case HTTPresponse::HTTP_STATUS_CREATED:             
      Stream << COL_T("Created");
      break;
   case HTTPresponse::HTTP_STATUS_ACCEPTED:            
      Stream << COL_T("Accepted");
      break;
   case HTTPresponse::HTTP_STATUS_NO_CONTENT:          
      Stream << COL_T("No Content");
      break;
   case HTTPresponse::HTTP_STATUS_MOVED_PERM:          
      Stream << COL_T("Moved Permanently");
      break;
   case HTTPresponse::HTTP_STATUS_FOUND:               
      Stream << COL_T("Found");
      break;
   case HTTPresponse::HTTP_STATUS_NOT_MODIFIED:        
      Stream << COL_T("Not Modified");
      break;
   case HTTPresponse::HTTP_STATUS_BAD_REQUEST:         
      Stream << COL_T("Bad Request");
      break;
   case HTTPresponse::HTTP_STATUS_UNAUTHORIZED:        
      Stream << COL_T("Unauthorized");
      break;
   case HTTPresponse::HTTP_STATUS_FORBIDDEN:           
      Stream << COL_T("Forbidden");
      break;
   case HTTPresponse::HTTP_STATUS_NOT_FOUND:           
      Stream << COL_T("Not Found");
      break;
   case HTTPresponse::HTTP_STATUS_INTERNAL_ERROR:      
      Stream << COL_T("Internal Server Error");
      break;
   case HTTPresponse::HTTP_STATUS_NOT_IMPLEMENTED:     
      Stream << COL_T("Not Implemented");
      break;
   case HTTPresponse::HTTP_STATUS_BAD_GATEWAY:         
      Stream << COL_T("Bad Gateway");
      break;
   case HTTPresponse::HTTP_STATUS_SERVICE_UNAVAILABLE: 
      Stream << COL_T("Service Unavailable");
      break;
   default: 
      Stream << COL_T("Extension Code");
      break;
   }
}

void HTTPsetupEntity(HTTPresponse& Response, COLuint64 LengthOfData, const COLstring& EntityType) {
   COL_FUNCTION(HTTPsetupEntity);
   Response.setEntityType(EntityType);
   COL_VAR2(Response.header(), LengthOfData);
   HTTPheaderSetContentLength(Response.header(),LengthOfData);
}

void HTTPsetupStringEntity(HTTPresponse& Response, const COLstring& Data, const COLstring& EntityType) {
   COL_FUNCTION(HTTPsetupStringEntity);
   HTTPsetupEntity(Response, Data.size(), EntityType);
}

void HTTPgenerateResponse(COLostream& ResponseStream, const HTTPresponse& Response, bool AlwaysOutputContentLength){
   COL_FUNCTION(HTTPgenerateResponse);
   const HTTPheader& ResponseHeader = Response.header();
   COLstring Buffer;
   Buffer.setCapacity(200);
   COLostream BufferStream(Buffer);
   COLsink* BufferStreamSink = BufferStream.sink();
   COL_VAR(Response.code());
   // output status line
   BufferStreamSink->write("HTTP/", 5);
   BufferStream << ResponseHeader.version() << ' ' << Response.code() << ' ';
   HTTPlookupReasonPhrase(Response.code(), BufferStream);
   BufferStreamSink->write("\r\n", 2);
   
   bool HaveContentHeader = false;
   if (ResponseHeader.findHeader("Content-Length") != -1) {
      HaveContentHeader = true;
   }
   // output headers
   for (int HeaderIndex = 0; HeaderIndex <  ResponseHeader.countOfHeader(); HeaderIndex++){
      BufferStream <<  ResponseHeader.headerKey(HeaderIndex);
      BufferStreamSink->write(": ", 2);
      BufferStream << ResponseHeader.headerValue(HeaderIndex);
      BufferStreamSink->write("\r\n", 2);
   }

   //Content-Length may not be present in the 
   //header obj. 
   if (!HaveContentHeader && AlwaysOutputContentLength){
      BufferStream << COL_T("Content-Length: 0\r\n");
   }
   BufferStreamSink->write("\r\n", 2);
   ResponseStream.sink()->write(Buffer.c_str(), Buffer.size());
}

const HTTPheader& HTTPresponse::header() const {
   return pMember->Header;
}

HTTPheader& HTTPresponse::header() {
   return pMember->Header;   
}

COLostream& operator<<(COLostream& Stream, const HTTPresponse& Response){
   Stream << "HTTP Response: code=" << Response.code() << newline << Response.header();
   return Stream;
}

