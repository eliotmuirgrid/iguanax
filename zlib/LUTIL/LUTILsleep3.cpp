// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUTILsleep3
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 19 June 2023 - 10:08AM
// ---------------------------------------------------------------------------

#include <LUTIL/LUTILsleep3.h>

#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAsleep.h>
#include <LUA/LUAcheck.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAutils.h>
#include <LUAL/LUALutils.h>
#include <SCK/SCKloop.h>
#include <COL/COLthreadPool.h>
#include <TRN/TRNannotationState.h>

#include <COL/COLevent.h>
#include <COL/COLutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void LUTILsleepDone(std::weak_ptr<COLevent> pEvent){
   COL_FUNCTION(LUTILsleepDone);
   if (std::shared_ptr<COLevent> pReal = pEvent.lock()) {
      COL_TRC("Sleep timer done. Signal worker thread");
      pReal->signal();
   }
}

static void LUTILsetupSleepTimer(SCKloop* pLoop, int TimeToSleep, COLclosure0* pSleepJob, int* pSleepTimerId){
   COL_FUNCTION(LUTILsetupSleepTimer);
   *pSleepTimerId = pLoop->addTimer(pSleepJob, TimeToSleep);
   COL_TRC("Set up a sleep timer for " << TimeToSleep << " ms with id " << *pSleepTimerId);
}

static void LUTILcancelSleepTimer(SCKloop* pLoop, int SleepTimerId){
   COL_FUNCTION(LUTILcancelSleepTimer);
   if(pLoop->isSet(SleepTimerId)){
      COL_TRC("Cancelling sleep timer, id: " << SleepTimerId);
      pLoop->cancelTimer(SleepTimerId);
   }
}

static void LUTILsleepImpl(lua_State* L, SCKloop* pLoop, int TimeToSleep, std::weak_ptr<COLevent> pEvent){
   COL_FUNCTION(LUTILsleepImpl);
   // Setup a "sleep" as a timer event
   int SleepTimerId = 0;
   COLclosure0* pOnSleepDone = COLnewClosure0(&LUTILsleepDone, pEvent);
   pLoop->post(COLnewClosure0(&LUTILsetupSleepTimer, pLoop, TimeToSleep, pOnSleepDone, &SleepTimerId));
   // Wait for the event to be signaled from sleep completion
   COL_TRC("Now waiting on lua thread for sleep to be done.");
   if (std::shared_ptr<COLevent> pReal = pEvent.lock()) {
      pReal->wait();
   }
   pLoop->post(COLnewClosure0(&LUTILcancelSleepTimer, pLoop, SleepTimerId)); // need this to make sure timers are cleaned up
   COL_TRC("Lua thread received signal to resume");
}

int LUTILsleep3(lua_State* L){
   COL_FUNCTION(LUTILsleep3);
   LUAcheckArgCount(L, 1, -1);
   int TimeToSleep = lua_tointeger(L, 1);
   if (TimeToSleep < 0){
      LUA_ERROR_STREAM(COL_T("sleep must be given a positive argument."));
   }
   SCKloop* pLoop = (SCKloop*)LUAgetPointerField(L, "_pEventLoop");
   int SleepTimerId = 0;
   if (LUALisEnvironmentStartup(L)) { return 0; }
   COL_VAR(LUALisEnvironmentTest(L));
   if (LUALisEnvironmentTest(L)){
      COL_TRC("Running translator sleep");
      TRNannotationState* pAnnotationState = TRNgetAnnotationState(L);
      LUTILsleepImpl(L, pLoop, TimeToSleep, pAnnotationState->m_pEvent);
      COL_VAR(pAnnotationState->m_Cancelled);
      if (pAnnotationState->m_Cancelled) { LUAraiseLuaError(L, "Connection to browser has gone away, abandon annotation job"); }
   } else {
      COL_TRC("Running headless sleep");
      BLUAinstance2* pInstance = BLUAinstanceGet(L);
      if (pInstance) {
         COL_TRC("Sleep with timer...");
         BLUAinstanceSleep(pInstance, TimeToSleep);
         return 0;
      }
   }
   // resume lua execution
   return 0;
}