//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testSimpleNetwork
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tue 11 Nov 2014 14:03:22 EST
//---------------------------------------------------------------------------

#include "testSimpleNetwork.h"

#include <UNIT/UNITapp.h>

#include <SCK/SCKconnection.h>
#include <SCK/SCKlistener.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKnameResolver.h>
#include <SCK/SCKutils.h>
#include <SCK/SCKerror.h>

#include <time.h>

#include <COL/COLmath.h>
#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <string.h> // memmove.

static void testThrow(){
   COL_FUNCTION(testThrow);
   throw COLerror("test throw", 0);
}

static void testPostWithThrow(){
   COL_FUNCTION(testPostWithThrow);
   COLthreadPool ThreadPool;
   ThreadPool.start();
   SCKloop Loop(&ThreadPool);
   Loop.post(COLnewClosure0(&testThrow));
   Loop.addTimer(COLnewClosure0(&testThrow), 0);
   Loop.addTimer(COLnewClosure0(&Loop, &SCKloop::shutdown), 1000);
   Loop.start();
}

static void testListenBindError(){
   COL_FUNCTION(testListenBindError);
   COLthreadPool ThreadPool;
   SCKloop Loop(&ThreadPool);
   SCKlistener One(Loop);
   SCKlistener Two(Loop);

   bool Result1 = One.listen(0);
   bool Result2 = Two.listen(One.port());

   UNIT_ASSERT_EQUALS(true, Result1);
   UNIT_ASSERT_EQUALS(false, Result2);
   UNIT_ASSERT_EQUALS(SCK_EADDRINUSE, Two.lastError());  // expected error code.
}

#ifdef SCK_SELECT
//TODO: Consider for removal
/*
static void testTooManySockets(){ //Function not completed.
   COL_FUNCTION(testTooManySockets);
   SCKloop Loop;
   int MaxFd = Loop.maxFd();
   int Port = 3999;
   SCKaddress Address;
   COLlist< COLauto<SCKlistener> > ListenerList;
   for (int i=0; i < MaxFd + 11; i++){ //Is it 10 or 11?
      //SCKlistener * ListenSocket = new SCKlistener(Loop)
      COLauto<SCKlistener> ListenerBind(new SCKlistener(Loop));
      bool BoundListener = false;
      for (int Offset=0; Offset < 100; Offset++){ //Attempting to find open ports. Cap at 100.
         Port++;
         Address.setPort(Port);
         bool Result = ListenerBind->listen(Address);
         if (Result) {
            ListenerList.add(ListenerBind);
            BoundListener = true;
            continue;
         }
      }
      if (!BoundListener) {
         COL_ERR("Not enough open ports (" << MaxFd << ") to perform test");
         return;
      }
   }
}
*/
#endif

static void onConnectSendHello(COLuint64 ConnectionId, SCKconnection* pConnector){
   COL_FUNCTION(onConnectSendHello);
   pConnector->writeString("Hello world");
}

static void onDataEcho(COLuint64 ConnectionId, COLstring* pData, SCKconnection* pConnection){
   COL_FUNCTION(onDataEcho);
   pConnection->writeString(*pData);
   COL_DUMP("In:", pData->c_str(), pData->size());
   pData->clear();
}

static void onDataEchoForOne(COLuint64 ConnectionId, COLstring* pData, SCKconnection* pConnection, int* pCount){
   COL_FUNCTION(onDataEchoForOne);
   (*pCount)++;
   COL_VAR(*pCount);
   if (*pCount > 1){
      COL_TRC("Ending connection");
      pConnection->close();
      return;
   }
   pConnection->writeString(*pData);
   COL_DUMP("In:", pData->c_str(), pData->size());
   pData->clear();
}

static void MakeEchoConnectionForOne(SCKlistener* pServer, int* pCount){
   COL_FUNCTION(MakeEchoConnectionForOne);
   // Now we have to accept what we are given.
   COL_TRC("Accept what you are given");
   SCKaddress PeerAddress;
   int PeerSocket = pServer->accept();
   COL_VAR(PeerAddress);
   if (PeerSocket == SCK_INVALID_SOCKET){
      COL_ERR("Accept failed.  This is the last socket error: " << SCKsocketError(pServer->lastError()));
      return;
   }
   SCKconnection* pConnection = new SCKconnection(*pServer->loop(), PeerSocket);
   pConnection->m_pOnData=COLnewClosure2(&onDataEchoForOne, pConnection, pCount);
}

