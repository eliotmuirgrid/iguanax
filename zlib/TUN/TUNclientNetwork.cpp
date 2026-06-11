// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNclientNetwork
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Friday 16 June 2023 - 01:28PM
// ---------------------------------------------------------------------------

#include <TUN/TUNclientNetwork.h>

#include <TUN/TUNstateClient.h>
#include <TUN/TUNkeyLoad.h>
#include <TUN/TUNsendHello.h>
#include <TUN/TUNsetup.h>
#include <TUN/TUNrequest.h>
#include <TUN/TUNrecordProcess.h>
#include <TUN/TUNkeyFingerPrint.h>
#include <TUN/TUNclientLoad.h>
#include <TUN/TUNsendPeerData.h>
#include <TUN/TUNconnectionRefresh.h>

#include <SCK/SCKconnection.h>
#include <SCK/SCKconnectionWrite.h>
#include <SCK/SCKconnectionRead.h>
#include <SCK/SCKloop.h>

#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNonConnect(COLuint64 ConnectionId, SCKconnection* pConnection, TUNstateClient* pClient){
   COL_FUNCTION(TUNonConnect);
   if (pClient->m_pOnRouterConnect.get()){
      pClient->m_pOnRouterConnect->run();
   }
   COLstring Out;
   TUNsendHello(&Out, *pClient);
   pConnection->writeString(Out);
}

void TUNonData(COLuint64 ConnectionId, COLstring* pData, TUNstateClient* pClient){
   COL_FUNCTION(TUNonData);
   TUNrequest Request;
   Request.pIn = pData;
   COLstring Out;
   Request.pOut = &Out;
   COL_DUMP("Data:", Request.pIn->c_str(), Request.pIn->size());
   if (pClient->m_Respond) { 
      TUNrecordProcess(&Request, pClient->m_HandlerTable);
      SCKconnectionWrite(pClient->m_pLoop, ConnectionId, Out);
   } else {
      COL_TRC("Warning - witholding response");
   }
}
void TUNclientNetworkInit(TUNstateClient* pState);

void TUNonClose(COLuint64 ConnectionId, TUNstateClient* pClient){
   COL_FUNCTION(TUNonClose);
   pClient->m_ConnectionId = 0;
   for (auto i = pClient->m_Peers.begin(); i != pClient->m_Peers.end(); i++){
      i->second.m_Authenticated = false;
   }
   if (pClient->m_pOnRouterDisconnect.get()){
      pClient->m_pOnRouterDisconnect->run();
   }
   pClient->m_ReconnectTimerId = pClient->m_pLoop->addTimer(COLnewClosure0(&TUNclientNetworkInit, pClient), 5000);
}

void TUNhookup(SCKconnection* pConnection, TUNstateClient* pClient){
   COL_FUNCTION(TUNhookup);
   pConnection->m_pOnConnect = COLnewClosure1(&TUNonConnect, pConnection, pClient);
   pConnection->m_pOnData    = COLnewClosure2(&TUNonData,                 pClient);
   pConnection->m_pOnClose   = COLnewClosure1(&TUNonClose,                pClient);
}

void TUNresetRouterConnection(TUNstateClient* pState){
   COL_FUNCTION(TUNresetRouterConnection);
   if (!TUNneedRefresh(pState->m_LastActivity)){
      COL_TRC("No need to reset router connection");
      pState->m_ConnectionRefreshTimerId = pState->m_pLoop->addTimer(COLnewClosure0(&TUNresetRouterConnection, pState), TUN_IDLE_CONNECTION_REFRESH);
      return;
   }
   COL_TRC("No activity for over a minute, resetting the router connection.");
   SCKconnectionHardClose(pState->m_pLoop, pState->m_ConnectionId);
}

void TUNclientNetworkInit(TUNstateClient* pState){
   COL_FUNCTION(TUNclientNetworkInit);
   pState->m_pLoop->cancelTimer(pState->m_ConnectionRefreshTimerId); // in case we lose router connection, we call this function every 5s so we need to clean this timer up
   pState->m_ConnectionRefreshTimerId = 0;
   SCKconnection* pConnection = new SCKconnection(*pState->m_pLoop);
   TUNhookup(pConnection, pState);
   
   COLstring CustomRouter = getenv("TUN_ROUTER");
   if (!CustomRouter.is_null()){
      COL_TRC("Warning TUN_ROUTER defined so we are using " << CustomRouter);
      pConnection->connect(CustomRouter, 443);
   } else {
      COL_TRC("TUN_ROUTER not set so use default router");
      pConnection->connect("tunnel.interfaceware.com", 443);
   }

   pState->m_ConnectionId = pConnection->m_Id;
   pState->m_ConnectionRefreshTimerId = pState->m_pLoop->addTimer(COLnewClosure0(&TUNresetRouterConnection, pState), TUN_IDLE_CONNECTION_REFRESH);
}

void TUNclientShutdown(TUNstateClient* pState){
   COL_FUNCTION(TUNclientShutdown);
   COL_VAR2(pState->m_ConnectionId, pState->m_ReconnectTimerId);
   pState->m_pOnRouterDisconnect = NULL;
   if (pState->m_ConnectionId){
      COL_TRC("Closing router connection.");
      SCKconnectionHardClose(pState->m_pLoop, pState->m_ConnectionId);
      pState->m_ConnectionId = 0;
   }
   if (pState->m_ReconnectTimerId){
      COL_TRC("Cancelling router reconnect timer.");
      pState->m_pLoop->cancelTimer(pState->m_ReconnectTimerId);
      pState->m_ReconnectTimerId = 0;
   }
   if (pState->m_ConnectionRefreshTimerId){
      COL_TRC("Cancelling router connection reset timer.");
      pState->m_pLoop->cancelTimer(pState->m_ConnectionRefreshTimerId);
      pState->m_ConnectionRefreshTimerId = 0;
   }
}
