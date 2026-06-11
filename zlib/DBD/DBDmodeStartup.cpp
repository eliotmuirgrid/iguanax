//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DBDmodeStartup.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   5/2/23 10:04 AM
//  ---------------------------------------------------------------------------
#include <DBD/DBDmodeStartup.h>

#include <DBD/DBDmodeApi.h>
#include <DBD/DBDserverApi.h>
#include <DBD/DBDsetupComponents.h>
#include <DBD/DBDshutdown.h>
#include <DBD/DBDworld.h>
#include <DBD/DBDmodeNormalStart.h>
#include <DBD/DBDextAuth.h>

#include <DBDVER/DBDVERapi.h>

#include <DIR/DIRpath.h>

#include <PRO/PROwhich.h>

#include <ACT/ACTserial.h>
#include <BLUA/BLUAmanager.h>

#include <GITE/GITEbitbucket.h>
#include <GITS/GITSapi.h>

#include <INST/INSTapi.h>
#include <INST/INSTstart.h>
#include <INST/INSTstop.h>

#include <SDM/SDMserver.h>
#include <SDM/SDMshutdown.h>
#include <SDM/SDMrestart.h>
#include <SDM/SDMmanage.h>

#include <USR/USRapi.h>

#include <FSB/FSBapi.h>

#include <FIL/FILutils.h>

#include <TRN/TRNjobQueue.h>

#include <WEB/WEBserver.h>
#include <WEB/WEBapi.h>

#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void DBDstartupShutdown(DBDworld* pWorld) {
   COL_FUNCTION(DBDstartupShutdown);
   SDMshutdownSet(NULL); // Don't allow double shutdowns

   // SDBIlogService("Stop signal received. Shutting down.", "#shutdown");
   COLcout << "Stop signal received. Shutting down." << newline;
   WEBserverClearHandlers(pWorld->pWebServer);
   pWorld->pWebServer->shutdown();
   pWorld->pLoop->threadPool()->shutdown(COLthreadPool::shutdown_immediate);
   pWorld->pLoop->shutdown();
   SDMrestartIfRequested();
}

static void DBDmodeStartupComplete(DBDworld* pWorld) {
   COL_FUNCTION(DBDmodeStartupComplete);
   COL_TRC("Setting up proper shutdown");
   SDMshutdownSet(COLnewClosure0(&DBDstartupShutdown, pWorld));
   INSTstartup(*pWorld->pWebConfig);
}

void DBDmodeStartup(DBDworld* pWorld) {
   COL_FUNCTION(DBDmodeStartup);
   pWorld->Mode = DBD_MODE::INSTALLER;
   pWorld->pWebServer->m_RequireAuthentication = false;  // No login required for setup mode.
   WEBserverClearHandlers(pWorld->pWebServer);
   GITSapi(pWorld->pWebServer->m_WorkerThreadHandlers);
   DBDmodeApi(pWorld->pWebServer->m_MainThreadHandlers, pWorld);
   USRapiStartup(pWorld->pWebServer->m_MainThreadHandlers, pWorld->pLoop, pWorld->pUserMap);
   DBDVERapi(pWorld->pWebServer->m_MainThreadHandlers);
   DBDserverApi(pWorld->pWebServer->m_MainThreadHandlers, pWorld->pWebConfig);
   INSTapi(pWorld->pWebServer->m_WorkerThreadHandlers);
   INSTapiAsync(pWorld->pWebServer->m_MainThreadHandlers, pWorld->pServer, pWorld->pLoop);
   FSBapi(pWorld->pWebServer->m_WorkerThreadHandlers);
   WEBapi(pWorld->pWebServer->m_MainThreadHandlers, &pWorld->pWebServer->m_SessionMap, pWorld->pUserMap, pWorld->pExtAuthenticator);  // TODO should be able to remove this.
   DBDmodeStartupComplete(pWorld);
}
