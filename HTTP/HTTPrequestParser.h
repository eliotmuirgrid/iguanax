#ifndef __HTTP_REQUEST_PARSER_H__
#define __HTTP_REQUEST_PARSER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPrequestParser
//
// Description:
//
// HTTPrequestParser - used to parse HTTP requests.
//
// Author: Eliot Muir
// Date:   Monday, December 11th, 2006 @ 01:35:45 PM
//---------------------------------------------------------------------------

#include "HTTPparseResult.h"

class HTTPheaderParser;
class HTTPrequest;
class HTTPheader;

enum HTTPrequestParseState{
   HTTP_REQ_PARSING_REQUEST,
   HTTP_REQ_PARSING_HEADER,
   HTTP_REQ_PARSING_COMPLETE,
   HTTP_REQ_PARSING_ERROR
};

class HTTPrequestParser{
public:
   HTTPrequestParser();
   HTTPrequestParser(HTTPrequest& Request);
   // This is non virtual because we are assuming no classes will inherit from this class
   ~HTTPrequestParser();

   // If you reuse the parser object then reset should set things back
   // as if the parser has been first created - although you'll need to
   // set the request
   void reset(HTTPrequest& Request);

   HTTPparseResult parse(const char* pData, int SizeOfData);

   const COLstring& lastParseError() const;

   int lastParseIndex() const;

   int endOfHeader() const;
private:
   HTTPrequest* pRequest;
   HTTPheaderParser* pHeaderParser;
   const char* pData;
   int SizeOfData;
   int EndOfRequestLine;
   int LastParseIndex;
   COLstring LastParseError;
   HTTPrequestParseState ParseState;

   HTTPrequestParser(const HTTPrequestParser& Orig); // not allowed
   HTTPrequestParser& operator=(const HTTPrequestParser& Orig); // not allowed
};

// takes a string and replaces all the escaped characters
// with their actual ascii values
void HTTPunescapeCharacters(COLstring& Value);

#endif // end of defensive include
