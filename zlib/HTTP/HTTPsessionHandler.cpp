//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPsessionHandler
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Sunday, January 14th, 2007 @ 05:18:49 PM
//
//---------------------------------------------------------------------------
#include "HTTPprecomp.h"
#pragma hdrstop

#include "HTTPsessionHandler.h"
#include "HTTPsession.h"
#include "HTTPcookie.h"

#include <COL/COLhashmap.h>
#include <COL/COLlist.h>
#include <COL/COLauto.h>
#include <COL/COLmath.h>
#include <COL/COLdateTime.h>

class HTTPsessionHandlerPrivate
{
public:
   COLhashmap<COLstring, COLauto<HTTPsession> > SessionTable;
};


COLstring HTTPfindUniqueSessionName(COLhashmap<COLstring, COLauto<HTTPsession> >& SessionTable)
{
   COLstring UniqueName;
   COLostream NameStream(UniqueName);
   do
   {
      // TODO this is not a cryptographically strong random number generator, but without HTTPS nothing
      // is that secure.
      NameStream << COLmath::randomNumber(RAND_MAX-1) << COLmath::randomNumber(RAND_MAX-1);
      if (!SessionTable.find(UniqueName))
      {
         return UniqueName;
      }
      UniqueName.clear();
   }  while(true);
}

HTTPsessionHandler::HTTPsessionHandler()
{
   pMember = new HTTPsessionHandlerPrivate;
   COLmath::resetRandomGenerator();
}

HTTPsessionHandler::~HTTPsessionHandler()
{
   delete pMember;
}

HTTPsession* HTTPsessionHandler::session(const COLstring& Key)
{
   COLhashmapPlace SessionIndex = pMember->SessionTable.find(Key);
   if (SessionIndex)
   {
      return pMember->SessionTable[SessionIndex].get();
   }
   else
   {
      return NULL;
   }
}

HTTPsession& HTTPsessionHandler::createSession()
{
   COLstring UniqueSessionId = HTTPfindUniqueSessionName(pMember->SessionTable);
   COLhashmapPlace NewIndex = pMember->SessionTable.add(UniqueSessionId, new HTTPsession(UniqueSessionId));
   return *pMember->SessionTable[NewIndex];
}

bool HTTPsessionHandler::destroySession(const COLstring& SessionName)
{
   COLhashmapPlace SessionIndex = pMember->SessionTable.find(SessionName);
   if (SessionIndex)
   {
      pMember->SessionTable.remove(SessionIndex);
      return true;
   }
   else
   {
      return false;
   }
}

// TODO if we ever have the need to start iterating through sessions this should
// be turned into a non member function.  I haven't tried to make this that efficient, for
// web guis it probably doesn't need to be.
void HTTPsessionHandler::removeExpiredSessions()
{
   COLlist<COLhashmapPlace> RemoveList;
   COLdateTime Now = COLdateTime::currentTime();
   for (COLhashmapPlace SessionIndex = pMember->SessionTable.first(); 
        SessionIndex != NULL;
        SessionIndex = pMember->SessionTable.next(SessionIndex))
   {
      if (pMember->SessionTable[SessionIndex]->expiryTime() < Now)
      {
         RemoveList.add(SessionIndex);
      }
   }
   for (COLlistPlace RemoveIndex=RemoveList.first(); RemoveIndex != NULL; RemoveIndex = RemoveList.next(RemoveIndex))
   {
      pMember->SessionTable.remove(RemoveList[RemoveIndex]);
   }
}

// Returns null if no HTTPsession exists.
HTTPsession* HTTPsessionHandlerFindSession(HTTPsessionHandler& SessionHandler, const HTTPrequest& Request)
{
   // brute force inefficient implementation that could be optimized later.
   COLhashmap<COLstring, COLstring> CookieJar; 
   HTTPextractCookies(Request, CookieJar);

   COLhashmapPlace CookieIndex = CookieJar.find("SessionId");
   if (CookieIndex)
   {         
       return SessionHandler.session(CookieJar[CookieIndex]);     
   }
   else
   {
      return NULL;
   }
}

const HTTPsession* HTTPsessionHandlerFindSession(const HTTPsessionHandler& SessionHandler, const HTTPrequest& Request)
{
   return (const HTTPsession*)HTTPsessionHandlerFindSession((HTTPsessionHandler&)SessionHandler, Request);
}
