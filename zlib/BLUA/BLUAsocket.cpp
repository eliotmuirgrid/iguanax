//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAinstanceSocket
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Friday May 14 2021
//---------------------------------------------------------------------------
#include "BLUAsocket.h"

#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAkill.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <LNET/LNETtcpAsync.h>
#include <LUA/LUAawait.h>
#include <LUA/LUAutils.h>
#include <SCK/SCKerror.h>
#include <SCK/SCKlistener.h>
#include <SCK/SCKloop.h>
#include <SCKS/SCKSsslConnection.h>
#include <SCKS/SCKSsslContext.h>
COL_LOG_MODULE;

BLUAsocketSslParams::BLUAsocketSslParams(const COLstring& PrivateKey, const COLstring& Certificate, bool VerifyPeer, const COLstring& CAfile)
: m_PrivateKey(PrivateKey), m_Cert(Certificate), m_VerifyPeer(VerifyPeer), m_CAfile(CAfile) {
   // COL_METHOD(BLUAsocketSslParams::BLUAsocketSslParams);
}

static BLUAtask BLUAsocketOnAcceptTask(int ConnectionId, const COLstring& Address) {
   COL_FUNCTION(BLUAsocketOnAcceptTask);
   BLUAtask Task;
   Task.Id = BLUA_ON_ACCEPT;
   Task.Payload["address"] = Address;
   Task.Payload["conn_id"] = ConnectionId;
   return Task;
}

// for deprecated socket.* API
static int BLUAdeprecatedOnAccept(lua_State* L, int ConnectionId, const COLstring& Address) {
   lua_getglobal(L, "socket");
   lua_pushstring(L, "onAccept");
   lua_gettable(L, -2);
   if (!lua_isfunction(L, -1)) {
      LUApushString(L, "The 'socket.onAccept' callback function is not set.");
      return BLUA_ERROR_COMPILE;
   }
   LUApushString (L, Address);
   lua_pushnumber(L, ConnectionId);
   return lua_pcall(L, 2, 0, 0);
}

// LUA THREAD
void BLUAhandleSocketOnAccept(lua_State*& L, const COLvar& Payload, int* pResult) {
   COL_FUNCTION(BLUAhandleSocketOnAccept);
   COLASSERT(L);
   COLstring Address = COLvarRequiredString(Payload, "address");
   int ConnectionId  = COLvarRequiredInt   (Payload, "conn_id");
   if (!LNETusingTcpAsyncListen(L)) {
      // deprecated socket.* API
      *pResult = BLUAdeprecatedOnAccept(L, ConnectionId, Address);
      return;
   }
   // new net.tcpAsync.* API
   *pResult = LNETonAccept(L, ConnectionId, Address);
}

static BLUAtask BLUAsocketOnConnectTask(int ConnectionId) {
   COL_FUNCTION(BLUAsocketOnConnectTask);
   BLUAtask Task;
   Task.Id = BLUA_ON_CONNECT;
   Task.Payload["conn_id"] = ConnectionId;
   return Task;
}

// for deprecated socket.* API
static int BLUAdeprecatedOnConnect(lua_State* L, int ConnectionId) {
   lua_getglobal(L, "socket");
   lua_pushstring(L, "onConnect");
   lua_gettable(L, -2);
   if (!lua_isfunction(L, -1)) {
      LUApushString(L, "The 'socket.onConnect' callback function is not set.");
      return BLUA_ERROR_COMPILE;
   }
   lua_pushnumber(L, ConnectionId);
   return lua_pcall(L, 1, 0, 0);
}

// LUA THREAD
void BLUAhandleSocketOnConnect(lua_State*& L, const COLvar& Payload, int* pResult) {
   COL_FUNCTION(BLUAhandleSocketOnConnect);
   COLASSERT(L);
   int ConnectionId  = COLvarRequiredInt   (Payload, "conn_id");
   if (!LNETusingTcpAsync(L, ConnectionId)) {
      // deprecated socket.* API
      *pResult = BLUAdeprecatedOnConnect(L, ConnectionId);
      return;
   }
   // new net.tcpAsync.* API
   *pResult = LNETonConnect(L, ConnectionId);
}

static BLUAtask BLUAsocketOnDataTask(int ConnectionId, const COLstring& Data) {
   COL_FUNCTION(BLUAsocketOnDataTask);
   BLUAtask Task;
   Task.Id = BLUA_ON_DATA;
   Task.Payload["data"]    = Data;
   Task.Payload["conn_id"] = ConnectionId;
   return Task;
}

