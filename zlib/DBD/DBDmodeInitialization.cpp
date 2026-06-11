// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDmodeInitialization
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Monday 17 July 2023 - 08:50AM
// ---------------------------------------------------------------------------
#include <ACT/ACTparallelStart.h>
#include <ACT/ACTserial.h>
#include <BLUA/BLUAiguanaEvent.h>
#include <BLUA/BLUAmanager.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLlog.h>
#include <COL/COLutils.h>
#include <CRH/CRHupload.h>
#include <DBD/DBDcheckAccess.h>
#include <DBD/DBDgrid.h>
#include <DBD/DBDmodeInitialization.h>
#include <DBD/DBDmodeNormal.h>
#include <DBD/DBDsetupComponents.h>
#include <DBD/DBDshutdown.h>
#include <DBD/DBDtunnel.h>
#include <DBD/DBDworld.h>
#include <DBIM/DBIMcolumnType.h>
#include <FIL/FILutils.h>
#include <FSB/FSBsep.h>
#include <GITE/GITEbitbucket.h>
#include <GITU/GITUlistLoad.h>
#include <GRID/GRIDnode.h>
#include <IDB/IDBsetup.h>
#include <IWEB/IWEBconfig.h>
#include <NOTI/NOTIdetect.h>
#include <NOTI/NOTImodel.h>
#include <NOTI/NOTIsetup.h>
#include <ROL/ROLload.h>
#include <SDB/SDBcapi.h>
#include <SDM/SDMserver.h>
#include <SDM/SDMshutdown.h>
#include <SSO/SSOsamlPostHandler.h>
#include <TRN/TRNjobQueue.h>
#include <WEB/WEBserver.h>
#include <WEB/WEBsessionHandler.h>
COL_LOG_MODULE;

#define DBD_CORRUPT_LOG_RECOVER_ENV_NAME "DBD_CORRUPT_LOG_RECOVERY"

static bool DBDcheckAccessStartup(DBDworld* pWorld) {
   COL_FUNCTION(DBDcheckAccessStartup);
   if(FILfileExists(DBDaccessFile())) { return true; }
   bool HasAllAccess = true;
   DBDcheckAccess(&HasAllAccess, pWorld->pTunnelStateClient);
   return HasAllAccess;
}

static void DBDmodeInitializationComplete(DBDworld* pWorld) {
   COL_FUNCTION(DBDmodeInitializationComplete);

   COLclosure0* pStopCommand;
   COL_TRC("Setting up proper shutdown");
   auto pShutdownList = new ACTactionSet();
   DBDmodeNormalShutdownList(pWorld, pShutdownList);
   pStopCommand = COLnewClosure0(&DBDbroadcastClusterShutdown, pWorld, pShutdownList);

   if(pWorld->StartupFailed) {
      COLcout << "Start up failed." << newline;
      SDBIlogService("Iguana service initialization encountered an error.", "#error #startup");
      pStopCommand->runAndDelete();
      return;
   }

   //   if(!DBDcheckAccessStartup(pWorld)) { return pStopCommand->runAndDelete(); }

   SDMshutdownSet(pStopCommand);
   COLcout << "Control+C to exit." << newline;
   COL_TRC("Set stop");

   SDBIlogService("Iguana has been initialized.", "#startup");

   COL_TRC("About to change mode to normal mode");
   if(getenv("DBD_SLOW_START")) {
      COLcout << "Warning DBD_SLOW_START defined so we are starting slowly!" << newline;
      pWorld->pLoop->addTimer(COLnewClosure0(&DBDmodeNormal, pWorld), 20000);
   } else {
      DBDmodeNormal(pWorld);
   }
   COL_CRASH_HERE_IF_ENV_VAR_IS_SET();
   COL_ABORT_HERE_IF_ENV_VAR_IS_SET();
}

