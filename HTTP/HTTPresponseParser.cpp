//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPresponseParser
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, December 12th, 2006 @ 12:15:42 PM
//
//---------------------------------------------------------------------------
#include "HTTPprecomp.h"
#pragma hdrstop

#include "HTTPresponseParser.h"
#include "HTTPheaderParser.h"
#include "HTTPresponse.h"
#include "HTTPheader.h"

#include <memory.h>  // for memchr
#include <COL/COLmath.h>  // COLmath::atoi64

HTTPresponseParser::HTTPresponseParser(HTTPresponse& Response)
: pResponse(&Response),
  EndOfResponseLine(0),
  LastParseIndex(0),
  ParseState(HTTP_RES_PARSING_RESPONSE)
{
   pHeaderParser = new HTTPheaderParser(Response.header());
}

HTTPresponseParser::~HTTPresponseParser()
{
   delete pHeaderParser;
}

void HTTPresponseParser::reset(HTTPresponse& Response)
{
   pResponse = &Response;
   pHeaderParser->reset(Response.header());
   EndOfResponseLine = 0;
   LastParseIndex = 0;
   ParseState = HTTP_RES_PARSING_RESPONSE;
}

COLuint32 HTTPresponseParser::lastParseIndex() const
{
   return LastParseIndex;
}

COLuint32 HTTPresponseParser::endOfHeader() const
{
   return LastParseIndex;
}

COLuint32 HTTPparseResponseLine(HTTPresponse& Response, const void* cpBuffer, COLuint32 SizeOfData)
{
   const void* pNewLine = HTTPfindNewline(cpBuffer, SizeOfData);
   if (!pNewLine)
   {
      return 0;
   }

   // This code doesn't attempt to be too optimal.
   COLstring ResponseLine;
   COLuint32 SizeOfLine = (const COLuint8*)(pNewLine) - (const COLuint8*)cpBuffer;
   
   ResponseLine.write(cpBuffer, SizeOfLine);

   COLstring Version;
   COLstring Code;
   COLstring Junk;

   ResponseLine.split(Version,Junk," ");
   Junk.split(Code,Junk," ");

   Response.header().setVersion(Version.substr(5, 3));
   Response.setCode(::atoi(Code.c_str()));
   
   return SizeOfLine;
}

HTTPparseResult HTTPresponseParser::parse(const char* ipData, COLuint32 iSizeOfData)
{
   pData = ipData;
   SizeOfData = iSizeOfData;
   switch(ParseState)
   {
   case HTTP_RES_PARSING_RESPONSE:
      EndOfResponseLine = HTTPparseResponseLine(*pResponse, pData, SizeOfData);
      if (EndOfResponseLine == 0)
      {
         // still haven't got the newline so return 1 since we need more data.
         return HTTP_PARSE_NEED_MORE_DATA;
      }
      else
      {
         // we got the new line
         ParseState = HTTP_RES_PARSING_HEADER;
      }
      if (SizeOfData <= EndOfResponseLine)
      {
         // need more data
         return HTTP_PARSE_NEED_MORE_DATA;
      }
   case HTTP_RES_PARSING_HEADER:
      {      
         HTTPparseResult Result = pHeaderParser->parse(pData + EndOfResponseLine, SizeOfData-EndOfResponseLine);
         LastParseIndex = pHeaderParser->currentParseIndex() + EndOfResponseLine;
         if (Result == HTTP_PARSE_ERROR)
         {
            LastParseError = pHeaderParser->lastError();
            LastParseIndex = pHeaderParser->currentParseIndex() + EndOfResponseLine-2;
            ParseState = HTTP_RES_PARSING_ERROR;
            return HTTP_PARSE_ERROR;
         }
         else if (Result == HTTP_PARSE_NEED_MORE_DATA)
         {         
            // need more data
            return HTTP_PARSE_NEED_MORE_DATA;
         }
         COLASSERT(Result == HTTP_PARSE_COMPLETE);
         ParseState = HTTP_RES_PARSING_COMPLETE;
         return HTTP_PARSE_COMPLETE;
      }
      break;
   default:
      COLASSERT(1==0);  // we shouldn't have been called in this state.
      return HTTP_PARSE_ERROR;
   }   
}

const COLstring& HTTPresponseParser::lastParseError() const
{
   return LastParseError;
}
