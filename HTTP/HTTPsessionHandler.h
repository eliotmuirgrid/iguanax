#ifndef __HTTP_SESSION_HANDLER_H__
#define __HTTP_SESSION_HANDLER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPsessionHandler
//
// Description:
//
// This HTTPsessionHandler class is intended to be a flexible way
// to handle server side session information in a loosely coupled manner.
// 
// I looked at the TEM framework and basically extracted the ideas from there but
// decomposed them into smaller classes with more tightly defined
// functionality.
//
// Author: Eliot Muir
// Date:   Sunday, January 14th, 2007 @ 05:18:49 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class HTTPsessionHandlerPrivate;
class HTTPsession;
class HTTPrequest;

class HTTPsessionHandler 
{
public:
   HTTPsessionHandler();
   // This is non virtual because we are assuming no classes will inherit from this class
   ~HTTPsessionHandler();

   HTTPsession* session(const COLstring& Name);

   HTTPsession& createSession();
   bool destroySession(const COLstring& Name);

   // It's up the the application to explicitly invoke this function.  This doesn't happen implicitly
   // by invoking any other function.  This is a deliberate part of the design of this class.  See ticket #5138
   void removeExpiredSessions();
private:
   HTTPsessionHandlerPrivate* pMember;
   HTTPsessionHandler(const HTTPsessionHandler& Orig); // not allowed
   HTTPsessionHandler& operator=(const HTTPsessionHandler& Orig); // not allowed
};

// Returns null if no HTTPsession exists.
HTTPsession* HTTPsessionHandlerFindSession(HTTPsessionHandler& SessionHandler, const HTTPrequest& Request);
const HTTPsession* HTTPsessionHandlerFindSession(const HTTPsessionHandler& SessionHandler, const HTTPrequest& Request);

#endif // end of defensive include