static void DBDserialStartupInitialization(const COLstring& Error, DBDworld* pWorld) {
   COL_FUNCTION(DBDserialStartupInitialization);
   COL_VAR(Error);
   if(!Error.is_null()) {
      COL_TRC("Error raised during init so stopping.");
      pWorld->ErrorLog  = Error;
      pWorld->InitError = true;
      COL_TRC("About to call shutdown on the loop");
      DBDshutdown(pWorld);
      pWorld->pLoop->shutdown();
      COL_TRC("Returning");
      return;
   }
   CFGremoveDeletedSources(pWorld->pComponentMap);
   COL_TRC("Setup notifiction timer and log system callback");
   SDBregisterNotificationCallback(pWorld->pLogQueue, COLnewClosure1(&NOTIlogContainsPattern, pWorld->pComponentMap,
                                                                     pWorld->pNotifications, pWorld->pBluaManager));
   COLauto<COLclosure0> Detect =
       COLnewClosure0(&NOTIdetectPoll, pWorld->pComponentMap, pWorld->pNotifications, pWorld->pBluaManager);
   pWorld->pNotifications->Timer.start(Detect, 60000, true);
   auto pSet = new ACTactionSet();
   pSet->List.add(COLnewClosure1<COLclosure0*>(&SDBIstartLogQueue, pWorld->pLogQueue));
   // pSet->List.add(COLnewClosure1<COLclosure0*>(&BLUAiguanaEvent, pWorld->pLoop, pWorld->pComponentMap, STARTUP));
   // pSet->List.add(COLnewClosure1<COLclosure0*>(&BLUAstartComponents, pWorld->pLoop, pWorld->pComponentMap, pWorld->pBluaManager));
   pSet->pDone = COLnewClosure0(&DBDmodeInitializationComplete, pWorld);
   COL_TRC("Scheduling serial start up.");
   ACTserial(pSet, pWorld->pLoop);
}

static void DBDmodeInitializationApi(DBDworld* pWorld) {
   COL_FUNCTION(DBDmodeInitializationApi);
   pWorld->pWebServer->m_RequireAuthentication = false;  // No login required for initialization mode.
   pWorld->pWebServer->m_MainThreadHandlers.add("/mode", COLnewClosure1(&DBDmodeWeb, pWorld));
   pWorld->pWebServer->m_MainThreadHandlers.add("/file_browser/sep", COLnewClosure1(&FSBfileBrowserSep));
}

static void DBDsamlInit(WEBserver* pWebServer, COLmap<COLstring, USRuser>* pUserMap, SCKloop* pLoop) {
   pWebServer->m_pSamlPostHandler = COLnewClosure1(&SSOsamlPostHandler, pUserMap, &pWebServer->m_SessionMap, pLoop);
}

void DBDmodeInitialization(DBDworld* pWorld) {
   COL_FUNCTION(DBDmodeInitialization);
   pWorld->Mode = DBD_MODE::INITIALIZATON;
   COL_TRC("Setting up INITIALIZATION mode api");
   DBDmodeInitializationApi(pWorld);
   DBDtunnelInit(pWorld->pWebServer, pWorld->pTunnelStateClient, pWorld->pLoop);  // This really needs to be on the main thread.
   DBDsamlInit(pWorld->pWebServer, pWorld->pUserMap, pWorld->pLoop);
   COL_TRC("Creating parallel start action list");
   COLlist<COLclosure0*> Actions;
   Actions.add(COLnewClosure0(&IDBsetup,           pWorld->pDatabaseConfig, pWorld->pDatabaseFactory, pWorld->pLoop));
   Actions.add(COLnewClosure0(&ROLload,            pWorld->pRoleMap));
   Actions.add(COLnewClosure0(&NOTIinit,           pWorld->pNotifications));
   Actions.add(COLnewClosure0(&DBIMinitTypeLookup));
   Actions.add(COLnewClosure0(&DBDsetupComponents, pWorld->pComponentMap, pWorld->pWebConfig));
   Actions.add(COLnewClosure0(&DBDgridInit,        pWorld->pWebServer, pWorld->pWebConfig, &pWorld->pGridNode, pWorld->pLoop));

   COL_TRC("About to launch parallel start.");
   ACTparallelStart(pWorld->pLoop, &Actions, COLnewClosure1(&DBDserialStartupInitialization, pWorld));

   COL_TRC("Initializing the catalogue file if it does not exist.");
   GITUdefaultCatalogueOnStartUp();
}