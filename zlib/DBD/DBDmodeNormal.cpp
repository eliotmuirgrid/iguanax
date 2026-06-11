//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DBDmodeNormal.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   5/2/23 10:04 AM
//  ---------------------------------------------------------------------------
#include <ACT/ACTparallelStart.h>
#include <ACT/ACTserial.h>
#include <BLUA/BLUAautoStart.h>
#include <BLUA/BLUAiguanaEvent.h>
#include <BLUA/BLUAmanager.h>
#include <COL/COLlog.h>
#include <COL/COLutils.h>
#include <DBD/DBDapi.h>
#include <DBD/DBDmode.h>
#include <DBD/DBDmodeNormal.h>
#include <DBD/DBDsetupComponents.h>
#include <DBD/DBDshutdown.h>
#include <DBD/DBDworld.h>
#include <SDB/SDBcapi.h>
#include <SDB/SDBqueueCount.h>
#include <TRN/TRNjobQueue.h>
#include <WEB/WEBserver.h>
#include <WEB/WEBsessionHandler.h>
COL_LOG_MODULE;

#define DBD_ENV_NAME_IGUAN_EVENT_SCRIPT   "IFW_ENABLE_GLOBAL_EVENTS"

void DBDmodeNormalShutdownList(DBDworld* pWorld, ACTactionSet* pShutDownSeries) {
   COL_FUNCTION(DBDmodeNormalShutdownList);
   pShutDownSeries->List.add(COLnewClosure1<COLclosure0* >(&TRNshutdownAllJobs,  pWorld->pTranslatorJobQueues, pWorld->pLoop));
   pShutDownSeries->List.add(COLnewClosure1<COLclosure0* >(&BLUAinstanceStopAll, pWorld->pBluaManager));
   if (getenv(DBD_ENV_NAME_IGUAN_EVENT_SCRIPT)) {
      pShutDownSeries->List.add(COLnewClosure1<COLclosure0* >(&BLUAiguanaEvent,     pWorld->pLoop, pWorld->pComponentMap, SHUTDOWN));
   }
   pShutDownSeries->List.add(COLnewClosure1<COLclosure0* >(&SDBIstopLogQueue,    pWorld->pLogQueue));
}

static void DBDmodeFinalizeNormalApi(DBDworld* pWorld){
   COL_FUNCTION(DBDmodeFinalizeNormalApi);
   COL_TRC("Setting up NORMAL mode api and enabling authentication checking");
   pWorld->pWebServer->m_RequireAuthentication = true;
   DBDapi(pWorld);
}

static void DBDmodeNormalComponentsDone(DBDworld* pWorld){
   COL_FUNCTION(DBDmodeNormalComponentsDone);
   COL_TRC("Everything set up, startup scripts finished running and autostart components have been started. Change mode to normal");
   pWorld->Mode = DBD_MODE::NORMAL;
}


void DBDmodeNormal(DBDworld* pWorld){
   COL_FUNCTION(DBDmodeNormal);
   COL_TRC("Switching to NORMAL mode");
   DBDmodeFinalizeNormalApi(pWorld);
   COL_TRC("Schedule autostart components");
   auto pSet = new ACTactionSet();
   // Doing this here in case the component scripts want to call Iguana api endpoints. Most endpoints are not setup
   // until DBDmodeFinalizeNormalApi(pWorld) is called above.
   if (getenv(DBD_ENV_NAME_IGUAN_EVENT_SCRIPT)) {
      pSet->List.add(COLnewClosure1<COLclosure0*>(&BLUAiguanaEvent,     pWorld->pLoop, pWorld->pComponentMap, STARTUP));
   }
   pSet->List.add(COLnewClosure1<COLclosure0*>(&BLUAstartComponents, pWorld->pLoop, pWorld->pComponentMap, pWorld->pBluaManager));
   pSet->pDone = COLnewClosure0(&DBDmodeNormalComponentsDone, pWorld);
   COL_TRC("Scheduling serial normal mode start up.");
   ACTserial(pSet, pWorld->pLoop);
}
