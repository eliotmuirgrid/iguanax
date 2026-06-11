//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAsleep
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Friday May 14 2021
//---------------------------------------------------------------------------
#include "BLUAsleep.h"

#include <BLUA/BLUAinstance.h>
#include <COL/COLlog.h>
#include <LUA/LUAawait.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void BLUAinstanceSleepWakeUp(std::weak_ptr<struct LUAsynchronize> pAsync, BLUAinstance2* pInstance) {
   COL_FUNCTION(BLUAinstanceSleepWakeUp);
   COL_TRC("Waking up...");
   LUAasync(pAsync);
   pInstance->m_SleepTimer = 0;
}

void BLUAinstanceTearDownSleep(BLUAinstance2* pInstance) {
   COL_FUNCTION(BLUAinstanceTearDownSleep);
   if (pInstance->m_SleepTimer) {
      COL_TRC("Wake up now for component shutdown");
      pInstance->m_pLoop->cancelTimer(pInstance->m_SleepTimer);
      pInstance->m_SleepTimer = 0;
      pInstance->m_ForceKill = true;
   } else {
      COL_TRC("No sleep timer.");
   }
}

static void BLUAinstanceSleepOnMainThread(std::weak_ptr<struct LUAsynchronize> pAsync, BLUAinstance2* pInstance, int Length) {
   COL_FUNCTION(BLUAinstanceSleepOnMainThread);
   if (pInstance->m_SleepTimer != 0){
      pInstance->m_pLoop->cancelTimer(pInstance->m_SleepTimer);
   }
   pInstance->m_SleepTimer = pInstance->m_pLoop->addTimer(COLnewClosure0(&BLUAinstanceSleepWakeUp, pAsync, pInstance), Length);
}

void BLUAinstanceSleep(BLUAinstance2* pInstance, int Length) {
   COL_FUNCTION(BLUAinstanceSleep);
   COL_TRC("Falling asleep...");
   COLclosure1<std::weak_ptr<struct LUAsynchronize>>* pFunction = COLnewClosure1(&BLUAinstanceSleepOnMainThread, pInstance, Length);
   bool Success = LUAawait(pFunction, pInstance->m_pLoop, pInstance->m_ForceKill);
   if (!Success) {
      // TODO: report timeout?
      COL_WRN("LUAwait() timed out.");
   }
}