// for deprecated socket.* API
static int BLUAdeprecatedOnData(lua_State* L, int ConnectionId, const COLstring& Data) {
   lua_getglobal(L, "socket");
   lua_pushstring(L, "onData");
   lua_gettable(L, -2);
   if (!lua_isfunction(L, -1)) {
      LUApushString(L, "The 'socket.onData' callback function is not set.");
      return BLUA_ERROR_COMPILE;
   }
   LUApushString (L, Data);
   lua_pushnumber(L, ConnectionId);
   return lua_pcall(L, 2, 0, 0);
}

// LUA THREAD
void BLUAhandleSocketOnData(lua_State*& L, const COLvar& Payload, int* pResult) {
   COL_FUNCTION(BLUAhandleSocketOnData);
   COLASSERT(L);
   COLstring Data   = COLvarRequiredString(Payload, "data");
   int ConnectionId = COLvarRequiredInt   (Payload, "conn_id");
   if (!LNETusingTcpAsync(L, ConnectionId)) {
      // deprecated socket.* API
      *pResult = BLUAdeprecatedOnData(L, ConnectionId, Data);
      return;
   }
   // new net.tcpAsync.* API
   *pResult = LNETonData(L, ConnectionId, Data);
}

BLUAtask BLUAhttpOnDataTask(int ConnectionId, const COLstring& Data, int StreamId){
   COL_FUNCTION(BLUAhttpOnDataTask);
   BLUAtask Task;
   Task.Id = BLUA_HTTP_DATA;
   Task.Payload["data"]    = Data;
   Task.Payload["conn_id"] = ConnectionId;
   if (StreamId != 0) {
      Task.Payload["stream_id"] = StreamId;
   }
   return Task;
}

static void BLUAhttpInjectConnectionHeaders(COLstring& Request, int ConnectionId, int StreamId){
   COL_FUNCTION(BLUAhttpInjectConnectionHeaders);
   COLstring Headers, Body;
   Request.split(Headers, Body, "\r\n\r\n");
   Headers.append("\r\nifw_conn_id: " + COLintToString(ConnectionId) + "\r\n");
   if (StreamId != 0){
      Headers.append("ifw_stream_id: " + COLintToString(StreamId) + "\r\n");
   }
   Headers.append("\r\n");
   Request = Headers + Body;
}

// LUA THREAD
void BLUAhandleHttpOnData(lua_State*& L, const COLvar& Payload, int* pResult) {
   COL_FUNCTION(BLUAhandleHttpOnData);
   COLASSERT(L);
   COLstring Data   = COLvarRequiredString(Payload, "data");
   int ConnectionId = COLvarRequiredInt   (Payload, "conn_id");
   int StreamId     = COLvarGet           (Payload, "stream_id", 0);
   COL_DBG(Data);
   if (!BLUAgetCalledFunction(COL_T("main"), L, pResult)){
      COL_TRC("No main function found.");
      return;
   }
   lua_pushinteger(L, ConnectionId);
   lua_setfield(L, LUA_REGISTRYINDEX, "BLUAcurrentHttpConnection");
   if (StreamId != 0) {
      lua_pushinteger(L, StreamId);
      lua_setfield(L, LUA_REGISTRYINDEX, "BLUAcurrentHttp2Stream");
   }
   BLUAhttpInjectConnectionHeaders(Data, ConnectionId, StreamId); // For multithreaded server. These get removed in net.http.respond.
   LUApushString (L, Data);
   *pResult = lua_pcall(L, 1, 0, 0);
}

static BLUAtask BLUAsocketOnWriteTask(int ConnectionId) {
   COL_FUNCTION(BLUAsocketOnWriteTask);
   BLUAtask Task;
   Task.Id = BLUA_ON_WRITE;
   Task.Payload["conn_id"] = ConnectionId;
   return Task;
}

// for deprecated socket.* API
static int BLUAdeprecatedOnWrite(lua_State* L, int ConnectionId) {
   lua_getglobal(L, "socket");
   lua_pushstring(L, "onWrite");
   lua_gettable(L, -2);
   if (!lua_isfunction(L, -1)) {
      LUApushString(L, "The 'socket.onWrite' callback function is not set.");
      return BLUA_ERROR_COMPILE;
   }
   lua_pushnumber(L, ConnectionId);
   return lua_pcall(L, 1, 0, 0);
}

