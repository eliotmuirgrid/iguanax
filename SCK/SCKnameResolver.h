#ifndef __SCK_NAME_RESOLVER_H__
#define __SCK_NAME_RESOLVER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKnameResolver
//
// Description:
//
// This is an asynchronous SCK name resolver class.  It needs a COLthreadPool
// to work.  I gutted the original implementation.
//
// Author: Eliot Muir
// Date:   Wed 26/01/2005 
//---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <COL/COLhashmap.h>
#include <COL/COLclosure.h>

#include "SCKaddress.h"

class COLstring;

class SCKaddress;
class COLthreadPool;
class SCKsocket;
class SCKloop;

class SCKnameRequest{
public:
   SCKnameRequest(const SCKaddress& Host, int Id);
   ~SCKnameRequest();

   SCKaddress m_Address;
   int m_Id;
};

class SCKnameResolver{
public:
   SCKnameResolver(COLthreadPool* pThreadPool, SCKloop* pLoop);
   ~SCKnameResolver();

   // Public interface for clients.
   // WARNING - thread will take ownership of pAddress when this method is run.  It returns an ID for the name request.
   int resolveStart(const SCKaddress& HostName, COLclosure1<const SCKaddress&>* pCallback);
   void resolveCancel(int Id);
   
   //TODO - Need a more elegant way of shutting down the SCKloop and this when there are outstanding threads.
   void setOnRequestReturn(COLclosure0* pCallback);
   int countOfNameRequest() const;

   // Called when we are done with a name request.
   void onDone(COLauto<SCKnameRequest> pRequest);
   void shutdown();

   SCKloop* loop() { return m_pLoop; }
private:
   COLhashmap<int, COLauto<COLclosure1<const SCKaddress&> > > m_NameResolutionMap;
   int m_NextNameResolutionId;
   int m_CountOfNameRequest;
   COLthreadPool* m_pThreadPool;
   SCKloop* m_pLoop;

   COLauto<COLclosure0>  m_pCallback; // Executed when all requests have been completed
   
   SCKnameResolver(const SCKnameResolver& Orig);  // not allowed
   SCKnameResolver& operator=(const SCKnameResolver& Orig); // not allowed
};

#endif // end of defensive include
