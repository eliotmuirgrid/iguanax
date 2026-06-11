// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testRestart
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Tuesday 29 August 2023 - 11:53AM
// ---------------------------------------------------------------------------
#include "testRestart.h"


#include <SCK/SCKloop.h>
#include <WEB/WEBserver.h>
#include <WEB/WEBresponse.h>
#include <WEB/WEBmainThread.h>

#include <UNIT/UNITapp.h>

#include <COL/COLweb.h>
#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void restartServer(WEBserver* pServer, int Port){
   COL_FUNCTION(restartServer);
   pServer->shutdown();
   pServer->start(Port);
}

static COLstring testSimpleResponse(const COLstring& Data){
   COLstring Result;
   Result = COL_T("HTTP/1.1 200 OK\r\n")
             + COL_T("Content-Type: text/html\r\n")                          
             + COL_T("Content-Length: ") + COLintToString(Data.size()) + COL_T("\r\n\r\n") + Data;
   return Result;
}

static void testIndex(const COLwebRequest& Request, SCKloop* pLoop, WEBserver* pServer){
   COL_FUNCTION(testIndex);
   int NewPort = pServer->m_pListener->port();
   if (NewPort == 8080){
      NewPort = 8081;
   } else {
      NewPort = 8080;
   }
   COL_VAR(NewPort);
   COLstring Response = testSimpleResponse("<a href='http://localhost:"+ COLintToString(NewPort)+"/index.html'>New port to go.</a>");
   COLsendWebResponse(Request.Address.ConnectionId, Response);

   pLoop->addTimer(COLnewClosure0(restartServer, pServer, NewPort), 1000);
}

static void testSimpleRestart(){
   COL_FUNCTION(testSimpleRestart);
   COLthreadPool Pool(1,100);
   SCKloop Loop(&Pool);
   WEBserver Server(Loop);
   COLwebSetResponseCallback(COLnewClosure2(&WEBresponseSend));
   WEBresponseSetServer(&Server);
   WEBsetMainThread();
   
   Server.m_RequireAuthentication = false;
   Server.m_MainThreadHandlers.add("/index.html", COLnewClosure1(&testIndex, &Loop, &Server));
   Server.start(8080);
   Pool.start();
   Loop.start();
   Server.shutdown();
}

void testRestart(UNITapp* pApp) {
   //pApp->add("restart", &testSimpleRestart);
}
