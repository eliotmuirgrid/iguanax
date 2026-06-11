#ifndef __HTTP_RESPONSE_H__
#define __HTTP_RESPONSE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPresponse
//
// Description:
//
// HTTPresponse - represents an HTTP response.
//
// Author: Rob Moyse (ported by Eliot) 
// Date:   Friday, December 8th, 2006 @ 02:11:52 PM
//---------------------------------------------------------------------------

class HTTPheader;
class HTTPresponsePrivate;

class HTTPresponse{
public:
   HTTPresponse();
   ~HTTPresponse();

   // Set status code for the response instead of default success code.
   //    see http://www.w3.org/Protocols/HTTP/1.0/spec.html#Status-Code
   void setCode(int StatusCode);
   int code() const;   

   // Sets the message's entity.  (Content-Type: MIME header)
   //    see http://www.faqs.org/rfcs/rfc2616.html (7)
   // For text/*: if charset is not specified, utf-8 is supplied.
   void setEntityType(const COLstring& EntityType);

   void setNoCaching();

   // access header information
   const HTTPheader& header() const;
   HTTPheader& header();

   enum HTTPstatusCode
   {
      HTTP_STATUS_OK= 200,
      HTTP_STATUS_CREATED = 201,
      HTTP_STATUS_ACCEPTED = 202,
      HTTP_STATUS_NO_CONTENT = 204,
      HTTP_STATUS_MOVED_PERM = 301,
      HTTP_STATUS_FOUND = 302,
      HTTP_STATUS_NOT_MODIFIED = 304,
      HTTP_STATUS_BAD_REQUEST = 400,
      HTTP_STATUS_UNAUTHORIZED = 401,
      HTTP_STATUS_FORBIDDEN = 403,
      HTTP_STATUS_NOT_FOUND = 404,
      HTTP_STATUS_INTERNAL_ERROR = 500,
      HTTP_STATUS_NOT_IMPLEMENTED = 501,
      HTTP_STATUS_BAD_GATEWAY = 502,
      HTTP_STATUS_SERVICE_UNAVAILABLE = 503
   };
private:
   HTTPresponsePrivate* pMember;
   HTTPresponse( const HTTPresponse& Orig );
   HTTPresponse& operator=( const HTTPresponse& Orig );
};

COLostream& operator<<(COLostream& Stream, const HTTPresponse& Response);

// This sets up the response to have the headers appropriate for the amount of data given.  It is the application's
// responsibility to actually get the headers using generateResponseString, write the out and to write the actual
// body data back to the recipient.
void HTTPsetupEntity(HTTPresponse& Response, COLuint64 LengthOfData, const COLstring& EntityType);

// This is a shortcut method for the above that takes COLstring containing the data.
void HTTPsetupStringEntity(HTTPresponse& Response, const COLstring& Data, const COLstring& EntityType);

// Returns the "Reason Phrase" associated with the
// response message's status code; used in the
// status line of the response.
void HTTPlookupReasonPhrase(int StatusCode, COLostream& Stream);

// Formats the response, not including the entity, as a string
// formatted according to the HTTP 1.1 specification.
void HTTPgenerateResponse(COLostream& Stream, const HTTPresponse& Response, bool AlwaysOutputContentLength = true);


#endif // end of defensive include