// LUA THREAD
void BLUAhandleSocketOnWrite(lua_State*& L, const COLvar& Payload, int* pResult) {
   COL_FUNCTION(BLUAhandleSocketOnWrite);
   COLASSERT(L);
   int ConnectionId  = COLvarRequiredInt   (Payload, "conn_id");
   if (!LNETusingTcpAsync(L, ConnectionId)) {
      // deprecated socket.* API
      *pResult = BLUAdeprecatedOnWrite(L, ConnectionId);
      return;
   }
   // new net.tcpAsync.* API
   *pResult = LNETonWrite(L, ConnectionId);
}

static BLUAtask BLUAsocketOnCloseTask(int ConnectionId, const COLstring& Data, const COLstring& Error) {
   COL_FUNCTION(BLUAsocketOnCloseTask);
   BLUAtask Task;
   Task.Id = BLUA_ON_CLOSE;
   Task.Payload["data"]    = Data;
   Task.Payload["error"]   = Error;
   Task.Payload["conn_id"] = ConnectionId;
   return Task;
}

// for deprecated socket.* API
static int BLUAdeprecatedOnClose(lua_State* L, int ConnectionId, const COLstring& Error, const COLstring& Data) {
   lua_getglobal(L, "socket");
   lua_pushstring(L, "onClose");
   lua_gettable(L, -2);
   if (!lua_isfunction(L, -1)) {
      LUApushString(L, "The 'socket.onClose' callback function is not set.");
      return BLUA_ERROR_COMPILE;
   }
   LUApushString (L, Data);
   lua_pushnumber(L, ConnectionId);
   if (!Error.is_null()) {
      LUApushString (L, Error);
      return lua_pcall(L, 3, 0, 0);
   } else {
      return lua_pcall(L, 2, 0, 0);
   }
}

// LUA THREAD
void BLUAhandleSocketOnClose(lua_State*& L, const COLvar& Payload, int* pResult) {
   COL_FUNCTION(BLUAhandleSocketOnClose);
   COLASSERT(L);
   COLstring Data   = COLvarRequiredString(Payload, "data");
   COLstring Error  = COLvarGetString     (Payload, "error", "");
   int ConnectionId = COLvarRequiredInt   (Payload, "conn_id");
   if (!LNETusingTcpAsync(L, ConnectionId)) {
      // deprecated socket.* API
      *pResult = BLUAdeprecatedOnClose(L, ConnectionId, Error, Data);
      return;
   }
   // new net.tcpAsync.* API
   *pResult = LNETonClose(L, ConnectionId, Error, Data);
}

void BLUAinstanceTearDownSocket(BLUAinstance2* pInstance) {
   COL_FUNCTION(BLUAinstanceTearDownSocket);
   SCKlistener*& pListener = pInstance->m_pListener;
   if (pListener) {
      COL_TRC("Stop listening for incoming connect requests.");
      pListener->close();
      delete pListener;
      pListener = NULL;
   }

   // todo: close connections later if we are serving a request
   COLhashmap<int, SCKconnection*>& Connections = pInstance->m_Connections;
   COL_TRC("Closing all connections: " << Connections.size());
   while (!Connections.empty()) {
      auto i = Connections.begin();
      COL_VAR(i->first);
      i->second->hardClose();
   }

   SCKSsslContext*& pServerContext = pInstance->m_pServerContext;
   if (pServerContext) {
      delete pServerContext;
      pServerContext = NULL;
   }

   SCKSsslContext*& pClientContext = pInstance->m_pClientContext;
   if (pClientContext) {
      delete pClientContext;
      pClientContext = NULL;
   }
}

void BLUAinstancePostSocketEvent(BLUAinstance2* pInstance, BLUAtaskId TaskId) {
   COL_FUNCTION(BLUAinstancePostSocketEvent);
   if (BLUA_ON_DATA != TaskId) {
      return;
   }
   auto Connections = pInstance->m_Connections;
   for (auto Itr = Connections.begin(); Itr != Connections.end(); Itr++) {
      int ConnId = Itr.key();
      if (Connections[ConnId]->bufferIn().size() > 0) {
         COL_TRC("Connection " << ConnId << " has more data");
         pInstance->m_JobQueue.put(BLUAsocketOnDataTask(ConnId, Connections[ConnId]->bufferIn()));
      }
      break;
   }
}