static void EndLoop(COLuint64 ConnectionId, SCKloop* pLoop){
   COL_FUNCTION(EndLoop);
   pLoop->shutdown();
}

static void testNewConnectHelper(const COLstring& LocalAddress, bool IpV6, int Port){
   COL_FUNCTION(testNewConnectHelper);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   int Count = 0;
   SCKlistener Server(Loop);
   COL_TRC("Created SCKlistener");
   Server.setOnAccept(COLnewClosure0(&MakeEchoConnectionForOne, &Server, &Count));
   if (!Server.listen(Port)){
      throw COLerror(SCKsocketError(Server.lastError()), Server.lastError());
   }
   COL_TRC("Called listen");
   SCKconnection* pConnection = new SCKconnection(Loop);
   pConnection->m_pOnConnect=COLnewClosure1(&onConnectSendHello, pConnection);
   pConnection->m_pOnData   =COLnewClosure2(&onDataEcho,         pConnection);
   pConnection->m_pOnClose  =COLnewClosure1(&EndLoop,            &Loop);
   pConnection->connect("localhost", Port);
   Loop.start();
   UNIT_ASSERT_EQUALS(2, Count);
}


static void testNewConnect(){
   COL_FUNCTION(testNewConnect);
   testNewConnectHelper("127.0.0.1", false, 9999);
}

static void testNewConnectSCKv6(){
   COL_FUNCTION(testNewConnectSCKv6);
   if (!SCKsocketIpV6Supported()){
      return;
   }
   testNewConnectHelper("::1", true, 12828);
}

static const int SIZE_OF_MY_DATA = 100000000;

static void onDataEchoWithBufferAndCloseWithLimit(COLuint64 ConnectionId, COLstring* pData, SCKconnection* pConnection, int* pTotalAmountRead, int Limit){
   COL_FUNCTION(onDataEchoWithBufferAndCloseWithLimit);
   COL_VAR(*pTotalAmountRead);
   *pTotalAmountRead += pData->size();
   pConnection->writeString(*pData);
   pData->clear();
}

static void onDataWrittenClose(COLuint64 ConnectionId, SCKconnection* pConnection, int* pTotalAmountRead, int Limit){
   COL_FUNCTION(onDataWrittenClose);
   COL_VAR2(*pTotalAmountRead, Limit);
   if (*pTotalAmountRead == Limit){
      COL_TRC("Echoed back data - closing now.");
      pConnection->close();
   }
}

static void MakeEchoConnectionWithLimit(SCKlistener* pServer, int* pAmountRead, int Limit){
   COL_FUNCTION(MakeEchoConnectionWithLimit);
   COL_VAR2(*pAmountRead, Limit);
   COL_TRC("About to accept socket connection.");
   SCKaddress Address;
   int PeerSocket = pServer->accept(&Address);
   COL_VAR(Address);
   if (PeerSocket == SCK_INVALID_SOCKET){
      COL_ERR("Accept failed.  This is the last socket error: " << SCKsocketError(pServer->lastError()));
      return;
   }
   SCKconnection* pConnection = new SCKconnection(*pServer->loop(), PeerSocket, 100000-1);
   // Interesting if you set the capacity of the echo buffer to be much smaller the transfer does take longer.
   // Because IOCP takes a pointer to the buffer we are not allowed to mess with it here.
   //pConnection->bufferIn().setCapacity(100000-1);
   COL_VAR(pConnection->bufferIn().capacity());
   pConnection->m_pOnData=COLnewClosure2(&onDataEchoWithBufferAndCloseWithLimit, pConnection, pAmountRead, Limit);
   pConnection->m_pOnWritable=COLnewClosure1(&onDataWrittenClose, pConnection, pAmountRead, Limit);
   //pConnection->init();
}

static void OnBigConnect(COLuint64 ConnectionId, SCKconnection* pConnection){
   COL_FUNCTION(OnBigConnect);
   COL_TRC("Connected to big data server.");
   pConnection->bufferIn().setCapacity(100000-1);
   COL_TRC("Made a large output buffer.");
}

