//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testSSL
//
// Description:
//
// Author: Kevin Cai
//---------------------------------------------------------------------------

#include "testSSL.h"

#include <UNIT/UNITapp.h>

#include <SCK/SCKaddress.h>
#include <SCK/SCKerror.h>
#include <SCK/SCKlistener.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKnameResolver.h>
#include <SCK/SCKutils.h>

#include <SCKS/SCKSsslCertificate.h>
#include <SCKS/SCKSsslConnection.h>
#include <SCKS/SCKSsslContext.h>
#include <SCKS/SCKSsslUtils.h>

#include <FIL/FILutils.h>

#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void setUpServerCtx(SCKSsslContext& Ctx){
   COL_FUNCTION(setUpServerCtx);
   if (0 != SCKSsetServerCtx(Ctx, "cacrt.pem", "cakey.pem")){
      COL_ERR(SCKSsslError());
      UNIT_ASSERT(false);
   }
}

static void setUpClientCtx(SCKSsslContext& Ctx){
   COL_FUNCTION(setUpClientCtx);
   if (0 != SCKSsetClientCtx(Ctx)){
      COL_ERR(SCKSsslError());
      UNIT_ASSERT(false);
   }
}

static void testContextSetup(){ //TODO: Test more things in this function.
   COL_FUNCTION(testContextSetup);
   SCKSsslContext serverCtx, clientCtx;
   COLASSERT(serverCtx.isServerMode() == true);
   COLASSERT(serverCtx.getCertificatePath() == "");
   COLASSERT(serverCtx.getPrivateKeyPath() == "");
   COLASSERT(serverCtx.getCAsPath() == "");
   setUpServerCtx(serverCtx);
   setUpClientCtx(clientCtx);

   UNIT_ASSERT(SCKSsslError() == "");
   UNIT_ASSERT(0 != serverCtx.setCertificate("ERRORFILE.txt"));
   SCKSsslError();
}

static void OnDataEcho(COLuint64 ConnectionId, COLstring* pData, SCKSsslConnection* pSslConnection, COLstring* pReceived){
   COL_FUNCTION(OnDataEcho);
   COL_VAR(pData->size());
   pReceived->append(*pData);
   // TODO - could we change this to use ConnectionId interface instead of SCKSsslConnection pointer?
   pSslConnection->writeString(*pData);
   pData->clear();
}

static void EndLoop(COLuint64 ConnectionId, SCKloop* pLoop){
   COL_FUNCTION(EndLoop);
   pLoop->shutdown();
}

static void MakeSslEchoConnection (SCKlistener* pServer, SCKSsslContext* pCtx, COLstring * ServerRecv){
   COL_FUNCTION(MakeSslEchoConnection);
   int PeerSocket = pServer->accept();
   COL_VAR(PeerSocket);
   if (PeerSocket == SCK_INVALID_SOCKET){
      COL_ERR("Accept failed.  This is the last socket error: " << SCKsocketError(pServer->lastError()));
      return;
   }
   SCKSsslConnection * pSslConnection = new SCKSsslConnection(pCtx, *pServer->loop(), PeerSocket);
   pSslConnection->m_pOnData=COLnewClosure2(&OnDataEcho, pSslConnection, ServerRecv);
   pSslConnection->m_pOnClose=COLnewClosure1(&EndLoop, pServer->loop());
}

static void OnConnectSendHello(COLuint64 ConnectionId, SCKSsslConnection* pSslConnection){
   COL_FUNCTION(OnConnectSendHello);
   pSslConnection->writeString("Hello");
}

static void OnEchoSuccessShutdown(COLuint64 ConnectionId, COLstring* pData, SCKSsslConnection* pSslConnection, COLstring* Response){
   COL_FUNCTION(OnEchoSuccessShutdown);
   Response->append(pSslConnection->bufferIn());
   pSslConnection->bufferIn().clear();
   COL_VAR(Response->size());
   if (Response->equals("Hello")){
      COL_TRC("Hello received. Closing connection");
      pSslConnection->close();
      return;
   }
}

static void testSimpleConnectionHelper(const COLstring& LocalAddress, bool IpV6){
   COL_FUNCTION(testSimpleConnectionHelper);
   SCKSsslContext serverCtx, clientCtx;
   setUpServerCtx(serverCtx);
   setUpClientCtx(clientCtx);

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   SCKlistener Server(Loop);
   COLstring ServerRecv; // Data received on server side.

   Server.setOnAccept(COLnewClosure0(&MakeSslEchoConnection, &Server, &serverCtx, &ServerRecv));
   SCKaddress Address(0);
   Address.setIpv6(IpV6);
   Address.setAny();
   if (!Server.listen(Address)){
      throw COLerror(SCKsocketError(Server.lastError()), Server.lastError());
   }
   COL_TRC("Called listen");
   COLstring Response;
   SCKSsslConnection* pSslConnection = new SCKSsslConnection(&clientCtx, Loop);
   pSslConnection->m_pOnConnect =COLnewClosure1(&OnConnectSendHello, pSslConnection);
   pSslConnection->m_pOnData = COLnewClosure2(&OnEchoSuccessShutdown, pSslConnection, &Response);

   pSslConnection->connect(LocalAddress, Server.port());
   Loop.start();
   UNIT_ASSERT_EQUALS("Hello", Response);
}