static void BLUAinstanceSocketOnAsyncConnect(COLuint64 ConnectionId, BLUAinstance2* pInstance, int Id) {
   COL_FUNCTION(BLUAinstanceSocketOnAsyncConnect);
   pInstance->m_JobQueue.put(BLUAsocketOnConnectTask(Id));
}

static void BLUAinstanceSocketOnAsyncData(COLuint64 ConnectionId, COLstring* pData, BLUAinstance2* pInstance, int Id) {
   COL_FUNCTION(BLUAinstanceSocketOnAsyncData);
   pInstance->m_JobQueue.put(BLUAsocketOnDataTask(Id, pInstance->m_Connections[Id]->bufferIn()));
   pInstance->m_Connections[Id]->bufferIn().clear();
}

static void BLUAinstanceSocketOnAsyncWritable(COLuint64 ConnectionId, BLUAinstance2* pInstance, int Id) {
   COL_FUNCTION(BLUAinstanceSocketOnAsyncWritable);
   pInstance->m_JobQueue.put(BLUAsocketOnWriteTask(Id));
}

static void BLUAhandleError(BLUAinstance2* pInstance, int ConnId, COLstring* pError) {
   COLstring& Error = *pError;
   SCKconnection* pConn = pInstance->m_Connections[ConnId];
   if (pConn->sslEnabled()) {
      SCKSsslConnection* pSslConn = (SCKSsslConnection*) pConn;
      Error = pSslConn->sslError();
   }
   int LastError = pConn->lastError();
   if (LastError != 0 ) {
      Error = SCKsocketError(LastError);
   }
}

static void BLUAinstanceSocketOnAsyncClose(COLuint64 ConnectionId, BLUAinstance2* pInstance, int Id) {
   COL_FUNCTION(BLUAinstanceSocketOnAsyncClose);
   COLstring Error;
   BLUAhandleError(pInstance, Id, &Error);
   pInstance->m_JobQueue.put(BLUAsocketOnCloseTask(Id, pInstance->m_Connections[Id]->bufferIn(), Error));
   pInstance->m_Connections.removeKey(Id);
   COL_TRC("Connection is closed " << Id);
}

static void BLUAinstanceHookupCallbacks(SCKconnection* pConnection, BLUAinstance2* pInstance, int ConnId){
   COL_FUNCTION(BLUAinstanceHookupCallbacks);
   pConnection->m_pOnData    =COLnewClosure2(&BLUAinstanceSocketOnAsyncData,     pInstance, ConnId);
   pConnection->m_pOnWritable=COLnewClosure1(&BLUAinstanceSocketOnAsyncWritable, pInstance, ConnId);
   pConnection->m_pOnClose   =COLnewClosure1(&BLUAinstanceSocketOnAsyncClose,    pInstance, ConnId);
}

SCKconnection* BLUAsetupRegularOrSslConnection(BLUAinstance2* pInstance, SCKaddress* pClientAddress){
   COL_FUNCTION(BLUAsetupRegularOrSslConnection);
   int Socket = pInstance->m_pListener->accept(pClientAddress);
   SCKconnection* pConnection = NULL;
   if (pInstance->m_pServerContext) {
      pConnection = new SCKSsslConnection(pInstance->m_pServerContext, *pInstance->m_pLoop, Socket);
   } else {
      pConnection = new SCKconnection(*pInstance->m_pLoop, Socket);
   }
   return pConnection;
}

void BLUAaddConnectionToMaps(BLUAinstance2* pInstance, SCKconnection* pConnection, int ConnId){
   COL_FUNCTION(BLUAaddConnectionToMaps);
   pInstance->m_Connections.add(ConnId, pConnection);
}

static void BLUAinstanceSocketOnAsyncAccept(BLUAinstance2* pInstance) {
   COL_FUNCTION(BLUAinstanceSocketOnAsyncAccept);
   SCKaddress ClientAddress;
   SCKconnection* pConnection = BLUAsetupRegularOrSslConnection(pInstance, &ClientAddress);
   int& ConnId = pInstance->m_ConnId;
   BLUAinstanceHookupCallbacks(pConnection, pInstance, ConnId);
   BLUAaddConnectionToMaps(pInstance, pConnection, ConnId);
   COLstring Address = ClientAddress.toString() + ":" + COLintToString(ClientAddress.port());
   pInstance->m_JobQueue.put(BLUAsocketOnAcceptTask(ConnId, Address));
   ConnId++;
}


