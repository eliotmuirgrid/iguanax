// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNEThttpListen
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Friday 27 October 2023 - 10:08AM
// ---------------------------------------------------------------------------
#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAsocket.h>
#include <COL/COLclosure6.h>
#include <COL/COLevent.h>
#include <COL/COLlog.h>
#include <FIL/FILutils.h>
#include <LNET/LNEThttpListen.h>
#include <LNET/LNETutils.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAutils.h>
#include <SCK/SCKerror.h>
#include <SCK/SCKconnection.h>
#include <SCK/SCKlistener.h>
#include <SCK/SCKloop.h>
#include <SCKS/SCKSsslConnection.h>
#include <SDB/SDBcapi.h>
#include <WEB/WEBrequestData.h>
#include <WEB/WEBserverParse.h>
COL_LOG_MODULE;

// OnData
// The onData needs to wait for a full http request, scan for its endpoint and then call main() when the paths match
// As a first cut, we just make sure we get a full request before passing it off
static void LNETsetSocketOnData(COLuint64 ConnectionId, COLstring* pData, BLUAinstance2* pInstance, int Id) {
   COL_FUNCTION(LNETsetSocketOnData);
   COL_DUMP("Buffer:", pData->data(), pData->size());
   if (LNETisHttp2Connection(pInstance, Id)) {
      COL_TRC("Handling client data received on HTTP2 connection " << Id);
      LNEThandleHttp2Data(pData, pInstance, Id);
      return;
   }
   if (LNETfoundHttp2Preface(*pData)) {
      COL_TRC("new HTTP2 connection without prior knowledge");
      LNETsetupHttp2Connection(pInstance, Id);
      return;
   }
   do {
      int FindHeaderEnd;
      if (!pInstance->m_Connections[Id]->bufferIn().find(&FindHeaderEnd,"\r\n\r\n")) {
         COL_TRC("Haven't got a complete body yet.");
         return;
      }
      int Consumed = 0;
      WEBrequestData RequestData;
      if (!WEBserverParse(&pInstance->m_Connections[Id]->bufferIn(), &RequestData, &Consumed)){ // providing the int* means this won't truncate
         COL_TRC("Haven't got a complete header yet.");
         return;
      }
      COL_TRC("Found a full http request");
      COLstring Request = pInstance->m_Connections[Id]->bufferIn().substr(0, Consumed); // grab the complete request
      if (LNETisHttp2UpgradeRequest(RequestData)) {
         COL_TRC("HTTP2 without prior knowledge");
         const COLstring& Http2Setting = RequestData.Headers["http2-settings"];
         LNETsetupHttp2Connection(pInstance, Id, Request, Http2Setting);
         pInstance->m_Connections[Id]->bufferIn().shift(Consumed);
         return;
      }
      COL_TRC("Add a call to main function on job queue");
      COL_VAR(Request);
      pInstance->m_JobQueue.put(BLUAhttpOnDataTask(Id, Request));
      COL_TRC("Consume connection buffer");
      pInstance->m_Connections[Id]->bufferIn().shift(Consumed);
   } while(true);
}

static void LNEThandleError(BLUAinstance2* pInstance, int ConnId) {
   SCKconnection* pConn = pInstance->m_Connections[ConnId];
   COLstring Error;
   if (pConn->sslEnabled()) {
      SCKSsslConnection* pSslConn = (SCKSsslConnection*) pConn;
      Error = pSslConn->sslError();
   }
   int LastError = pConn->lastError();
   if (LastError != 0 ) {
      Error = SCKsocketError(LastError);
   }
   if (Error.is_null()) { return; }
   Error.prepend("Connection " + COLintToString(ConnId) + " encountered error:\n");
   SDBIlogComponent(Error, pInstance->m_Config.CoreConfig.Guid, "#error");
   pInstance->m_Config.Status.ErrorCount++;  // increment error count
}

// OnClose
// This needs to remove the ConnId from the Connections map
static void LNETsetSocketOnClose(COLuint64 ConnectionId, BLUAinstance2* pInstance, int ConnId){
   COL_FUNCTION(LNETsetSocketOnClose);
   LNEThandleError(pInstance, ConnId);
   if (LNETisHttp2Connection(pInstance, ConnId)) {
      COL_TRC("Remove from HTTP2 connections map");
      LNETteardownHttp2Connection(pInstance, ConnId);
   }
   COL_TRC("Remove from Connections map");
   pInstance->m_Connections.removeKey(ConnId);
}

