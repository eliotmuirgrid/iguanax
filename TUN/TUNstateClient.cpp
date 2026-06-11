// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNstateClient
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 14 June 2023 - 03:13PM
// ---------------------------------------------------------------------------

#include <TUN/TUNstateClient.h>
#include <TUN/TUNcacheRequest.h>
#include <TUN/TUNkeyFingerPrint.h>
#include <TUN/TUNsetup.h>

#include <SCK/SCKloop.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void TUNcancelPendingRequestTimers(TUNstateClient* pClient){
   COL_FUNCTION(TUNcancelPendingRequestTimers);
   for (auto i : pClient->m_RequestsInProgress){
      pClient->m_pLoop->cancelTimer(i->second->m_AckTimer);
   }
}

static void TUNclearPendingRequests(TUNstateClient* pClient){
   COL_FUNCTION(TUNclearPendingRequests);
   TUNcancelPendingRequestTimers(pClient);
   for (auto i : pClient->m_RequestsInProgress){
      delete i->second;
   }
}

TUNstateClient::TUNstateClient() : m_ConnectionId(0), m_ReconnectTimerId(0), m_ConnectionRefreshTimerId(0), m_LastActivity(time(NULL)){
   COL_METHOD(TUNstateClient::TUNstateClient);
   TUNsetupClient(&m_HandlerTable, this);
}

TUNstateClient::~TUNstateClient(){
   COL_METHOD(TUNstateClient::~TUNstateClient);
   TUNclearPendingRequests(this);
}

COLostream& operator<<(COLostream& Stream, const TUNstateClient& Client){
   Stream << "Client" << newline << Client.m_Name << " - " << Client.m_Peers.size() << " peers";
   Stream << indent;
   for (auto i= Client.m_Peers.begin(); i != Client.m_Peers.end(); i++){
      Stream << newline << i->first << " - " << i->second.m_Authenticated;
   }
   Stream << unindent;
   Stream << newline << "Key Map:";
   Stream << indent;
   for (auto i= Client.m_KeyToPeerMap.begin(); i != Client.m_KeyToPeerMap.end(); i++){
      Stream << newline << i->first << " --> " << i->second;
   }
   Stream << unindent;
   return Stream;
}
