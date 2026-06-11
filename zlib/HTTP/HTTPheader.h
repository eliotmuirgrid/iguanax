#ifndef __HTTP_HEADER_H__
#define __HTTP_HEADER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPheader
//
// Description:
//
// HTTPheader class - this is the parent class for HTTPrequest and HTTPresponse
// since HTTP requests and responses have very similar structures.
//
// Author: Eliot Muir
// Date:   Monday, December 11th, 2006 @ 12:05:17 PM
//
//---------------------------------------------------------------------------

#include <COL/COLarray.h>

// General comment about the HTTP library.  It's not as optimal as it could be...
// The issue here is that using COLstring's imposes quite a lot of overhead for a
// parser since many small malloc calls have to be made.  It's faster to have a big
// block of memory and parse it by recording the position and end of each token being
// parsed - not even null terminating each token.  This of course results in quite a
// different interface to the entire library - I'm probably not going to do it now but
// if we did go down that path this library would change...Eliot

// Allow default copy constructor.
class HTTPheaderPair{
public:
   COLstring Key;
   COLstring Value;
};

class HTTPheader 
{
public:
   HTTPheader();
   // Default copy constructor and assignment operator should work since it's plain old data.
   ~HTTPheader();

   void addHeader(const COLstring& Key, const COLstring& Value);
   void addHeader(const char* pHeader, COLuint32 SizeOfHeader, const char* pValue, COLuint32 SizeOfValue);
   void removeHeader(COLuint32 HeaderIndex);
   void replaceHeader(COLuint32 HeaderIndex, const COLstring& Key, const COLstring& Value);

   COLuint32 countOfHeader() const;
   const COLstring& headerKey(COLuint32 HeaderIndex) const;
   const COLstring& headerValue(COLuint32 HeaderIndex) const;
   
   // returns -1 if the header does not exist.
   COLuint32 findHeader(const COLstring& HeaderName) const;

   // Version of HTTP protocol - defaults to "1.1"
   const COLstring& version() const;
   void setVersion(const COLstring& Version);
private:
   COLarray<HTTPheaderPair> m_HeaderVector;
   COLuint64 m_ContentLength;
   COLstring m_Version;
};

COLostream& operator<<(COLostream& Stream, const HTTPheader& Header);

// Gets the keep-alive value in the header.  Returns false if not present
bool HTTPheaderGetKeepAlive(const HTTPheader& Header);
void HTTPheaderSetKeepAlive(HTTPheader& Header, bool KeepAlive);

// Gets the gzip encoding value in the header, returns false if not present.
bool HTTPheaderGetGzipSupported(const HTTPheader& Header);

// Gets the size of the entity body in bytes. See: http://www.w3.org/Protocols/HTTP/1.0/spec.html#Entity-Body
COLuint64 HTTPheaderGetContentLength(const HTTPheader& Header);
bool HTTPheaderHasContentLength(const HTTPheader& Header);
void HTTPheaderSetContentLength(HTTPheader& Header, COLuint64 ContentLength);


#endif // end of defensive include
