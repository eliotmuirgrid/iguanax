//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testIocp
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thu  4 Dec 2014 10:38:42 CST
//---------------------------------------------------------------------------

#include "testSet2.h"

#include <UNIT/UNITapp.h>

#include <SCK/SCKloop.h>
#include <SCK/SCKconnection.h>
#include <SCK/SCKlistener.h>
#include <SCK/SCKaddress.h>
#include <SCK/SCKerror.h>

#include <COL/COLthreadPool.h>
#include <COL/COLclosure.h>
#include <COL/COLthreadQueue.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testListen(){
   COL_FUNCTION(testListen);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   SCKlistener Server(Loop);
   Server.listen(0);
   COLASSERT(0 != Server.port());
}

static void Echo(COLuint64 ConnectionId, COLstring* pData, SCKconnection* pConnection){
   COL_FUNCTION(Echo);
   COL_DBG(*pData);
   COLstring Buffer(*pData);
   pData->clear();
   pConnection->writeString(Buffer);
}

static void StopLoop(COLuint64 ConnectionId, SCKconnection* pConnection){
   COL_FUNCTION(StopLoop);
   pConnection->m_pLoop->shutdown();
}


static void OnNewEchoConnection(SCKlistener* pServer){
   COL_FUNCTION(OnNewEchoConnection);
   SCKaddress RemoteAddress;
   SCKconnection* pNewConnection = new SCKconnection(*pServer->loop(), pServer->accept(&RemoteAddress));
   COL_VAR(pNewConnection->m_Handle);
   pNewConnection->m_pOnData = COLnewClosure2(&Echo, pNewConnection);
   pNewConnection->m_pOnClose = COLnewClosure1(&StopLoop, pNewConnection);
}

static void EchoCount(COLuint64 ConnectionId, COLstring* pData, SCKconnection* pConnection, int* pCount){
   COL_FUNCTION(EchoCount);
   (*pCount)--;
   COL_TRC("Got " << pData->size() << " bytes.");
   COL_DBG(*pData);
   if (npos != pData->find("down: 1\n")){
      pConnection->close();
   }
   pData->clear();

   if (*pCount > 0){
      pConnection->writeString(COLstring("Count down: ") + COLintToString(*pCount) + "\n");
   }
}

static void Connected(COLuint64 ConnectionId, SCKaddress* pAddress){
   COL_FUNCTION(Connected);
   COL_TRC("Connected to: " << *pAddress);
}

static void Close(COLuint64 ConnectionId, SCKaddress* pAddress){
   COL_FUNCTION(Close);
   COL_TRC("Closed connection to: " << *pAddress);
}

static void testConnection(){
   COL_FUNCTION(testConnection);
   int Count = 10;
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   SCKlistener Server(Loop);
   Server.listen(0);
   Server.setOnAccept(COLnewClosure0(&OnNewEchoConnection, &Server));

   SCKconnection* pConnection = new SCKconnection(Loop);
   SCKaddress Address;
   Address.fromString("127.0.0.1");
   Address.setPort(Server.port());
   pConnection->writeString("START COUNTDOWN!");
   pConnection->m_pOnConnect=COLnewClosure1(&Connected, &Address);
   pConnection->m_pOnData   =COLnewClosure2(&EchoCount, pConnection, &Count);
   pConnection->m_pOnClose  =COLnewClosure1(&Close, &Address);
   pConnection->connect(Address);
   COL_VAR(pConnection->m_Handle);
   Loop.start();
}

static void CountDown(int* pCount, SCKloop* pLoop){
   COL_FUNCTION(CountDown);
   (*pCount)--;
   COL_VAR(*pCount);
   if (*pCount == 0){
      pLoop->shutdown();
   }
}

static void testTick(){
   COL_FUNCTION(testTick);
   int Count = 3;
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   int TimerId = Loop.addTimer(COLnewClosure0(&CountDown, &Count, &Loop), 1, true);
   Loop.start();
   Loop.cancelTimer(TimerId);
   UNIT_ASSERT_EQUALS(0, Count);
}

static void EndLoop(SCKloop* pLoop){
   COL_FUNCTION(EndLoop);
   pLoop->shutdown();
}

static void CallMainThread(SCKloop* pLoop){
   COL_FUNCTION(CallMainThread);
   pLoop->post(COLnewClosure0(&EndLoop, pLoop));
}

static void testThreadMessage(){
   COL_FUNCTION(testThreadMessage);
   COLthreadPool Pool(1,10);
   SCKloop Loop(&Pool);
   Pool.start();
   Pool.scheduleTask(COLnewClosure0(&CallMainThread, &Loop));
   Loop.start();
}

static void StopLoopAndPrintError(COLuint64 ConnectionId, SCKconnection* pConnection, bool* HardClose){
   COL_FUNCTION(StopLoopAndPrintError);
   COL_TRC("Error code: " << pConnection->lastError() << ", Text: " << SCKsocketError(pConnection->lastError()));
   if (pConnection->lastError() == SCK_ECONNRESET || pConnection->lastError() == SCK_EPIPE || pConnection->lastError() == SCK_ECONNABORTED){
      *HardClose = true;
   } else {
      COL_WRN("We did not get SCK_ECONNRESET= " << SCK_ECONNRESET << " or SCK_EPIPE = " << SCK_EPIPE << " or SCK_ECONNABORTED = " << SCK_ECONNABORTED);
   }
   pConnection->m_pLoop->shutdown();
}

static void OnNewEchoConnectionError(SCKlistener* pServer, bool* HardClose){
   COL_FUNCTION(OnNewEchoConnection);
   SCKaddress RemoteAddress;
   SCKconnection* pNewConnection = new SCKconnection(*pServer->loop(), pServer->accept(&RemoteAddress));
   pNewConnection->m_pOnData = COLnewClosure2(&Echo, pNewConnection);
   pNewConnection->m_pOnClose = COLnewClosure1(&StopLoopAndPrintError, pNewConnection, HardClose);
}

static void hardCloseConnection(COLuint64 ConnectionId, COLstring* pData, SCKconnection* pConnection){
   COL_FUNCTION(hardCloseConnection);
   COL_DBG(*pData);
   pConnection->hardClose();
}

static void testHardClose(){
   COL_FUNCTION(testHardClose);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   SCKlistener Server(Loop);
   bool HardClose = false;
   Server.listen(0);
   Server.setOnAccept(COLnewClosure0(&OnNewEchoConnectionError, &Server, &HardClose));
   SCKconnection* pConnection = new SCKconnection(Loop);
   SCKaddress Address;
   Address.fromString("127.0.0.1");
   Address.setPort(Server.port());
   int DataSize = 100000;
   COLstring Out;
   Out.setCapacity(DataSize);
   Out.setSize(DataSize);
   for (int i=0; i < DataSize ; i++){
      Out[i] = '0' + (i % 10);
   }
   pConnection->writeString(Out);
   pConnection->m_pOnData = COLnewClosure2(&hardCloseConnection, pConnection);
   pConnection->connect(Address);
   COL_TRC("About to start loop.");
   Loop.start();
   UNIT_ASSERT(HardClose);
}

// TODO - write a test where we don't write data before the connection.

void testSet2(UNITapp* pApp){
   pApp->add("set2/listen",         testListen);
   pApp->add("set2/connection",     testConnection);
   pApp->add("set2/tick",           testTick);
   pApp->add("set2/threadMessasge", testThreadMessage);
   pApp->add("set2/hardClose",      testHardClose);
}
