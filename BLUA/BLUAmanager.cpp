//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:
//
// Description:
//
// Implementation
//
// Author: Nicolas Lehman
// Date:   Friday July 16 2021
//---------------------------------------------------------------------------
#include "BLUAinstance.h"
#include "BLUAmanager.h"
#include "BLUAstop.h"

#include <SDB/SDBcapi.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool BLUAallComponentsStopped(const BLUAinstanceManager& Manager) {
   COL_FUNCTION(BLUAallComponentsStopped);
   return Manager.m_Instances.size() == 0;
}

bool BLUAinstanceRunning(const BLUAinstanceManager& Manager, const COLstring& ComponentId) {
   COL_FUNCTION(BLUAinstanceRunning);
   return Manager.m_Instances.count(ComponentId) != 0;
}

static void BLUAinstanceStopAllImpl(COLclosure0* pCallback, BLUAinstanceManager* pManager, COLmap<COLstring, COLauto<BLUAinstance2>>& Instances){
   COL_FUNCTION(BLUAinstanceStopAllImpl);
   if (pManager->m_ShutdownRequested) {
      COL_TRC("Shutdown already in progress");
      return;
   }
   COL_WRN("TODO: ugly shutdown logic because instance shutdown is async and removes itself from m_Instances map on complete.");
   pManager->m_ShutdownRequested = true;
   pManager->m_pOnShutdown = pCallback;
   COLmap<COLstring, COLauto<BLUAinstance2>>::iterator Itr;
   for (Itr = Instances.begin(); Itr != Instances.end(); Itr++) {
      BLUAinstanceDelete(Itr->second);
   }
}

// TODO - probably belongs in it's own CPP since it's part of shutdown logic
void BLUAinstanceStopAll(COLclosure0* pCallback, BLUAinstanceManager* pManager) {
   COL_FUNCTION(BLUAinstanceStopAll);
   SDBIlogService("Stopping components...", "#shutdown");
   COLmap<COLstring, COLauto<BLUAinstance2>>& Instances = pManager->m_Instances;
   if (Instances.size() == 0) {
      COL_TRC("No components to shutdown");
      pCallback->runAndDelete();
      return;
   }
   BLUAinstanceStopAllImpl(pCallback, pManager, Instances);
}

// This one doesn't cleanup the callback. This version is run from the licensing system when an expired license
// is detected to stop all running components.
void BLUAinstanceStopAllWithoutCleanup(COLclosure0* pCallback, BLUAinstanceManager* pManager) {
   COL_FUNCTION(BLUAinstanceStopAllWithoutCleanup);
   COLmap<COLstring, COLauto<BLUAinstance2>>& Instances = pManager->m_Instances;
   if (Instances.size() == 0) {
      COL_TRC("No components to shutdown");
      pCallback->run();
      return;
   }
   BLUAinstanceStopAllImpl(pCallback, pManager, Instances);
}
