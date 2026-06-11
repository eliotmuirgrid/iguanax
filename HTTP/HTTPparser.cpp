//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPparser
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Fri 24 Jan 2014 10:41:19 EST
//---------------------------------------------------------------------------
#include "HTTPprecomp.h"
#pragma hdrstop

#include "HTTPparser.h"
#include "HTTPbodyParser.h"
#include "HTTPrequestParser.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

const int HTTP_INIT_CAPACITY   =  8 * 1024 - 1; // Subtract 1 here because setCapacity adds a byte for trailing nil.
const int HTTP_HIGH_WATER_MARK = 60 * 1024;     // 60K - this seems reasonably optimal.
const int HTTP_CAPACITY_MARK   = 64 * 1024;     // High enough to make resizing of the buffer unlikely.

HTTPparser::HTTPparser() : m_pBuffer(NULL) {
   COL_METHOD(HTTPparser::HTTPparser);
}
HTTPparser::~HTTPparser(){
   COL_METHOD(HTTPparser::~HTTPparser);
}

HTTPparser::ParseResult HTTPparser::parse(HTTPrequestInfo* pPackage, COLstring* pError){
   COL_METHOD(HTTPparser::parse);
   if (m_pBuffer->size() == 0){
      COL_TRC("No data in buffer.");
      return HTTPparser::EmptyBuffer;
   }

   HTTPrequest Request;
   HTTPrequestParser HeaderParser(Request);
   COL_TRC("Try and parse headers.");
   HTTPparseResult ParseResult = HeaderParser.parse(m_pBuffer->c_str(), m_pBuffer->size());
   switch(ParseResult){
      case HTTP_PARSE_ERROR:
         COL_TRC("Got a parse error");
         *pError = HeaderParser.lastParseError();
         pError->append("\r\n");
         pError->append(*m_pBuffer);
         return HTTPparser::Bad;
      case HTTP_PARSE_NEED_MORE_DATA:
         COL_TRC("Header - need more data");
         return HTTPparser::MoreData;
      case HTTP_PARSE_COMPLETE:
         break;
      default:
         COLASSERT(1==0);
         return HTTPparser::Bad;
   }

   if (Request.header().findHeader("Expect") != -1 && m_pBuffer->size()-HeaderParser.lastParseIndex() == 0){
      COL_TRC("Got Expect header and no request body, handling cURL Expect");
      return HTTPparser::ExpectContinue;
   }

   HTTPextractGetVariables(Request, &pPackage->GetParameters);

   HTTPbodyParser BodyParser;
   if (!pPackage->Body.is_null()) {
      COL_TRC("Clearing body.");   
      pPackage->Body.clear(); 
   }
   COLsinkString BodySink(&pPackage->Body, false);
   COL_TRC("Parse body.");
   ParseResult = BodyParser.parse(m_pBuffer->c_str() + HeaderParser.lastParseIndex(), 
                       m_pBuffer->size()-HeaderParser.lastParseIndex(), Request.header(), BodySink);
   COLuint32 HeaderIndex = Request.header().findHeader("Content-Type");

   switch(ParseResult){
      case HTTP_PARSE_CHUNK_COMPLETE:
      case HTTP_PARSE_NEED_MORE_DATA:
         COL_TRC("Body - need more data");
         return HTTPparser::MoreData;
      case HTTP_PARSE_ERROR:
         *pError = BodyParser.lastError();
         pError->append("\r\n");
         pError->append(*m_pBuffer);
         return HTTPparser::Bad;
      case HTTP_PARSE_COMPLETE:
         pPackage->Request = Request;  // copy the request
         pPackage->Header  = COLstring(m_pBuffer->c_str(), HeaderParser.lastParseIndex());
         pPackage->OriginalData = *m_pBuffer;
         m_pBuffer->shift(HeaderParser.lastParseIndex() + BodyParser.parseIndex());
         COL_TRC("Size of request: " << pPackage->Header.size() + pPackage->Body.size());

         // parse form data in body
         if ( HeaderIndex != -1 && Request.header().headerValue(HeaderIndex).find("form") != npos ) {
            HTTPextractPostVariables((const char*)pPackage->Body.data(), pPackage->Body.size(), &pPackage->PostParameters);
         }

         return HTTPparser::Success;
         break;
      default:
         COLASSERT(1==0);
         return HTTPparser::Bad;
   }
}

HTTPparser::ParseResult HTTPparseRequest(HTTPrequestInfo* pPackage, COLstring* pError,
      COLstring* pBuffer){
   COL_FUNCTION(HTTPparseRequest);
   HTTPparser Parser;
   Parser.setBuffer(pBuffer);
   return Parser.parse(pPackage, pError);
}


COLostream& operator<<(COLostream& Stream, const HTTPrequestInfo& Info){
   Stream << "Request: " << Info.Request;
   return Stream;
}
