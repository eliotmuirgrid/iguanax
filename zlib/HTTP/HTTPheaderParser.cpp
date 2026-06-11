//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPheaderParser
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, December 11th, 2006 @ 12:05:23 PM
//---------------------------------------------------------------------------
#include "HTTPprecomp.h"
#pragma hdrstop

#include "HTTPheaderParser.h"
#include "HTTPheader.h"

#include <COL/COLmath.h>  // COLmath::atoi64
#include <memory.h>  // for memchr

HTTPheaderParser::HTTPheaderParser()
: ParseState(HTTP_PARSING_HEADER),
  CurrentParseIndex(0),
  LastTokenIndex(0),
  pHeader(NULL)
{
}

HTTPheaderParser::HTTPheaderParser(HTTPheader& Header)
: ParseState(HTTP_PARSING_HEADER),
  CurrentParseIndex(0),
  LastTokenIndex(0),
  pHeader(&Header)
{
}

void HTTPheaderParser::reset(HTTPheader& Header){
   ParseState = HTTP_PARSING_HEADER;
   CurrentParseIndex = 0;
   LastTokenIndex = 0;
   pHeader = &Header;
}

HTTPparseResult HTTPparseHeader(HTTPheaderParser& Parser);
HTTPparseResult HTTPparseValue(HTTPheaderParser& Parser);

HTTPparseResult HTTPheaderParser::parse(const char* ipData, int iSizeOfData){
   pData = ipData;
   SizeOfData = iSizeOfData;
   if (SizeOfData >= 2 && ipData[0] == '\r' && ipData[1] == '\n')
   {
      return HTTP_PARSE_COMPLETE;
   }
   if (SizeOfData < 3)
   {
      return HTTP_PARSE_NEED_MORE_DATA;
   }
 
   switch (ParseState)
   {
   case HTTP_PARSING_HEADER:
      return HTTPparseHeader(*this);
      break;
   case HTTP_PARSING_VALUE:
      return HTTPparseValue(*this);
      break;
   default:
      COLASSERT(1==0); // we should not have been called in this state.
   }
   return HTTP_PARSE_ERROR;
}

const COLstring& HTTPheaderParser::lastError() const{
   return LastError;
}

void HTTPheaderParser::setLastError(const COLstring& Error){
   LastError = Error;
}

void HTTPheaderParser::foundHeader(){
   StartOfHeader = LastTokenIndex;
   EndOfHeader = CurrentParseIndex-2;
   ParseState = HTTP_PARSING_VALUE;
}

void HTTPheaderParser::foundValue(){
   pHeader->addHeader(pData+StartOfHeader, EndOfHeader-StartOfHeader, pData+ EndOfHeader+2, CurrentParseIndex-EndOfHeader-4);
   LastTokenIndex = CurrentParseIndex;
   ParseState = HTTP_PARSING_HEADER;
}

HTTPparseResult HTTPparseHeader(HTTPheaderParser& Parser){
   while (Parser.currentParseIndex() < Parser.sizeOfData()-1 )
   {
      switch (Parser.currentChar())
      {
      case ':':
         {
            Parser.incPosition();
            switch (Parser.currentChar())
            {
            case ' ':
               // good
               Parser.incPosition();
               Parser.foundHeader();
               return HTTPparseValue(Parser);
               break;
            default:              
               Parser.setLastError("Expected space after header : character");
               return HTTP_PARSE_ERROR;
            }
         }
      default:
         Parser.incPosition();
         break;
      }
   }
   return HTTP_PARSE_NEED_MORE_DATA;
}

// TODO we could profile this and examine the generated machine code.  VC++ can
// actually set up some pretty efficient jump tables with switch statement although
// one has to play with the cases - sometimes a contiguous set of case statements
// from 0 --> \r might do the trick.  You have to profile and look at the machine code
// actually generated though.
HTTPparseResult HTTPparseValue(HTTPheaderParser& Parser){
   while (Parser.currentParseIndex() < Parser.sizeOfData()-3 )
   {
      switch (Parser.currentChar())
      {
      case '\r':
         {         
            Parser.incPosition();
            switch(Parser.currentChar())
            {
            case '\n':
               // good
               Parser.incPosition();
               Parser.foundValue();               
               switch(Parser.currentChar())
               {
               case '\r':
                  Parser.incPosition();
                  switch (Parser.currentChar())
                  {
                  case '\n':
                     // good we got to the end of the headers
                     Parser.incPosition();
                     return HTTP_PARSE_COMPLETE;
                  default:
                     // bad
                     Parser.setLastError("Expected \\n after \\r at end of header block");
                     return HTTP_PARSE_ERROR;
                  }
               default:
                  return HTTPparseHeader(Parser);
               }
               break;
            default:
               Parser.setLastError("Expected \\n after \\r");
               return HTTP_PARSE_ERROR;
               break;
            }

         }
         break;
      default:
         Parser.incPosition();
         break;
      }
   }
   return HTTP_PARSE_NEED_MORE_DATA;
}


const void* HTTPfindNewline(const void* cpBuffer, int SizeOfData){
   //search for \r\n
   // I think this now makes reasonable assumptions with respect to fragmented
   // packages since I have tested the logic now - Eliot
   const COLuint8* pStart = (const COLuint8*)cpBuffer;
   if (const COLuint8* pCr = (const COLuint8*)::memchr(cpBuffer,'\r',SizeOfData)){
      if (((pCr - pStart) < SizeOfData-1) && *(pCr+1) == '\n'){ //not last char
         return pCr+2;
      }
   }
   return 0;
}
