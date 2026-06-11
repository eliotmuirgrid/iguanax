#ifndef __HTTP_RESPONSE_PARSER_H__
#define __HTTP_RESPONSE_PARSER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPresponseParser
//
// Description:
//
// HTTPresponseParser class.
//
// Author: Eliot Muir
// Date:   Tuesday, December 12th, 2006 @ 12:15:42 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include "HTTPparseResult.h"

class HTTPheaderParser;
class HTTPresponse;

enum HTTPresponseParseState
{
   HTTP_RES_PARSING_RESPONSE,
   HTTP_RES_PARSING_HEADER,
   HTTP_RES_PARSING_COMPLETE,
   HTTP_RES_PARSING_ERROR
};

class HTTPresponseParser 
{
public:
   HTTPresponseParser(HTTPresponse& Response);
   // This is non virtual because we are assuming no classes will inherit from this class
   ~HTTPresponseParser();

   // If you reuse the parser object then reset should set things back
   // as if the parser has been first created - although you'll need to
   // set the request
   void reset(HTTPresponse& Response);

   HTTPparseResult parse(const char* pData, COLuint32 SizeOfData);

   const COLstring& lastParseError() const;

   COLuint32 lastParseIndex() const;

   COLuint32 endOfHeader() const;
private:
   HTTPresponse* pResponse;
   HTTPheaderParser* pHeaderParser;
   const char* pData;
   COLuint32 SizeOfData;
   COLuint32 EndOfResponseLine;
   COLuint32 LastParseIndex;
   COLstring LastParseError;
   HTTPresponseParseState ParseState;

   HTTPresponseParser(const HTTPresponseParser& Orig); // not allowed
   HTTPresponseParser& operator=(const HTTPresponseParser& Orig); // not allowed
};

#endif // end of defensive include