static void OnGetBigData(COLuint64 ConnectionId, COLstring* pData,SCKconnection* pConnection, COLstring* pDataOut){
   COL_FUNCTION(OnGetBigData);
   pDataOut->append(*pData);
   pData->clear();
   COL_VAR(pDataOut->size());
}

static void OnBigClose(COLuint64 ConnectionId, SCKconnection* pConnection){
   COL_FUNCTION(OnBigClose);
   pConnection->m_pLoop->shutdown();
}

static void OnCloseCountdown(COLuint64 ConnectionId, SCKconnection* pConnection, int* pCountdown){
   COL_FUNCTION(OnCloseCountdown);
   (*pCountdown)--;
   COL_VAR(*pCountdown);
   if (*pCountdown == 0){
      COL_TRC("Shutting down Loop");
      pConnection->m_pLoop->shutdown();
   }
}

// This is for testing out writing a large amount of data.
static void testLargeWrite(){
   COL_FUNCTION(testLargeWrite);
   COLthreadPool ThreadPool(1, 10);
   ThreadPool.start();
   SCKloop Loop(&ThreadPool);

   COLstring DataIn;
   COL_TRC("Got DataIn string created.");
   DataIn.setCapacity(SIZE_OF_MY_DATA);
   COL_TRC("Allocated memory:" << SIZE_OF_MY_DATA);
   DataIn.setSize(SIZE_OF_MY_DATA);
   COL_TRC("Allocated size.");
   for (int i=0; i < SIZE_OF_MY_DATA; i++){
      DataIn[i] = '0' + (i % 10);
   }
   COL_TRC("Initialized large data set.");
   COL_TRC("Initialized data.");
   COL_VAR(DataIn.size());
   int AmountEchoed = 0;
   SCKlistener Server(Loop);
   Server.setOnAccept(COLnewClosure0(&MakeEchoConnectionWithLimit, &Server, &AmountEchoed, SIZE_OF_MY_DATA));
   if (!Server.listen(0)){
      throw COLerror(SCKsocketError(Server.lastError()), Server.lastError());
   }

   COLstring DataOut;
   DataOut.setCapacity(SIZE_OF_MY_DATA);
   //int ProgressCount=0;
   SCKconnection* pConnection = new SCKconnection(Loop);
   pConnection->m_pOnConnect=COLnewClosure1(&OnBigConnect, pConnection);
   pConnection->m_pOnData=COLnewClosure2(&OnGetBigData, pConnection, &DataOut);
   pConnection->m_pOnClose=COLnewClosure1(&OnBigClose, pConnection);
   COL_TRC("About to send data.");
   COL_TRC("Transfer data into socket output buffer.");
   pConnection->writeString(DataIn);
   COL_TRC("Done.");
   COL_TRC("Given data to connection socket.");
   pConnection->connect("localhost", Server.port());
   COL_TRC("Begin message loop.");
   Loop.start();
   UNIT_ASSERT_EQUALS(DataIn.size(), DataOut.size());
   COL_DUMP("DataIn :", DataIn.c_str(), DataIn.size());
   COL_DUMP("DataOut:", DataOut.c_str(), DataOut.size());
   if (DataIn != DataOut){
      throw COLerror("Data in does not match data out.  Test has failed.");
   }

   //UNIT_ASSERT_EQUALS(ProgressCount, SIZE_OF_MY_DATA);
}

static void OnConnectWithBadConnection(COLuint64 ConnectionId,bool* pConnected, SCKloop* pLoop){
   COL_FUNCTION(OnConnectWithBadConnection);
   // Can't just UNIT_ASSERT(false) here
   // With Kqueue because writable events can come before read events then the socket becomes briefly connected
   // before it aborts so the assumption in this unit test was wrong.
   *pConnected = true;
   pLoop->shutdown();
}

static void OnBadConnection(COLuint64 ConnectionId, SCKconnection* pConnection, int* pLastError){
   COL_FUNCTION(OnBadConnection);
   *pLastError = pConnection->lastError();
   COL_TRC("Socket error: " << SCKsocketError(*pLastError));
   pConnection->m_pLoop->shutdown();
}

