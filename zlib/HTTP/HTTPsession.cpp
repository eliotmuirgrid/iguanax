//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPsession
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday, January 18th, 2007 @ 10:28:16 AM
//
//---------------------------------------------------------------------------
#include "HTTPprecomp.h"
#pragma hdrstop

#include "HTTPsession.h"
#include "HTTPcookie.h"

#include <COL/COLdateTime.h>
#include <COL/COLhashmap.h>
#include <COL/COLmath.h>

class HTTPsessionPrivate
{
public:
   HTTPsessionPrivate()
      : LastTransactionId(0)
   {
      ExpiryTime = COLdateTime::currentTime() 
                   + COLdateTimeSpan(0, 0, 0, HTTP_SESSION_EXPIRY);      
   }

   HTTPsessionPrivate(const COLstring& iName, COLuint32 SessionExpiry)
      : Name(iName), LastTransactionId(0)
   {
      ExpiryTime = COLdateTime::currentTime() 
                   + COLdateTimeSpan(0, 0, 0, SessionExpiry);
   }

   COLdateTime ExpiryTime;  
   COLhashmap<COLstring,COLstring> VariableTable;   
   COLstring Name;

   COLuint32 LastTransactionId;
   COLhashmap<COLstring,bool> TransactionTable;
};

HTTPsession::HTTPsession(const COLstring& Name, COLuint32 SessionExpiry)
{
   pMember = new HTTPsessionPrivate(Name, SessionExpiry);
}

HTTPsession::HTTPsession()
{
   pMember = new HTTPsessionPrivate;
}   

HTTPsession::~HTTPsession()
{
   delete pMember;
}

const COLstring& HTTPsession::name() const
{
   return pMember->Name;
}

void HTTPsession::setName(const COLstring& NewName)
{
   pMember->Name = NewName;
}

void HTTPsession::setVariable
(
   const COLstring& Name, 
   const COLstring& Value
)
{
   pMember->VariableTable[Name] = Value;
}

// returns NULL if the variable does not exist.
const COLstring* HTTPsession::variable(const COLstring& Name) const
{
   COLhashmapPlace VariableIndex = pMember->VariableTable.find(Name);
   if (VariableIndex)
   {
      return &pMember->VariableTable[Name];
   }
   else
   {
      return NULL;
   }
}

HTTPvariablePlace HTTPsession::first() const
{
   return pMember->VariableTable.first();
}

HTTPvariablePlace HTTPsession::next(HTTPvariablePlace VariableIndex) const
{
   return pMember->VariableTable.next((COLhashmapPlace)VariableIndex);
}

const COLstring& HTTPsession::key(HTTPvariablePlace VariableIndex) const
{
   return pMember->VariableTable[(COLhashmapPlace)VariableIndex];
}

const COLstring& HTTPsession::value(HTTPvariablePlace VariableIndex) const
{
   return pMember->VariableTable[(COLhashmapPlace)VariableIndex];
}

const COLdateTime& HTTPsession::expiryTime() const
{
   return pMember->ExpiryTime;
}

COLdateTime& HTTPsession::expiryTime()
{
   return pMember->ExpiryTime;
}

const COLstring& HTTPsession::createTransaction()
{
   COLstring TransactionId = pMember->Name;
   COLostream TransactionStream(TransactionId);
   TransactionStream << pMember->LastTransactionId++;
   COLhashmapPlace Location = pMember->TransactionTable.add(TransactionId, true);
   COLPOSTCONDITION(Location != NULL);
   return pMember->TransactionTable.key(Location);
}

bool HTTPsession::closeTransaction(const COLstring& TransactionId)
{
   COLhashmapPlace TableIndex = pMember->TransactionTable.find(TransactionId);
   if(TableIndex)
   {
      pMember->TransactionTable.remove(TableIndex);  
      return true;
   }
   return false;
}

bool HTTPsession::isTransactionValid(const COLstring& TransactionId) const
{
   COLhashmapPlace TableIndex = pMember->TransactionTable.find(TransactionId);
   if(TableIndex)
   {
      return pMember->TransactionTable[TableIndex];
   }
   return false;
}

void HTTPsessionTouch(HTTPsession& Session, COLuint32 ExpiryTimeInSeconds)
{
   Session.expiryTime() = COLdateTime::currentTime() + COLdateTimeSpan(0,0,0,ExpiryTimeInSeconds);
}

void HTTPsessionSetCookie(const HTTPsession& Session, HTTPresponse& Response)
{
   HTTPsetCookie(Response, "SessionId", Session.name(), NULL);
}


