#ifndef __HTTP_COOKIE_H__
#define __HTTP_COOKIE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPcookie
//
// Description:
//
// Routines to create and parse cookies.  The idea here is that only applications
// that need cookies need to have the overhead associated with cookies.  It also keeps
// the core of HTTP simpler and makes it possible for applications to replace parts
// of the HTTP library if they don't meet the requirements of the application.
//
// These routines could easily be pushed out into another library.
//
// Author: Eliot Muir
// Date:   Friday, December 8th, 2006 @ 04:26:33 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLstring.h>

class HTTPresponse;
class HTTPrequest;

class COLdateTime;

template <class KeyType>
struct COLhash;
template <class TKey, class TValue, class CompareType >
class COLhashmap;

typedef COLhashmap<COLstring,COLstring,COLhash<COLstring> > HTTPcookieJar;

// This method can be used to read the cookies out of an HTTP request - note
// that cookies with the same name are not supported - but you could add a routine
// which puts the cookie values into a different container class like a COLmap
void HTTPextractCookies
(
   const HTTPrequest& Request, 
   HTTPcookieJar& CookieJar
);

// Same as the previous method, but pulls info from a response instead of a request
void HTTPextractCookies
(
   const HTTPresponse& Response, 
   HTTPcookieJar& CookieJar
);


// Set a cookie - this used to be a method of HTTPrequest
void HTTPsetCookie
(
   HTTPresponse& Response, 
   const COLstring& Name, 
   const COLstring& Value, 
   const COLdateTime* Expiration,
   const COLstring& Path = "",
   const COLstring& Domain = "",
   bool Secure = false
);

void HTTPsetCookie
(
   HTTPrequest& Request,
   const COLstring& Name,
   const COLstring& Value,
   const COLdateTime* Expiration,
   const COLstring& Path = "",
   const COLstring& Domain = "",
   bool Secure = false
);

// Really like to roll your own HTTP responses?  This routine
// is part of the implementation of HTTPsetCookie
COLstring HTTPmakeCookieString
(
   const COLstring& Name, 
   const COLstring& Value, 
   const COLdateTime* Expiration,
   const COLstring& Path,
   const COLstring& Domain,
   bool Secure);

void HTTPsetCookies
(
   HTTPrequest& Request, 
   const HTTPcookieJar& CookieJar
);

#endif // end of defensive include