static void OnBadConnectionManuallyTimedOut(bool* pTimedOut, SCKloop* pLoop, SCKconnection* pConnection) {
   COL_FUNCTION(OnBadConnectionManuallyTimedOut);
   *pTimedOut = true;
   pConnection->cleanUp();
   pLoop->shutdown();
}

static void testConnectionRefused(){
   COL_FUNCTION(testConnectionRefused);
   COLthreadPool Pool;
   SCKloop Loop(&Pool);
   SCKconnection* pConnection = new SCKconnection(Loop);
   int LastError = 0;
   bool Connected = false;
   bool TimedOut = false;
   pConnection->m_pOnConnect=COLnewClosure1(&OnConnectWithBadConnection, &Connected, &Loop);
   pConnection->m_pOnClose=COLnewClosure1(&OnBadConnection, pConnection, &LastError);
   SCKaddress Address;
   Address.fromString("127.0.0.1");
   Address.setPort(9900);
   // it should be possible to at least INITIALIZE the connection
   COL_TRC("About to connect.");
   pConnection->connect(Address);
   int TimeoutTimerId = Loop.addTimer(COLnewClosure0(&OnBadConnectionManuallyTimedOut, &TimedOut, &Loop, pConnection), 5000);
   Loop.start();
   COL_TRC("Exited message loop");
   COL_VAR(TimedOut);
   if (!TimedOut) {
      Loop.cancelTimer(TimeoutTimerId);
   }
   UNIT_ASSERT_EQUALS(false, TimedOut);
   UNIT_ASSERT_MESSAGE(!Connected, "Connected to port 9999... pick a different port please");
   if (TimedOut) {
      COL_TRC("Treating this as a connection refused - doesn't seem to be connecting");
   } else {
      //UNIT_ASSERT_EQUALS(SCK_ECONNREFUSED, LastError);
      // Seems hard to predict what we will get with a closed connection
      // TODO we should get a connection refused error  - see IE-1601
   }
}

static void CountDown(int* pCount, SCKloop* pSelect){
   COL_FUNCTION(CountDown);
   (*pCount)--;
   COL_VAR(*pCount);
   if (*pCount <= 0){
      COL_TRC("End countdown.");
      pSelect->shutdown();
   }
}

// This tests the approximate timing of the select timer when there is no socket activity.
static void testEmptySelectTimer(){
   COL_FUNCTION(testEmptySelectTimer);
   COLthreadPool Pool;
   SCKloop Loop(&Pool);
   int Count = 3;
   time_t Start = time(NULL);
   int TimerId = Loop.addTimer(COLnewClosure0(&CountDown, &Count, &Loop), 1000, true);
   Loop.start();
   Loop.cancelTimer(TimerId);
   time_t End = time(NULL);
   COL_VAR2(End-Start, Count);
   UNIT_ASSERT(End-Start >= 1);
}

static void DeleteMeAndStop(COLuint64 ConnectionId, SCKconnection* pConnection){
   COL_FUNCTION(DeleteMeAndStop);
   COL_VAR(pConnection->bufferIn().size());
   UNIT_ASSERT_EQUALS(SIZE_OF_MY_DATA, pConnection->m_TotalRead);
   pConnection->close();
   pConnection->m_pLoop->shutdown();
}

static void ClearInput(COLuint64 ConnectionId, COLstring* pData,  SCKconnection* pConnection){
   COL_FUNCTION(ClearInput);
   COL_VAR2(pData->capacity(), pData->size());
   pData->clear();
   COL_VAR2(pData->capacity(), pData->size());
}

static void MakeBigWrite(COLuint64 ConnectionId, SCKconnection* pConnection){
   COL_FUNCTION(MakeBigWrite);
   pConnection->writeString(COLstring(SIZE_OF_MY_DATA, 'a'));
}

static void DeleteMe(COLuint64 ConnectionId, SCKconnection* pConnection){
   COL_FUNCTION(DeleteMe);
   pConnection->close();
}

static void PoorServer(SCKlistener* pListener){
   COL_FUNCTION(PoorServer);
   int SocketHandle = pListener->accept();
   SCKconnection* pConnection = new SCKconnection(*pListener->loop(), SocketHandle);
   pConnection->m_pOnData=COLnewClosure2(&ClearInput, pConnection);
   pConnection->m_pOnClose=COLnewClosure1(&DeleteMeAndStop, pConnection);
}

