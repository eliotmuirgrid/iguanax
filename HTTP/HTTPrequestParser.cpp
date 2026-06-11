//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPrequestParser
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, December 11th, 2006 @ 01:35:45 PM
//---------------------------------------------------------------------------
#include "HTTPprecomp.h"
#pragma hdrstop

#include "HTTPrequestParser.h"
#include "HTTPheaderParser.h"
#include "HTTPrequest.h"
#include "HTTPheader.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <ctype.h>

HTTPrequestParser::HTTPrequestParser()
: pRequest(NULL),
  EndOfRequestLine(0),
  LastParseIndex(0),
  ParseState(HTTP_REQ_PARSING_REQUEST)
{
   pHeaderParser = new HTTPheaderParser();
}

HTTPrequestParser::HTTPrequestParser(HTTPrequest& Request)
: pRequest(&Request),
  EndOfRequestLine(0),
  LastParseIndex(0),
  ParseState(HTTP_REQ_PARSING_REQUEST)
{
   pHeaderParser = new HTTPheaderParser(Request.header());
}

HTTPrequestParser::~HTTPrequestParser()
{
   delete pHeaderParser;
}

void HTTPrequestParser::reset(HTTPrequest& Request)
{
   pRequest = &Request;
   pHeaderParser->reset(Request.header());
   EndOfRequestLine = 0;
   LastParseIndex = 0;
   ParseState = HTTP_REQ_PARSING_REQUEST;
}

int HTTPrequestParser::lastParseIndex() const{
   return LastParseIndex;
}

int HTTPrequestParser::endOfHeader() const{
   return LastParseIndex;
}

int HTTPparseRequestLine(HTTPrequest& Request, const void* cpBuffer, int SizeOfData){
   const void* pNewLine = HTTPfindNewline(cpBuffer, SizeOfData);
   if (!pNewLine){
      return 0;
   }
   COLstring RequestLine;
   COLuint32 SizeOfLine = (const COLuint8*)(pNewLine) - (const COLuint8*)cpBuffer;
   RequestLine.write(cpBuffer, SizeOfLine);
   COL_INF(RequestLine);

   COLstring RequestMethod,URI,Version;
   COLstring Rest0,Rest1,Rest2;
   RequestLine.split(RequestMethod,Rest0," ");
   Rest0.split(URI,Rest1," ");
   Rest1.split(Version,Rest2," ");
   // TODO - slight problem - if there is no HTTP version string we can
   // end up setting the version string to nothing....
   Version = Version.substr(5, 3); 
   
   Request.setMethod( RequestMethod );
   Request.header().setVersion( Version );

   // split out the request location component from
   // the request URI
   const size_t GetIndex = URI.find("?");
   
   if (GetIndex == npos)
   {
      Request.setQueryString("");
      HTTPunescapeCharacters(URI);
      Request.setLocation(URI);
   }
   else
   {
      COLstring RequestLocation  = URI.substr(0, GetIndex);
      // deal with escaped characters
      HTTPunescapeCharacters(RequestLocation);
      Request.setLocation(RequestLocation);
      Request.setQueryString(URI.substr(GetIndex+1));
   }

   return SizeOfLine;
}

void HTTPunescapeCharacters(COLstring& Value){
   const char* pIn = Value.c_str();
   char* pOut = (char*)Value.c_str();
   for(;;) {
      int Length = strcspn(pIn, "%+");
      memmove(pOut, pIn, Length);
      pOut += Length;
      pIn += Length;
      switch(*pIn++) {
      default:
         Value.setSize(pOut - Value.c_str());
         return;
      case '+':
         *pOut++ = ' ';
         break;
      case '%':
         if(!isxdigit(pIn[0]) || !isxdigit(pIn[1])) {
            *pOut++ = '%';
         } else {
            char Hex[3];
            Hex[0]=*pIn++; Hex[1]=*pIn++; Hex[2]=0;
            *pOut++ = strtol(Hex, NULL, 16);
         }
      }
   }
}

HTTPparseResult HTTPrequestParser::parse(const char* ipData, int iSizeOfData){
   COL_METHOD(HTTPrequestParser::parse);
   COL_VAR(iSizeOfData);
   COL_DUMP("Data: ", ipData, iSizeOfData);
   pData = ipData;
   SizeOfData = iSizeOfData;
   switch(ParseState)
   {
   case HTTP_REQ_PARSING_REQUEST:
      EndOfRequestLine = HTTPparseRequestLine(*pRequest, pData, SizeOfData);
      if (EndOfRequestLine == 0)
      {
         // still haven't got the newline so return 1 since we need more data.
         return HTTP_PARSE_NEED_MORE_DATA;
      }
      else
      {
         // we got the new line
         ParseState = HTTP_REQ_PARSING_HEADER;
      }
      if (SizeOfData <= EndOfRequestLine)
      {
         // need more data
         return HTTP_PARSE_NEED_MORE_DATA;
      }
   case HTTP_REQ_PARSING_HEADER:
      {      
         HTTPparseResult Result = pHeaderParser->parse(pData + EndOfRequestLine, SizeOfData-EndOfRequestLine);
         LastParseIndex = pHeaderParser->currentParseIndex() + EndOfRequestLine;
         if (Result == HTTP_PARSE_ERROR)
         {
            LastParseError = pHeaderParser->lastError();
            LastParseIndex = pHeaderParser->currentParseIndex() + EndOfRequestLine-2;
            ParseState = HTTP_REQ_PARSING_ERROR;
            return HTTP_PARSE_ERROR;
         }
         else if (Result == HTTP_PARSE_NEED_MORE_DATA)
         {         
            // need more data
            return HTTP_PARSE_NEED_MORE_DATA;
         }
         COLASSERT(Result == HTTP_PARSE_COMPLETE);
         ParseState = HTTP_REQ_PARSING_COMPLETE;
         return HTTP_PARSE_COMPLETE;
      }
      break;
   default:
      COLASSERT(1==0);  // we shouldn't have been called in this state.
      return HTTP_PARSE_ERROR;
   }   
}

const COLstring& HTTPrequestParser::lastParseError() const{
   return LastParseError;
}

