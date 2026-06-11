//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKnameResolver
//
// Description:
//
// Implementation 
//
// Author: Eliot Muir
// Date:   Wed 26/01/2005 
//---------------------------------------------------------------------------

#include "SCKnameResolver.h"
#include "SCKutils.h"
#include "SCKaddress.h"
#include "SCKloop.h"

#include <COL/COLutils.h>
#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// This is returned on the SCKloop thread.
void SCKnameResolver::onDone(COLauto<SCKnameRequest> pNameRequest){
   COL_FUNCTION(SCKnameResolver::onDone);
   m_CountOfNameRequest--;
   COL_VAR(m_CountOfNameRequest);
   COL_TRC(pNameRequest->m_Address.hostName() << " name resolved.");
   COLhashmap<int, COLauto< COLclosure1<const SCKaddress&> > >::iterator i = m_NameResolutionMap.find(pNameRequest->m_Id);
   if (i) {
      COL_TRC("Sending the results to the callee");
      COLauto< COLclosure1<const SCKaddress&> >pCallback = i->second;
      COL_VAR(pCallback);
      m_NameResolutionMap.removeKey(pNameRequest->m_Id);
      pCallback->run(pNameRequest->m_Address);  // note that this could throw.
   } else {
      COL_ERR("Name request was cancelled.");
   }
   if (m_pCallback){
      COL_TRC("Call main name resolution callback.");
      try{
         m_pCallback->run();
      } catch(const COLerror& Error){
         COL_ERR("ERROR - MAJOR PROBLEM - ON NAME RESOLUTION HANDLER THREW EXCEPTION" << newline << Error);
      }
   }
}
void SCKdoLookup(SCKnameResolver* pNameResolver, COLauto<SCKnameRequest> pRequest){
   COL_FUNCTION(SCKdoLookup);
   COL_VAR2(pRequest, pRequest->m_Address.hostName());
   int SavePort = pRequest->m_Address.port();
   COLstring SaveHostName = pRequest->m_Address.hostName();
   // Uncomment the below to simulate slow performing networks
   //COL_ERR("Sleeping for a bit"); COLsleep(5000);
   bool Success = SCKhostResolve(pRequest->m_Address.hostName(), &pRequest->m_Address);
   pRequest->m_Address.setPort(SavePort);
   pRequest->m_Address.setHostName(SaveHostName);
   if (!Success){
      COL_TRC("Failed to resolve " << pRequest->m_Address.hostName() << ", error: ");
   } else {
      COL_TRC("Resolved " << pRequest->m_Address.hostName() << " to address " << pRequest->m_Address.toString());
   }
   pNameResolver->loop()->post(COLnewClosure0(pNameResolver, &SCKnameResolver::onDone, pRequest));
}

SCKnameRequest::SCKnameRequest(const SCKaddress& Host, int Id) :  m_Id(Id), m_Address(Host) {
   COL_METHOD(SCKnameRequest::SCKnameRequest);
   COL_VAR2(m_Address, Id);
}

SCKnameRequest::~SCKnameRequest(){
   COL_METHOD(SCKnameRequest::~SCKnameRequest);
}

// Eliot one optimization which could be done would be to notice if there are outstanding name resolutions pending on a given
// hostname and piggy back off the result of the thread that has been launched.  Something to consider for the future.
void SCKasyncNameResolve(SCKnameResolver* pNameResolver, COLthreadPool* pThreadPool, const SCKaddress& HostName, int Id){
   COL_FUNCTION(SCKasyncNameResolve);
   COLauto<SCKnameRequest> pRequest = new SCKnameRequest(HostName, Id);
   pThreadPool->scheduleTask(COLnewClosure0(&SCKdoLookup, pNameResolver, pRequest));
}

SCKnameResolver::SCKnameResolver(COLthreadPool* pThreadPool, SCKloop* pLoop)
: m_pThreadPool(pThreadPool), m_CountOfNameRequest(0), m_NextNameResolutionId(0), m_pLoop(pLoop), m_pCallback(0){
   COL_METHOD(SCKnameResolver::SCKnameResolver);
}

SCKnameResolver::~SCKnameResolver(){
   COL_METHOD(SCKnameResolver::~SCKnameResolver);
   shutdown();
}

int SCKnameResolver::resolveStart(const SCKaddress& Host, COLclosure1<const SCKaddress&>* pCallback){
   COL_METHOD(SCKnameResolver::resolveStart);
   m_CountOfNameRequest++;
   COL_VAR(m_CountOfNameRequest);
   m_NextNameResolutionId++;
   COL_VAR2(m_NextNameResolutionId, Host);
   m_NameResolutionMap.add(m_NextNameResolutionId, pCallback);
   COL_VAR(m_NameResolutionMap.size());
   SCKasyncNameResolve(this, m_pThreadPool, Host, m_NextNameResolutionId);
   return m_NextNameResolutionId;
}

void SCKnameResolver::setOnRequestReturn(COLclosure0* pCallback){
   COL_METHOD(SCKnameResolver::setOnRequestReturn);
   m_pCallback = pCallback;
}

int SCKnameResolver::countOfNameRequest() const{
   return m_CountOfNameRequest;
}

void SCKnameResolver::resolveCancel(int Id){
   COL_METHOD(SCKnameResolver::resolveCancel);
   m_NameResolutionMap.removeKey(Id);
}

void SCKnameResolver::shutdown(){
   COL_METHOD(SCKnameResolver::shutdown);
   COL_VAR(m_NameResolutionMap.size());
   m_NameResolutionMap.clear();  // we don't want to call anyone back at this point.
   // TODO - we have a potential problem of what to do with unresolved name requests.
   //while (m_CountOfNameRequest > 0){
   //   COL_TRC("Still have " << m_NameResolutionMap.size() << " name resolution requests remaining.");
   //   checkNameResolution();
   //   if (m_CountOfNameRequest > 0){
   //      COL_TRC("Sleep a little while we wait for name requests to be returned.");
   //      COLsleep(10);
   //   }
   //}
}