static void testConnectWithLargeDataSent(){
   COL_FUNCTION(testConnectWithLargeDataSent);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   SCKlistener Listener(Loop);
   Listener.listen(0);
   Listener.setOnAccept(COLnewClosure0(&PoorServer, &Listener));
   SCKconnection* pConnection = new SCKconnection(Loop);
   pConnection->m_pOnConnect=COLnewClosure1(&MakeBigWrite, pConnection);
   pConnection->m_pOnWritable=COLnewClosure1(&DeleteMe, pConnection);
   pConnection->connect(SCKaddress("127.0.0.1", Listener.port()));
   Loop.start();
}

static void AbortServer(SCKlistener* pListener, int* CountDown){
   COL_FUNCTION(AbortServer);
   int SocketHandle = pListener->accept();
   SCKconnection* pConnection = new SCKconnection(*pListener->loop(), SocketHandle);
   pConnection->m_pOnClose=COLnewClosure1(&OnCloseCountdown, pConnection, CountDown);
}

static void MakeBigWriteAndAbort(COLuint64 ConnectionId, SCKconnection* pConnection){
   COL_FUNCTION(MakeBigWriteAndAbort);
   pConnection->writeString(COLstring(SIZE_OF_MY_DATA, 'a'));
   pConnection->close();
}

static void testAbortedConnect(){
   COL_FUNCTION(testAbortedConnect);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   int CountDown = 2;
   SCKlistener Listener(Loop);
   Listener.listen(0);
   Listener.setOnAccept(COLnewClosure0(&AbortServer, &Listener, &CountDown));
   SCKconnection* pConnection = new SCKconnection(Loop);
   pConnection->m_pOnConnect=COLnewClosure1(&MakeBigWriteAndAbort, pConnection);
   pConnection->connect(SCKaddress("127.0.0.1", Listener.port()));
   pConnection->m_pOnClose=COLnewClosure1(&OnCloseCountdown, pConnection, &CountDown);
   Loop.start();
}

// Write a test to check the address of the accepted socket - DONE.
// TODO - write these tests
// Write a test which uses the OnWritable event to feed in a large buffer.
// Clean out some of SCKutils to be methods on SCKaddress - unit test them as we go.

// check return codes
// test large data on old sockets - compare speed
// test large number of sockets - scale to 512 connections 512 listeners and test maximum connections (maybe look at kqueue end epoll?)

// test socket info (and make this info non static)
// test running SCKdispatchers on multiple threads.
// test having multiple listening sockets.
// WEB test - client and server certificates.
// unify socket objects??
// write a simple LLP server and client - have very little functionality but enough to make Iguana go.
// Support timer events - may or may not be needed.
// Got through socket errors (http://msdn.microsoft.com/en-ca/library/windows/desktop/ms740668%28v=vs.85%29.aspx)
// and figure which out which are relevant to us.  Create and ensure we have consistent behavior.

// get rid of thread dispatching - DONE (new sockets don't
// stop buffering of data (new sockets don't do it). DONE
// write an SCKV6 test (DONE)
// accept implementation - DONE
// wrap iv4 and iv6 structs inside union inside SCKaddress - DONE!
// test bind on unique SCK address - DONE
// SCKaddress.fromString should not reset the port. DONE (although windows side is messyish)
// graceful close with large data - DONE (turns out the shutdown isn't needed)
// Simplify the name lookup stuff - separate out concerns again. DONE


static void SCKTrequestShutdown(COLuint64 Socket, SCKloop* pLoop, SCKlistener* pListener, SCKconnection** ppToClient, SCKconnection** ppToServer) {
   COL_FUNCTION(SCKTrequestShutdown);
   COL_TRC("CLIENT : Requesting server shutdown");
   (*ppToServer)->write("SHUTDOWN", 8);
}

static void SCKTshutDownServer(SCKloop* pLoop, SCKlistener* pListener, SCKconnection** ppToClient, SCKconnection** ppToServer) {
   COL_FUNCTION(SCKTshutDownServer);
   COL_TRC("SERVER : Stop listening");
   pListener->close();
   COL_TRC("SERVER : Close existing client connections");
   (*ppToServer)->hardClose();
   COL_TRC("Client : Close server connection");
   (*ppToClient)->hardClose();
   COL_TRC("SERVER : Stop message loop");
   pLoop->shutdown();
}

