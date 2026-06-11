//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: WEBserver
//
// Author: Eliot Muir
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------

#include <WEB/WEBserver.h>
#include <WEB/WEBhandle.h>
#include <WEB/WEBredirect.h>
#include <WEB/WEBrequestData.h>
#include <WEB/WEBresponse.h>
#include <WEB/WEBserverPurge.h>
#include <WEB/WEBserverDispatch.h>
#include <WEB/WEBserverParse.h>

#include <SCK/SCKutils.h>
#include <SCK/SCKerror.h>

#include <SCK/SCKaddress.h>
#include <SCKS/SCKSsslConnection.h>

#include <SCK/SCKconnectionWrite.h>

#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

WEBconnection::WEBconnection(const SCKaddress& Address)
: m_Address(Address), m_IsWs(false) {
   COL_METHOD(WEBconnection::WEBconnection);
   time(&m_LastActive);
}

WEBconnection::~WEBconnection(){
   COL_METHOD(WEBconnection::~WEBconnection);
}

WEBserver::WEBserver(SCKloop& Loop) : m_pLoop(&Loop), m_RequireAuthentication(true){
   COL_METHOD(WEBserver::WEBserver);
   m_SslContext.init();
}

WEBserver::~WEBserver(){
   COL_METHOD(WEBserver::~WEBserver);
}

int WEBserver::lastError() const {
   COL_METHOD(WEBserver::lastError);
   return m_pListener->lastError();
}

void WEBeventConnect(WEBserver* pServer, SCKconnection* pConnection){
   COL_FUNCTION(WEBeventConnect);
   pConnection->m_pOnData =COLnewClosure2(pServer, &WEBserver::onData);
   pConnection->m_pOnClose=COLnewClosure1(pServer, &WEBserver::onClose);
}

void WEBserver::onAccept(){
   COL_METHOD(WEBserver::onAccept);
   SCKaddress Address;
   int Socket = m_pListener->accept(&Address);
   SCKconnection* pConnection;
   if (!m_pCtx){
      pConnection = new SCKconnection(*m_pLoop, Socket);
   } else {
      SCKSsslConnection* pSslConnection = new SCKSsslConnection(m_pCtx, *m_pLoop, Socket);
      pSslConnection->setOnRawData(COLnewClosure2(this, &WEBserver::onRawData));
      pConnection = pSslConnection;
   }
   WEBeventConnect(this, pConnection);
   m_Connections.add(pConnection->m_Id, new WEBconnection(Address));
}

void WEBserver::onRawData(const COLstring* pData, SCKconnection* pConnection){
   COL_METHOD(WEBserver::onRawData);
   COL_DUMP("Raw buffer", pData->data(), pData->size());
   if (!WEBisHttpRequest(*pData)) { return; }
   COL_TRC("Received an HTTP request during SSL handshake. Serving the HTTPS redirect page");
   WEBserveRedirectHtml(pConnection, *pData);
   // pConnection is closed shortly after we return here. We may not be able
   // to send the HTML response in some cases.
}

// TODO - we have a double ID problem here.
void WEBserver::onData(COLuint64 ConnectionId, COLstring* pData){
   COL_METHOD(WEBserver::onData);
   WEBconnection* pConnection = m_Connections[ConnectionId];
   time(&(pConnection->m_LastActive));    // update last active time
   COL_VAR(pConnection->m_LastActive);
   int FindHeaderEnd;
   do{
      COL_VAR(*pData);
      COL_DUMP("Buffer:", pData->c_str(), pData->size());
      if (pData->find(&FindHeaderEnd,"\r\n\r\n")){
         if (WEBserverParse(pData, &pConnection->m_RequestData)){
            pConnection->m_RequestData.ConnectionId = ConnectionId;   // TODO - we can shift these to startup.
            pConnection->m_RequestData.IsSecure = m_pCtx !=NULL;
            COL_VAR(pConnection->m_RequestData);
            const bool IsWsUpgrade = pConnection->m_RequestData.Headers.find("upgrade") != pConnection->m_RequestData.Headers.end();
            if(IsWsUpgrade && pConnection->m_RequestData.Headers["upgrade"] == "websocket" && m_pWebSocketUpgradeHandler.get()) {
               COL_TRC("WebSocket upgrade detected - invoking WebSocket handler before HTTP dispatch");
               isWebSocket(ConnectionId);
               m_pWebSocketUpgradeHandler->run(pConnection->m_RequestData);
            } else {
               WEBserverDispatch(pConnection->m_RequestData, &m_SessionMap, m_pLoop, m_RequireAuthentication, m_WorkerThreadHandlers, m_MainThreadHandlers, this);
            }
            if (pData->size() == 0){
               COL_TRC("No more data in buffer.  Returning");
               return;
            }
         } else {
            COL_TRC("Haven't got a complete body yet.");
            return;
         }
      } else {
         COL_TRC("Haven't got a complete header yet. Buffer size: " << pData->size());
         COL_DUMP("Partial data", pData->c_str(), pData->size());
         return;
      }
   } while(true);
   // TODO - deal with Expect header from Curl.
   //pConnection->m_pConnection->write("HTTP/1.1 100 Continue\r\n\r\n");
 }

void WEBserver::onClose(COLuint64 ConnectionId){
   COL_METHOD(WEBserver::onClose);
   COL_TRC("Closing = " << ConnectionId);
   delete m_Connections[ConnectionId];
   m_Connections.removeKey(ConnectionId);
   //if (m_pConnectionCloseCallback){
   //   m_pConnectionCloseCallback->run(ConnectionId);
  // }
}

const SCKaddress& WEBserver::address(COLuint64 Id) const{
   COL_METHOD(WEBserver::address);
   return m_Connections[Id]->m_Address;
}

bool WEBserver::write(COLuint64 ConnectionId, const char* pData, int Size){
   auto i =  m_Connections.find(ConnectionId);   // This is redundant?
   if (!i){
      COL_TRC("Socket connection has gone away.");
      return false;
   }
   SCKconnectionWrite(m_pLoop, ConnectionId, pData, Size);
   return true;
}

COLauto< COLclosure0 > s_pResponseCompleteCallback;

void WEBserverSetResponseCompleteCallback(COLclosure0* pCallback){
   s_pResponseCompleteCallback = pCallback;
}

void WEBserverRunResponseCompleteCallback(){
   s_pResponseCompleteCallback->run();
}

void WEBserverClearHandlers(WEBserver* pServer){
   COL_FUNCTION(WEBserverClearHandlers);
   pServer->m_WorkerThreadHandlers.clear();
   pServer->m_MainThreadHandlers.clear();
}

void WEBserver::setOnWebSocketUpgrade(COLauto<COLclosure1<const WEBrequestData&>> pHandler) {
   COL_METHOD(WEBserver::setOnWebSocketUpgrade);
   m_pWebSocketUpgradeHandler = pHandler;
}

void WEBserver::setOnRestartGrid(COLauto<COLclosure0> pHandler) {
   COL_METHOD(WEBserver::setOnRestartGrid);
   m_pOnRestartGrid = pHandler;
}

void WEBserver::isWebSocket(const COLuint64 ConnectionId) {
   COL_METHOD(WEBserver::isWebSocket);
   WEBconnection* pConnection = m_Connections[ConnectionId];
   if(pConnection) {
      pConnection->m_IsWs = true;
      COL_TRC("Marked connection " << ConnectionId << " as WebSocket - will not be subject to idle timeout");
   }
}
