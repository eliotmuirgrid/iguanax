#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPrequest
//
// Description:
//
// HTTPrequest - simple plain old data class that we parse out a HTTP request into.
//
// Author: Eliot Muir (ruthlessly plaguerized from Nasron and Rob)
// Date:   Thursday, December 7th, 2006 @ 03:20:50 PM
//---------------------------------------------------------------------------

#include <HTTP/HTTPheader.h>

class COLstring;

class HTTPrequest{
public:
   HTTPrequest();
   // default assignment operators should work
   ~HTTPrequest();

   // access header information
   const HTTPheader& header() const;
   HTTPheader& header();

   // Outputs the request string - i.e the first line but without the headers.
   //   See: http://www.w3.org/Protocols/HTTP/1.0/spec.html#Request
   COLstring requestString() const;

   // Gets the URI with the location and ? stripped off - i.e. the GET values
   //   See: http://www.w3.org/Protocols/HTTP/1.0/spec.html#Request-URI
   const COLstring& queryString() const;
   void setQueryString(const COLstring& Value);
   
   // Gets the request method - typically GET, POST, HEAD or other....
   //   See: http://www.w3.org/Protocols/HTTP/1.0/spec.html#Method
   const COLstring& method() const;
   void setMethod(const COLstring& pMethod);

   // Returns the location pointed to by the URI with the GET values stripped off.
   const COLstring& location() const;
   void setLocation(const COLstring& Location);   
private:
   HTTPheader m_Header;
   COLstring m_QueryString;
   COLstring m_Method;
   COLstring m_Location; // file location
};


// Formats the request, not including the entity, as a string
// formatted according to the HTTP 1.1 specification.
void HTTPgenerateRequest(COLostream& Stream, const HTTPrequest& Request);

COLostream& operator<<(COLostream& Stream, const HTTPrequest& Request);


#endif // end of defensive include