static void SCKTrestartServer(SCKloop* pLoop, SCKlistener* pListener, SCKconnection** ppToClient, SCKconnection** ppToServer) {
   COL_FUNCTION(SCKTrestartServer);
   COL_TRC("SERVER : Stop listening");
   pListener->close();
   COL_TRC("SERVER : Close existing client connections");
   (*ppToClient)->hardClose();
   // simulating WEBserver::start()
   COL_TRC("SERVER : Start listening");
   pListener->listen(0);
   // simulating client reconnect
   COL_TRC("Client : Reconnecting to server");
   COL_VAR(*ppToServer);
   *ppToServer = new SCKconnection(*pLoop);
   COL_VAR(*ppToServer);
   (*ppToServer)->m_pOnConnect=COLnewClosure1(&SCKTrequestShutdown, pLoop, pListener, ppToClient, ppToServer);
   (*ppToServer)->connect(SCKaddress("127.0.0.1", pListener->port()));
}

static void SCKTclientRequest(COLuint64 Socket, COLstring* pBuffer, SCKloop* pLoop, SCKlistener* pListener, SCKconnection** ppToClient, SCKconnection** ppToServer) {
   COL_FUNCTION(SCKTclientRequest);
   if ("RESTART" == *pBuffer) {
      COL_TRC("SERVER : Received restart request");
      SCKTrestartServer(pLoop, pListener, ppToClient, ppToServer);
   }
   if ("SHUTDOWN" == *pBuffer) {
      COL_TRC("SERVER : Received shutdown request");
      SCKTshutDownServer(pLoop, pListener, ppToClient, ppToServer);
   }
}

static void SCKTclientConnect(SCKloop* pLoop, SCKlistener* pListener, SCKconnection** ppToClient, SCKconnection** ppToServer) {
   COL_FUNCTION(SCKTclientConnect);
   COL_TRC("SERVER: Client connected");
   int Socket = pListener->accept();
   SCKconnection* pConnection = new SCKconnection(*pLoop, Socket);
   pConnection->m_pOnData = COLnewClosure2(&SCKTclientRequest, pLoop, pListener, ppToClient, ppToServer);
   COL_VAR(*ppToClient);
   *ppToClient = pConnection;
   COL_VAR(*ppToClient);
}

static void SCKTrequestRestart(COLuint64 Socket, SCKconnection** ppToServer) {
   COL_FUNCTION(SCKTrequestRestart);
   COL_TRC("CLIENT : Requesting server restart");
   (*ppToServer)->write("RESTART", 7);
}

static void testRestart() {
   COL_FUNCTION(testRestart);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   SCKconnection* pToClient = NULL;
   SCKlistener Listener(Loop);
   SCKconnection* pToServer = new SCKconnection(Loop);
   COL_TRC("SERVER : Start listening");
   Listener.listen(0);
   Listener.setOnAccept(COLnewClosure0(&SCKTclientConnect, &Loop, &Listener, &pToClient, &pToServer));
   COL_TRC("Client : Connecting to server");
   pToServer->m_pOnConnect = COLnewClosure1(&SCKTrequestRestart, &pToServer);
   pToServer->connect(SCKaddress("127.0.0.1", Listener.port()));
   Loop.start();
}

void testSimpleNetwork(UNITapp* pApp){
  // pApp->add("simple/test/throw", &testPostWithThrow); // This will cause an assert.
   pApp->add("simple/net/bindError", &testListenBindError);
   pApp->add("simple/net/newConnection", &testNewConnect);
   pApp->add("simple/net/newConnectionSCKv6", &testNewConnectSCKv6);
   pApp->add("simple/net/largeWrite", &testLargeWrite);
   pApp->add("simple/net/connectRefused", &testConnectionRefused);
   pApp->add("simple/net/emptySelectTimer", &testEmptySelectTimer);
   pApp->add("simple/net/connectLargeDataSent", &testConnectWithLargeDataSent);
   pApp->add("simple/net/abortedConnect", &testAbortedConnect);
   pApp->add("simple/net/restart", &testRestart);
}
