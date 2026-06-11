#ifndef __HTTP_BODY_PARSER_H__
#define __HTTP_BODY_PARSER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPbodyParser
//
// Description:
//
// HTTPbodyParser class.  For extracting either chunked or full HTTP body.
//
// Author: Ramanan Sivasothinathan
// Date:   Thursday, September 10th, 2009 @ 11:57:49 AM
//---------------------------------------------------------------------------

#include "HTTPparseResult.h"

class HTTPbodyParserPrivate;
class HTTPheader;
class COLsink;

class HTTPbodyParser{
public:
   // If CombineChunks==false parse(...) will return HTTP_PARSE_CHUNK_COMPLETE at the end of each non-empty chunk and
   //                                         return HTTP_PARSE_COMPLETE when empty (terminating) chunk is encountered
   HTTPbodyParser(bool CombineChunks = true);
   ~HTTPbodyParser();
   void reset();                // Invoke reset() to reuse the parser for another request
   COLuint32 parseIndex();      // # bytes in Input data that has already been processed
   void resetParseIndex();      // Reset Parse Index to 0
   const COLstring& lastError() const;
   HTTPparseResult parse
   (
      const void* pData,        // Input data
      COLuint32 SizeOfData,     // Size of input data
      const HTTPheader& Header, // Header (for getting "Transfer-Encoding"/"Content-Length")
      COLsink& Body             // Output 
   );

private:
   HTTPbodyParserPrivate* pMember;
   HTTPbodyParser(const HTTPbodyParser& Orig);            // not allowed
   HTTPbodyParser& operator=(const HTTPbodyParser& Orig); // not allowed
};

#endif // end of defensive include