static void LNETsetSocketOnWrite(COLuint64 ConnectionId, BLUAinstance2* pInstance, int ConnId){
   COL_FUNCTION(LNETsetSocketOnWrite);
   if (!LNETisHttp2Connection(pInstance, ConnId)) { return; }
   LNEThttp2OnWrite(pInstance, ConnId);
}

// OnAccept
// This needs to set up an onData and an onClose
static void LNETsetSocketOnAccept(BLUAinstance2* pInstance){
   COL_FUNCTION(LNETsetSocketOnAccept);
   SCKaddress ClientAddress;
   COL_TRC("Create new SCKconnection");
   SCKconnection* pConnection = BLUAsetupRegularOrSslConnection(pInstance, &ClientAddress);
   int& ConnId = pInstance->m_ConnId;
   COL_TRC("Set up onData and onClose");
   pConnection->m_pOnData = COLnewClosure2(&LNETsetSocketOnData, pInstance, ConnId);
   pConnection->m_pOnClose = COLnewClosure1(&LNETsetSocketOnClose, pInstance, ConnId);
   pConnection->m_pOnWritable = COLnewClosure1(&LNETsetSocketOnWrite, pInstance, ConnId);
   COL_TRC("Add to Connections and ConnectionStates maps");
   BLUAaddConnectionToMaps(pInstance, pConnection, ConnId);
   ConnId++;
}

static void LNETsetupListenOnMainThread(BLUAinstance2* pInstance, int Port, BLUAsocketSslParams SslParams, COLstring* pError, COLevent* pEvent) {
   COL_FUNCTION(LNETsetupListenOnMainThread);
   COLclosure0* pOnAccept = COLnewClosure0(&LNETsetSocketOnAccept, pInstance);
   BLUAsetupListener(pInstance, Port, SslParams, pOnAccept, pError, LNETsslAlpnSelector); // This function will set up a regular socket if both Certificate and Key are null
   COL_TRC("Signal back to Lua thread that listener set up or error encountered.");
   pEvent->signal();
}

// must be called from Lua worker thread
bool LNETinstanceListenAsync(BLUAinstance2* pInstance, int Port, const COLstring& Certificate, const COLstring& PrivateKey, bool VerifyPeer, const COLstring& CAfile, COLstring* pError) {
   COL_FUNCTION(LNETinstanceListenAsync);
   pError->clear();
   COLevent Event;
   BLUAsocketSslParams SslParams(PrivateKey, Certificate, VerifyPeer, CAfile);
   pInstance->m_pLoop->post(COLnewClosure0(&LNETsetupListenOnMainThread, pInstance, Port, SslParams, pError, &Event));
   COL_TRC("Posting to main and running to set up SCKlistener and callbacks");
   COL_TRC("Blocking until socket listener set up...");
   Event.wait();
   COL_TRC("Listener set up");
   COL_VAR(*pError);
   return pError->is_null();
}

int LNEThttpListen(lua_State* L){
   COL_FUNCTION(LNEThttpListen);
   int Port = LUAexpectedIntParam(L, 1, "port");
   COLstring Certificate = LUAoptionalStringParam(L, 1, "cert");
   COLstring PrivateKey  = LUAoptionalStringParam(L, 1, "key");
   bool VerifyPeer       = LUAoptionalBoolParam(L, 1, "verify_peer", false);
   COLstring CAfile      = LUAoptionalStringParam(L, 1, "ca_file");
   COL_VAR3(Port, Certificate, PrivateKey);
   COL_VAR2(VerifyPeer, CAfile);
   COLstring Error;
   if (!LNETcheckCertAndPrivateKey(Certificate, PrivateKey, &Error)){ return LUAraiseLuaError(L, Error); }
   if (LUALisEnvironmentTest(L) || LUALisEnvironmentStartup(L)) {
      COL_TRC("this is a no-op in IDE mode");
      return 0;
   }
   BLUAinstance2* pInstance = BLUAinstanceGet(L);
   if(!LNETinstanceListenAsync(pInstance, Port, Certificate, PrivateKey, VerifyPeer, CAfile, &Error)){
      COL_VAR(Error);
      return LUAraiseLuaError(L, Error);
   }
   return 0;
}
