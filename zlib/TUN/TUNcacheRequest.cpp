// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNcacheRequest
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Wednesday 02 August 2023 - 03:12PM
// ---------------------------------------------------------------------------

#include <TUN/TUNcacheRequest.h>
#include <TUN/TUNstateClient.h>


#include <SCK/SCKloop.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

TUNrequestInProgress::TUNrequestInProgress(COLstring* pData, int TimerId) : m_pBinaryRequestData(pData), m_AckTimer(TimerId)
{
   COL_METHOD(TUNrequestInProgress::TUNrequestInProgress);
}

TUNrequestInProgress::~TUNrequestInProgress(){
   COL_METHOD(TUNrequestInProgress::~TUNrequestInProgress);
   delete m_pBinaryRequestData;
}


void TUNcacheRequest(COLstring* pRequest, int AckTimer, const COLstring& ConnectionId, TUNstateClient* pClient){
   COL_FUNCTION(TUNcacheRequest);
   COL_TRC("Caching request with connection id: " + ConnectionId);
   if (pClient->m_RequestsInProgress.count(ConnectionId)) { COL_TRC("CONNECTION ID ALREADY EXISTS"); }
   pClient->m_RequestsInProgress.add(ConnectionId, new TUNrequestInProgress(pRequest, AckTimer));
   COL_TRC("LOADED " + ConnectionId);
}

void TUNunloadRequest(const COLstring& ConnectionId, TUNstateClient* pClient){
   COL_FUNCTION(TUNunloadRequest);
   if (!pClient->m_RequestsInProgress.count(ConnectionId)) {
      // Could just be a router/protocol request. Probably ok to ignore and return. 
      COL_TRC("Request with connection id: " << ConnectionId << " not found in cache");
      return;
   }
   COL_TRC("Deleting cached request with id: " + ConnectionId);
   pClient->m_RequestsInProgress.erase(ConnectionId);
   COL_TRC("UNLOADED " + ConnectionId);
}