static void testSimpleConnection(){
   COL_FUNCTION(testSimpleConnection);
   testSimpleConnectionHelper("127.0.0.1", false);
}

static void testSimpleConnectionV6(){
   COL_FUNCTION(testSimpleConnectionV6);
   if (!SCKsocketIpV6Supported()){
      return;
   }
   testSimpleConnectionHelper("::1", true);
}

static void testWriteBeforeConnect(){
   COL_FUNCTION(testLargeOperation);
   SCKSsslContext serverCtx, clientCtx;
   setUpServerCtx(serverCtx);
   setUpClientCtx(clientCtx);

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   SCKlistener Server(Loop);

   COLstring ServerRecv; // Data received on server side.
   Server.setOnAccept(COLnewClosure0(&MakeSslEchoConnection, &Server, &serverCtx, &ServerRecv));

   SCKaddress Address(0);
   Address.setAny();
   if (!Server.listen(Address)){
      throw COLerror(SCKsocketError(Server.lastError()), Server.lastError());
   }
   COL_TRC("Called listen");
   COLstring Response;

   SCKSsslConnection * pSslConnection = new SCKSsslConnection(&clientCtx, Loop);
   pSslConnection->m_pOnData = COLnewClosure2(&OnEchoSuccessShutdown, pSslConnection, &Response);

   pSslConnection->writeString("Hello");
   pSslConnection->connect("127.0.0.1", Server.port());

   Loop.start();
   UNIT_ASSERT(Response.equals("Hello"));
}

const static int LARGE_DATA_SIZE = 10000000;

static void OnLargeEchoSuccessShutdown(COLuint64 ConnectionId, COLstring* pData, SCKSsslConnection * pSslConnection, COLstring * Response){
   COL_FUNCTION(OnLargeEchoSuccessShutdown);
   Response->append(*pData);
   pData->clear();
   COL_VAR(Response->size());
   if (Response->size() == LARGE_DATA_SIZE){
      COL_TRC("" << LARGE_DATA_SIZE << " data received. Closing socket connection.");
      pSslConnection->close();
      return;
   }
}

static void testLargeOperation(){
   COL_FUNCTION(testLargeOperation);
   SCKSsslContext serverCtx, clientCtx;
   setUpServerCtx(serverCtx);
   setUpClientCtx(clientCtx);

   COL_VAR(LARGE_DATA_SIZE);

   COLthreadPool ThreadPool(1, 10);
   ThreadPool.start();
   SCKloop Loop(&ThreadPool);

   COLstring ServerRecv; // Data received on server side.
   SCKlistener Server(Loop);
   Server.setOnAccept(COLnewClosure0(&MakeSslEchoConnection, &Server, &serverCtx, &ServerRecv));

   if (!Server.listen(0)){
      throw COLerror(SCKsocketError(Server.lastError()), Server.lastError());
   }
   COL_TRC("Called listen");

   COLstring LargeSend;
   LargeSend.setCapacity(LARGE_DATA_SIZE);
   LargeSend.setSize(LARGE_DATA_SIZE);
   for (int i=0; i < LARGE_DATA_SIZE; i++){
      LargeSend[i] = '0' + (i % 10);
   }
   COLstring Response;
   Response.setCapacity(LARGE_DATA_SIZE);

   SCKSsslConnection * pSslConnection = new SCKSsslConnection(&clientCtx, Loop);
   pSslConnection->m_pOnData =COLnewClosure2(&OnLargeEchoSuccessShutdown, pSslConnection, &Response);
   pSslConnection->writeString(LargeSend);
   pSslConnection->connect("localhost", Server.port());
   Loop.start();
   UNIT_ASSERT(LargeSend == Response);
}

static void EndLoopOnCount(COLuint64 ConnectionId, SCKloop* pLoop, int* Count){
   COL_FUNCTION(EndLoopOnCount);
   (*Count)--;
   COL_VAR(*Count);
   if (*Count == 0){
      COL_TRC("Count has hit 0. Closing loop!");
      pLoop->shutdown();
   }
}

static void MakeSslEchoConnectionMultiple(SCKlistener* pServer,
      SCKSsslContext* pCtx, COLstring* ServerRecv, int* Count){
   COL_FUNCTION(MakeSslEchoConnectionMultiple);

   int PeerSocket = pServer->accept();
   COL_VAR(PeerSocket);
   if (PeerSocket == SCK_INVALID_SOCKET){
      COL_ERR("Accept failed.  This is the last socket error: " << SCKsocketError(pServer->lastError()));
      return;
   }
   SCKSsslConnection * pSslConnection = new SCKSsslConnection(pCtx, *pServer->loop(), PeerSocket);
   pSslConnection->m_pOnData=COLnewClosure2(&OnDataEcho, pSslConnection, ServerRecv);
   pSslConnection->m_pOnClose=COLnewClosure1(&EndLoopOnCount, pServer->loop(), Count);
}

