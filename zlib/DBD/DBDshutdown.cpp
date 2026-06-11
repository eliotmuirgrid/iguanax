//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDshutdown
//
// Author: Eliot Muir
//
// Description:
//
// Simple functional code to handle shutting down IguanaX cleanly.
//---------------------------------------------------------------------------
#include <ACT/ACTserial.h>
#include <BLUA/BLUAmanager.h>
#include <CACHE/CACHEfile.h>
#include <COL/COLhashmap.h>
#include <COL/COLioCheck.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <DBD/DBDshutdown.h>
#include <DBD/DBDtunnel.h>
#include <DBD/DBDworld.h>
#include <ICOL/ICOLcomponent.h>
#include <ICOL/ICOLmap.h>
#include <NOTI/NOTImodel.h>
#include <PRO/PROkill.h>
#include <RGN/RGNmachineId.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
#include <SDM/SDMrestart.h>
#include <SDM/SDMserver.h>
#include <SDM/SDMshutdown.h>
#include <SEC/SECaccess.h>
#include <TRN/TRNjobQueue.h>
#include <WEB/WEBserver.h>
COL_LOG_MODULE;

// The idea here is to do non-blocking shutdown operations which can happen immediately.
void DBDshutdown(DBDworld* pWorld) {
   COL_FUNCTION(DBDshutdown);
   COL_TRC("Clear webserver handlers to stop accepting new requests");
   WEBserverClearHandlers(pWorld->pWebServer);
   pWorld->pWebServer->shutdown();   // TODO - could be dangerous with grid still running.
   pWorld->pNotifications->Timer.stop();
   DBDtunnelShutdown(pWorld->pTunnelStateClient);
}

static void DBDstopEventLoop(DBDworld* pWorld, SCKloop* pLoop) {
   COL_FUNCTION(DBDstopEventLoop);
   SECfreePointers();
   ICOLfree();
   ICOLcomponentFree();
   CACHEfreeCache();
   COLclearMainThread();
   SDBIlogService("Stopping the event loop...", "#shutdown");
   COLcout << "Stopping the event loop..." << newline;
   pLoop->threadPool()->shutdown(COLthreadPool::shutdown_immediate);
   pLoop->shutdown();
   // TODO: find a way to web server and sckloop shutdown in logging system.
   SDMrestartIfRequested();
}

void DBDterminateApp(DBDworld* pWorld, ACTactionSet* pShutDownSeries) {
   COL_FUNCTION(DBDterminateApp);
   DBDshutdown(pWorld);
   pShutDownSeries->pDone = COLnewClosure0(&DBDstopEventLoop, pWorld, pWorld->pLoop);
   ACTserial(pShutDownSeries, pWorld->pLoop);
}

void DBDbroadcastClusterShutdown(DBDworld* pWorld, ACTactionSet* pShutDownSeries) {
   COL_FUNCTION(DBDbroadcastClusterShutdown);
   SDMshutdownSet(NULL); // Don't allow double shutdowns
   // SDBIlogService("Stop signal received. Shutting down.", "#shutdown");
   COLcout << "Stop signal received. Shutting down." << newline;
   SDBIlogService("Stop signal received. Shutting down...", "#shutdown");
   // TODO: call addTimer from main thread.
   pWorld->pLoop->post(COLnewClosure0(&DBDterminateApp, pWorld, pShutDownSeries));
}
