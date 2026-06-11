#ifndef __HTTP_BASIC_AUTHENT_H__
#define __HTTP_BASIC_AUTHENT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPbasicAuthent
//
// Description:
//
// HTTPbasicAuthent - routines to do basic authentication
//
// Support HTTP 1.0 compliant Basic Authentication
//    http://www.w3.org/Protocols/HTTP/1.0/draft-ietf-http-spec.html#BasicAA
//    http://www.w3.org/Protocols/HTTP/1.0/draft-ietf-http-spec.html#WWW-Authenticate
//
// Author: Eliot Muir
// Date:   Monday, January 14th, 2013 @ 02:35:39 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class COLstring;
class HTTPheader;

// Returns true if basic authentication works.  Username and password will be in the result.
bool HTTPparseBasicAuthent(const HTTPheader& Headers, COLstring* pUserName, COLstring* pPassword);

void HTTPsetBasicAuthent(HTTPheader* pHeaders, const COLstring& UserName, const COLstring& Password);

#endif // end of defensive include
