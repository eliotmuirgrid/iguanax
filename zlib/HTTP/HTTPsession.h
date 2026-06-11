#ifndef __HTTP_SESSION_H__
#define __HTTP_SESSION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPsession
//
// Description:
//
// HTTPsession class.  This represents an actual user session.  I've
// broken this original concepts of TEM into smaller classes.  I've also
// gone for a minimalist approach of making each method have a single defined
// purpose with no side effects.  The old implementation would alter the
// expiry time each time a session variable is read.  I prefer to have an
// approach where each method does what is says it does and no more.  The logic
// upstream should be more obvious.
//
// Author: Eliot Muir
// Date:   Thursday, January 18th, 2007 @ 10:28:16 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class HTTPsessionPrivate;
class COLdateTime;
class HTTPresponse;

typedef void* HTTPvariablePlace;

const COLuint32 HTTP_SESSION_EXPIRY = 1200;  // expiry time in seconds

class HTTPsession 
{
public:
   HTTPsession(const COLstring& Name, COLuint32 SessionExpiry=HTTP_SESSION_EXPIRY);
   HTTPsession();
   // This is non virtual because we are assuming no classes will inherit from this class
   ~HTTPsession();

   const COLstring& name() const;
   void setName(const COLstring& Name);

   void setVariable(const COLstring& Name, 
                          const COLstring& Value);

   // returns NULL if the variable does not exist.
   const COLstring* variable(const COLstring& Name) const;   

   HTTPvariablePlace first() const;
   HTTPvariablePlace next(HTTPvariablePlace VariableIndex) const;

   const COLstring& key(HTTPvariablePlace VariableIndex) const;
   const COLstring& value(HTTPvariablePlace VariableIndex) const;

   const COLdateTime& expiryTime() const;
   COLdateTime& expiryTime();

   const COLstring& createTransaction();
   bool closeTransaction(const COLstring& TransactionId);
   bool isTransactionValid(const COLstring& TransactionId) const;

private:
   HTTPsessionPrivate* pMember;
   HTTPsession(const HTTPsession& Orig); // not allowed
   HTTPsession& operator=(const HTTPsession& Orig); // not allowed
};

void HTTPsessionSetCookie(const HTTPsession& Session, HTTPresponse& Response);

// this resets the expiry to a later time... guess this could be a non member function
void HTTPsessionTouch(HTTPsession& Session, COLuint32 ExpiryTimeInSeconds=HTTP_SESSION_EXPIRY);

#endif // end of defensive include
