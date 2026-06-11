//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPheader
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, December 11th, 2006 @ 12:05:17 PM
//
//---------------------------------------------------------------------------
#include "HTTPprecomp.h"
#pragma hdrstop

#include "HTTPheader.h"

#include <COL/COLmath.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

HTTPheader::HTTPheader() : m_ContentLength(0), m_Version("1.1"){
   COL_METHOD(HTTPheader::HTTPheader);
}

HTTPheader::~HTTPheader(){
   COL_METHOD(HTTPheader::~HTTPheader);
}

void HTTPheader::addHeader(const COLstring& Key, const COLstring& Value){
   COL_METHOD(HTTPheader::addHeader);
   COL_VAR2(Key, Value);
   m_HeaderVector.push_back();
   m_HeaderVector.back().Key = Key;
   m_HeaderVector.back().Value = Value;
}

void HTTPheader::addHeader(const char* pHeader, COLuint32 SizeOfHeader, const char* pValue, COLuint32 SizeOfValue){
   COL_METHOD(HTTPheader::addHeader);
   COLstring Key(pHeader, SizeOfHeader);
   COLstring Value(pValue, SizeOfValue);
   COL_VAR2(Key, Value);
   m_HeaderVector.push_back();
   m_HeaderVector.back().Key = Key;
   m_HeaderVector.back().Value = Value;
}

void HTTPheader::removeHeader(COLuint32 HeaderIndex){
   m_HeaderVector.remove(HeaderIndex);
}

COLuint32 HTTPheader::countOfHeader() const{
   return m_HeaderVector.size();
}

const COLstring& HTTPheader::headerKey(COLuint32 HeaderIndex) const{
   return m_HeaderVector[HeaderIndex].Key;
}

const COLstring& HTTPheader::headerValue(COLuint32 HeaderIndex) const{
   return m_HeaderVector[HeaderIndex].Value;
}

COLuint32 HTTPheader::findHeader(const COLstring& HeaderName) const{
   COL_METHOD(HTTPheader::findHeader);
   int Size = m_HeaderVector.size();
   for (COLuint32 i=0; i< Size; i++){
      COL_VAR2(i,m_HeaderVector[i].Key);
      if (m_HeaderVector[i].Key.equalsIgnoreCase(HeaderName)){
         return i;   
      }
   }
   return (COLuint32)-1;
}

const COLstring& HTTPheader::version() const{
   return m_Version;
}

void HTTPheader::setVersion(const COLstring& Version)
{
   m_Version = Version;
}

void HTTPheader::replaceHeader(COLuint32 HeaderIndex, const COLstring& Key, const COLstring& Value){
   m_HeaderVector[HeaderIndex].Key = Key;
   m_HeaderVector[HeaderIndex].Value = Value;
}

COLuint64 HTTPheaderGetContentLength(const HTTPheader& Header){
   COL_FUNCTION(HTTPheaderGetContentLength);
   COLuint32 HeaderIndex = Header.findHeader("Content-Length");
   if (HeaderIndex != npos){
      return ::COLmath::atoi64(Header.headerValue(HeaderIndex).c_str());
   } else {
      return 0;
   }
}

bool HTTPheaderHasContentLength(const HTTPheader& Header){
   return Header.findHeader("Content-Length") != npos;
}

void HTTPheaderSetContentLength(HTTPheader& Header, COLuint64 ContentLength){
   COLstring StringValue;
   COLostream Stream(StringValue);
   Stream << ContentLength;
   COLuint32 HeaderIndex = Header.findHeader("Content-Length");
   if (HeaderIndex != npos){
      Header.removeHeader(HeaderIndex);
   }
   Header.addHeader("Content-Length",StringValue);
}

bool HTTPheaderGetGzipSupported(const HTTPheader& Header){
   COL_FUNCTION(HTTPheaderGetGzipSupported);
   COLuint32 HeaderIndex = Header.findHeader("Accept-Encoding");
   if (HeaderIndex != npos){
      COLstring Value( Header.headerValue(HeaderIndex) );
      COL_VAR(Value);
      Value.toLowerCase();
      COL_VAR(Value);
#ifdef COL_ENABLE_TRACING
      bool GzipOn = Value.find("gzip") != npos;
      COL_VAR(GzipOn);
#endif
      return (Value.find("gzip") != npos);
   } else{
      COL_TRC("No Accept-Encoding header found");
   }
   COL_TRC("GZIP encoding not supported");
   return false;
}

bool HTTPheaderGetKeepAlive(const HTTPheader& Header){
   COL_FUNCTION(HTTPheaderGetKeepAlive);
   COLuint32 HeaderIndex = Header.findHeader("Connection");
   if (HeaderIndex != npos){
      COLstring Value( Header.headerValue(HeaderIndex) );
      COL_TRC("Connection: " << Value);
      Value.toLowerCase();
      COL_VAR(Value);
      if (Value == "close") {
         COL_TRC("Connection: close received - we should close after this connection.");
         return false;
      }
      
      return (Value == "keep-alive") != 0;
   }
   else if (Header.version() == "1.1"){
      COL_TRC("Header is 1.1 so keep alive is true.");
      //No Connection header, but if version is 1.1, then keep-alive is true
      return true;
   }
   return false;
}

void HTTPheaderSetKeepAlive(HTTPheader& Header, bool KeepAlive){
   COLuint32 HeaderIndex = Header.findHeader("Connection");
   if (HeaderIndex != npos){
      Header.removeHeader(HeaderIndex);
   }
   if (KeepAlive){
      Header.addHeader("Connection","keep-alive");
   } else {
      Header.addHeader("Connection","close");
   }
}

COLostream& operator<<(COLostream& Stream, const HTTPheader& Header){
   for (int i=0; i < Header.countOfHeader(); i++){
      Stream << Header.headerKey(i) << ": " << Header.headerValue(i) << newline;
   }
   return Stream;
}
