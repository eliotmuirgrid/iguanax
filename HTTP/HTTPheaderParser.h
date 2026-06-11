#ifndef __HTTP_HEADER_PARSER_H__
#define __HTTP_HEADER_PARSER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPheaderParser
//
// Description:
//
// HTTPheaderParser class used to parse the MIME headers.  It could
// really be part of the private implementation of this library - application
// code shouldn't need to use this interface.
//
// Author: Eliot Muir
// Date:   Monday, December 11th, 2006 @ 12:05:23 PM
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include "HTTPparseResult.h"

class HTTPheader;

enum HTTPparseState{
   HTTP_PARSING_HEADER,
   HTTP_PARSING_VALUE,
   HTTP_PARSING_DONE
};

class HTTPheaderParser{
public:
   HTTPheaderParser();
   HTTPheaderParser(HTTPheader& Header);
   // This is non virtual because we are assuming no classes will inherit from this class
   ~HTTPheaderParser() {}

   void reset(HTTPheader& Header);

   HTTPparseResult parse(const char* pData, int SizeOfData);

   int currentParseIndex() const { return CurrentParseIndex; }
   int sizeOfData() const { return SizeOfData; }
   
   char currentChar() { return pData[currentParseIndex()];}

   void incPosition() { CurrentParseIndex++; }

   void foundHeader();
   void foundValue();
   
   const COLstring& lastError() const;
   void setLastError(const COLstring& Error);
private:
   COLstring LastError;

   int StartOfHeader;
   int EndOfHeader;

   int LastTokenIndex;
   int CurrentParseIndex;
   const char* pData;
   int SizeOfData;
   HTTPparseState ParseState;
   HTTPheader* pHeader;

   HTTPheaderParser(const HTTPheaderParser& Orig); // not allowed
   HTTPheaderParser& operator=(const HTTPheaderParser& Orig); // not allowed
};

const void* HTTPfindNewline(const void* cpBuffer, int SizeOfData);

#endif // end of defensive include