static bool BLUAinstanceHandleSslParameters(BLUAinstance2* pInstance, const COLstring& SSLcert, const COLstring& SSLkey, bool VerifyPeer, const COLstring& CAfile, SCKSsslAPLNselectCallback pAlpnSelector, COLstring* pError) {
   COL_FUNCTION(BLUAinstanceHandleSslParameters);
   COLstring& Error = *pError;
   if (SSLcert.is_null() && SSLkey.is_null()) {
      COL_TRC("No SSL config. skip.");
      return true;
   }
   SCKSsslContext*& pServerContext = pInstance->m_pServerContext;
   if (pServerContext == NULL) {
      pServerContext = new SCKSsslContext();
      pServerContext->init();
   }
   if (pServerContext->setCertificate(SSLcert)) {
      Error = "Failed to load SSL certificate at " + SSLcert;
      COL_ERR(Error);
      return false;
   }
   if (pServerContext->setPrivateKey(SSLkey))  {
      Error = "Failed to load SSL key at " + SSLkey;
      COL_ERR(Error);
      return false;
   }
   if (pServerContext->setVerify(VerifyPeer)) {
      Error = "Failed to set verify peer";
      COL_ERR(Error);
      return false;
   }
   if (!CAfile.is_null() && pServerContext->setCAs(CAfile)){
      Error = "Failed to load certificate authority file at " + CAfile;
      COL_ERR(Error);
      return false;
   }
   if (pServerContext->setAlpnSelectCallback(pAlpnSelector)) {
      Error = "Failed to set ALPN select callback";
      COL_ERR(Error);
      return false;
   }
   // TODO: other SSL options we want to support
   return true;
}

void BLUAsetupListener(BLUAinstance2* pInstance, int Port, const BLUAsocketSslParams& SslParams, COLclosure0* pOnAccept, COLstring* pError, SCKSsslAPLNselectCallback pAlpnSelector){
   COL_FUNCTION(BLUAsetupListener);
   SCKlistener*& pListener = pInstance->m_pListener;
   do {
      if (pListener) {
         COL_WRN("Already listening");    // TODO: may be this should be fatal error
         break;
      };
      if (!BLUAinstanceHandleSslParameters(pInstance, SslParams.m_Cert, SslParams.m_PrivateKey, SslParams.m_VerifyPeer, SslParams.m_CAfile, pAlpnSelector, pError)) {
         break;
      }
      pListener = new SCKlistener(*pInstance->m_pLoop);
      pListener->setOnAccept(pOnAccept);
      if (!pListener->listen(Port)) {
         COL_TRC("Listen failed.");
         *pError = "Failed to bind to port " + COLintToString(Port);
      }
   } while (false);
}

static void BLUAinstanceListenOnMainThread(std::weak_ptr<struct LUAsynchronize> pAsync, BLUAinstance2* pInstance, int Port, BLUAsocketSslParams SslParams, COLstring* pError) {
   COL_FUNCTION(BLUAinstanceListenOnMainThread);
   COLclosure0* pOnAccept = COLnewClosure0(&BLUAinstanceSocketOnAsyncAccept, pInstance);
   BLUAsetupListener(pInstance, Port, SslParams, pOnAccept, pError);
   LUAasync(pAsync);    // sync BLUAinstanceListenAsync()
}

// must be called from Lua worker thread
bool BLUAinstanceListenAsync (BLUAinstance2* pInstance, int Port, const COLstring& SSLcert, const COLstring& SSLkey, bool VerifyPeer, const COLstring& CAfile, COLstring* pError){
   COL_FUNCTION(BLUAinstanceListenAsync);
   pError->clear();
   BLUAsocketSslParams SslParams = BLUAsocketSslParams(SSLkey, SSLcert, VerifyPeer, CAfile);
   COLclosure1<std::weak_ptr<struct LUAsynchronize>>* pFunction = COLnewClosure1(&BLUAinstanceListenOnMainThread, pInstance, Port, SslParams, pError);
   bool Success = LUAawait(pFunction, pInstance->m_pLoop, pInstance->m_ForceKill);
   if (!Success) {
      COL_TRC("LUAwait() timed out.");
      *pError = "Timeout";    // TODO: better error msg
   }
   return pError->is_null();
}

