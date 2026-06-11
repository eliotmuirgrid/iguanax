#ifndef __HTTP_PARSER_H__
#define __HTTP_PARSER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPparser
//
// Description:
//
// The HTTP library contains a lot components that can go into implementing a web server.  In
// the past it was up to libraries like WEB to handle the buffering etc. to put these together
// on top of a socket stack.
//
// The purpose of this class is to try and wrap up all things to do with buffering and parsing
// into one package which can then be hooked up on a top of socket implementation without less
// additional code (and get it right!).
//
// Author: Eliot Muir
// Date:   Fri 24 Jan 2014 10:34:13 EST
//---------------------------------------------------------------------------

#include "HTTPrequest.h"
#include "HTTPvariables.h"

#include <COL/COLhashmap.h>

class HTTPrequestInfo{
public:
   HTTPrequest Request;
   COLstring   Header;
   COLstring   Body;
   HTTPvarMap  GetParameters;
   HTTPvarMap  PostParameters;
   COLstring   OriginalData;
};

COLostream& operator<<(COLostream& Stream, const HTTPrequestInfo& Info);

class HTTPparser{
public:
   HTTPparser();
   // Default copy constructor and assignment operator should work since it's plain old data.
   ~HTTPparser();

   void setBuffer(COLstring* pBuffer) { m_pBuffer = pBuffer;}

   enum ParseResult{
      Bad,
      MoreData,
      ExpectContinue,
      EmptyBuffer,
      Success
   };

   ParseResult parse(HTTPrequestInfo* pPackage, COLstring* pError);
private:
   COLstring* m_pBuffer;
};

// TODO - For now, it truncates the buffer after a successful parse. In the future maybe,
//        let the application decide what to do?
HTTPparser::ParseResult HTTPparseRequest(HTTPrequestInfo* pPackage, COLstring* pError,
      COLstring* pBuffer);

#endif // end of defensive include