static void testMultipleConnect(){
   COL_FUNCTION(testMultipleConnect);
   SCKSsslContext serverCtx, clientCtx;
   setUpServerCtx(serverCtx);
   setUpClientCtx(clientCtx);

   COLthreadPool ThreadPool(1, 10);
   ThreadPool.start();
   SCKloop Loop(&ThreadPool);
   SCKnameResolver NameResolver(&ThreadPool, &Loop);

   COLstring ServerRecv;
   int Count = 10;
   SCKlistener Server(Loop);
   Server.setOnAccept(COLnewClosure0(&MakeSslEchoConnectionMultiple, &Server, &serverCtx, &ServerRecv, &Count));

   if (!Server.listen(0)){
      throw COLerror(SCKsocketError(Server.lastError()), Server.lastError());
   }
   COL_TRC("Called listen");

   COLstring Response[10];

   for (int i = 0; i < 10; i++){
       SCKSsslConnection * pSslConnection = new SCKSsslConnection(&clientCtx, Loop);
       pSslConnection->m_pOnData =COLnewClosure2(&OnEchoSuccessShutdown, pSslConnection, &Response[i]);
       pSslConnection->writeString("Hello");
       pSslConnection->connect("127.0.0.1", Server.port());
   }
   Loop.start();
   for (int i = 0; i < 10; i++){
      UNIT_ASSERT(Response[i].equals("Hello"));
   }
}

static void OnDataCloseOnLimit(COLuint64 ConnectionId, COLstring* pData, SCKSsslConnection* pConnection, int* pLimit){
   COL_FUNCTION(OnDataCloseOnLimit);
   (*pLimit) -= pData->size();
   pData->clear();
   COL_VAR(*pLimit);

   if (*pLimit == 0){
      COL_TRC("We received all " << LARGE_DATA_SIZE << " bytes. Closing serverside.");
      pConnection->close();
   }
}

static void MakeSslConnectionCloseOnLimit(SCKlistener* pServer, SCKSsslContext* pCtx, int* pLimit){
   COL_FUNCTION(MakeSslConnectionCloseOnLimit());
   int PeerSocket = pServer->accept();
   COL_VAR(PeerSocket);
   if (PeerSocket == SCK_INVALID_SOCKET){
      COL_ERR("Accept failed.  This is the last socket error: " << SCKsocketError(pServer->lastError()));
      return;
   }
   SCKSsslConnection * pSslConnection = new SCKSsslConnection(pCtx, *pServer->loop(), PeerSocket);
   pSslConnection->m_pOnData=COLnewClosure2(&OnDataCloseOnLimit, pSslConnection, pLimit);
}

static void testLargeWrite(){
   COL_FUNCTION(testLargeWrite);
   SCKSsslContext serverCtx, clientCtx;
   setUpServerCtx(serverCtx);
   setUpClientCtx(clientCtx);

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   SCKlistener Server(Loop);
   int Limit = LARGE_DATA_SIZE;
   Server.setOnAccept(COLnewClosure0(&MakeSslConnectionCloseOnLimit, &Server, &serverCtx, &Limit));

   if (!Server.listen(0)){
      throw COLerror(SCKsocketError(Server.lastError()), Server.lastError());
   }
   COL_TRC("Called listen");

   COLstring LargeSend;
   LargeSend.setCapacity(LARGE_DATA_SIZE);
   LargeSend.setSize(LARGE_DATA_SIZE);
   for (int i=0; i < LARGE_DATA_SIZE; i++){
      LargeSend[i] = '0' + (i % 10);
   }
   SCKSsslConnection* pSslConnection = new SCKSsslConnection(&clientCtx, Loop);
   pSslConnection->m_pOnClose=COLnewClosure1(&EndLoop, &Loop);
   pSslConnection->writeString(LargeSend);
   pSslConnection->connect("127.0.0.1", Server.port());

   Loop.start();
}

static void testMakeCertificate() {
   COLstring ConfigDir = "./cow";
   COL_VAR(ConfigDir);
   UNIT_ASSERT_EQUALS(true, SCKSsslMakeCertificate(ConfigDir, 2048, 1, 365));
   UNIT_ASSERT_EQUALS(true, FILfileExists("./cow/cert.pem"));
   UNIT_ASSERT_EQUALS(true, FILfileExists("./cow/key.pem"));
}

void testSsl(UNITapp* pApp){
   pApp->add("context/setup", &testContextSetup);
   pApp->add("connection/simple", &testSimpleConnection);
   pApp->add("connection/v6", &testSimpleConnectionV6);
   pApp->add("write/before/connect", &testWriteBeforeConnect);
   pApp->add("write/large", &testLargeWrite);
   pApp->add("write/large/operation", &testLargeOperation);
   pApp->add("connection/multiple", &testMultipleConnect);
}