SCKconnection* BLUAsetupRegularOrSslClientConnection(BLUAinstance2* pInstance, bool Secure){
   COL_FUNCTION(BLUAsetupRegularOrSslClientConnection);
   SCKconnection* pConnection = NULL;
   if (Secure) {
      // TODO: all client connections share the same client SSL context
      SCKSsslContext*& pContext = pInstance->m_pClientContext;
      if(!pContext) {
         pContext = new SCKSsslContext(false);
         pContext->init();
      }
      pConnection = new SCKSsslConnection(pContext, *pInstance->m_pLoop);
   } else {
      pConnection = new SCKconnection(*pInstance->m_pLoop);
   }
   return pConnection;
}

void BLUAinstanceConnectOnMainThread(std::weak_ptr<struct LUAsynchronize> pAsync, BLUAinstance2* pInstance, COLstring Host, int Port, bool Secure, int* pConnId) {
   COL_FUNCTION(BLUAinstanceConnectOnMainThread);
   // BLUAinstanceConnectAsync(pInstance, Host, Port, Secure);
   SCKconnection* pConnection = BLUAsetupRegularOrSslClientConnection(pInstance, Secure);
   int& ConnId = pInstance->m_ConnId;
   BLUAinstanceHookupCallbacks(pConnection, pInstance, ConnId);
   BLUAaddConnectionToMaps(pInstance, pConnection, ConnId);
   pConnection->m_pOnConnect = COLnewClosure1(&BLUAinstanceSocketOnAsyncConnect,  pInstance, ConnId);
   pConnection->connect(Host, Port);
   *pConnId = ConnId;
   ConnId++;
   LUAasync(pAsync);    // sync BLUAinstanceConnect()
}

// must be called from Lua worker thread
// for net.tcpAsync.connect, which deprecates socket.connect_a
bool BLUAinstanceConnectAsyncNew(BLUAinstance2* pInstance, COLstring Host, int Port, bool Secure, int* pConnId) {
   COL_FUNCTION(BLUAinstanceConnectAsyncNew);
   COLclosure1<std::weak_ptr<struct LUAsynchronize>>* pFunction = COLnewClosure1(&BLUAinstanceConnectOnMainThread, pInstance, Host, Port, Secure, pConnId);
   bool Success = LUAawait(pFunction, pInstance->m_pLoop, pInstance->m_ForceKill);
   if (!Success) {
      COL_TRC("LUAwait() timed out.");
      return false;
   }
   return true;
}

// must be called from main thread
// for socket.connect_a, which is deprecated by net.tcpAsync.connect
void BLUAinstanceConnectAsync(BLUAinstance2* pInstance, COLstring Host, int Port, bool Secure) {
   COL_FUNCTION(BLUAinstanceConnectAsync);
   SCKconnection* pConnection = NULL;
   if (Secure) {
      SCKSsslContext*& pContext = pInstance->m_pClientContext;
      if(!pContext) {
         pContext = new SCKSsslContext(false);
         pContext->init();
      }
      pConnection = new SCKSsslConnection(pContext, *pInstance->m_pLoop);
   } else {
      pConnection = new SCKconnection(*pInstance->m_pLoop);
   }
   // TODO: okay to use int& ? it's a bit easier than int*
   int& ConnId = pInstance->m_ConnId;
   // TODO - why do we have two places we need to do this?
   pConnection->m_pOnConnect =COLnewClosure1(&BLUAinstanceSocketOnAsyncConnect,  pInstance, ConnId);
   BLUAinstanceHookupCallbacks(pConnection, pInstance, ConnId);
   pInstance->m_Connections.add(ConnId, pConnection);
   ConnId++;
   pConnection->connect(Host, Port);
}

// must be called from main thread
void BLUAinstanceSendAsync(BLUAinstance2* pInstance, COLauto<COLstring> Data, int ConnId) {
   COL_FUNCTION(BLUAinstanceSendAsync);
   if (pInstance->m_Connections.count(ConnId)) {
      COL_DUMP("Data", Data->c_str(), Data->size());
      pInstance->m_Connections[ConnId]->writeString(Data);
   }
}

void BLUAinstanceCloseAsync(BLUAinstance2* pInstance, int ConnId) {
   COL_FUNCTION(BLUAinstanceCloseAsync);
   if (pInstance->m_Connections.count(ConnId)) {
      pInstance->m_Connections[ConnId]->hardClose();
   }
}
