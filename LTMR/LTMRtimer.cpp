// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LTMRtimer
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Tuesday 23 May 2023 - 12:59PM
// ---------------------------------------------------------------------------
#include <LTMR/LTMRtimer.h>

#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAtimer.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAopen.h>

#include <SCK/SCKloop.h>

#include <COL/COLclosure6.h>
#include <COL/COLevent.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


static void LTMRtimerExpired(BLUAinstance2* pInstance, COLauto<COLstring> pData, int Callback, COLauto<int> pTimerId) {
   COL_FUNCTION(LTMRtimerExpired);
   COL_VAR(*pTimerId);
   pInstance->m_JobQueue.put(BLUAtimerTask(pData, Callback));
   pInstance->m_TimerMap.removeKey(pTimerId);
}

// MAIN THREAD
static void LTMRsetTimerOnMain(BLUAinstance2* pInstance, int Delay, COLauto<COLstring> pData, int Callback, COLevent* pEvent, int* pTimerId) {
   COL_FUNCTION(LTMRsetTimerOnMain);
   COLauto<int> pTimer = new int();
   int TimerId = pInstance->m_pLoop->addTimer(COLnewClosure0(&LTMRtimerExpired, pInstance, pData, Callback, pTimer), Delay);
   *pTimer.get() = TimerId;
   pInstance->m_TimerMap[TimerId] = true;
   COL_VAR2(TimerId, pInstance->m_TimerMap.size());
   *pTimerId = TimerId;
   COL_TRC("Signal LTMRsetTimer() to continue");
   pEvent->signal();
}

// LUA THREAD
int LTMRsetTimer(lua_State* L) {
   COL_FUNCTION(LTMRsetTimer);
   COL_VAR(LUAcomponentId(L));
   COLauto<COLstring> pData = new COLstring(LUAoptionalStringParam(L, 1, "data"));
   int Delay = LUAexpectedIntParam(L, 1, "delay");
   if (!LUAoptionalFuncParam(L, 1, "callback")) {
      COL_TRC("No function name provided, defaulting to main");
      lua_getglobal(L, "main");
   }
   int FcnRef = luaL_ref(L, LUA_REGISTRYINDEX);
   COL_VAR3(*pData, Delay, FcnRef);
   if (LUALisEnvironmentTest(L) || LUALisEnvironmentStartup(L)) { // this is a no-op in IDE mode, just return a dummy id
      COL_TRC("This is IDE mode - return a dummy id");
      lua_pushnumber(L, 0);
      return 1;
   }

   COLevent Event;
   int TimerId = 0;
   BLUAinstance2* pInstance = BLUAinstanceGet(L);
   if (!pInstance) { LUAraiseLuaError(L, "Internal error - could not get a valid instance"); }
   pInstance->m_pLoop->post(COLnewClosure0(&LTMRsetTimerOnMain, pInstance, Delay, pData, FcnRef, &Event, &TimerId));
   COL_TRC("Block until LTMRsetTimerOnMain() signals");
   Event.wait();

   COL_VAR(TimerId);
   lua_pushnumber(L, TimerId);
   return 1;
}


// MAIN THREAD
static void LTMRcancelTimerOnMain(BLUAinstance2* pInstance, int TimerId, COLevent* pEvent) {
   COL_FUNCTION(LTMRcancelTimerOnMain);
   // TODO: error checking? timer id invalid, etc.
   pInstance->m_pLoop->cancelTimer(TimerId);
   pInstance->m_TimerMap.removeKey(TimerId);
   COL_TRC("Signal LTMRcancelTimer() to continue");
   pEvent->signal();
}

// LUA THREAD
int LTMRcancelTimer(lua_State* L) {
   COL_FUNCTION(LTMRcancelTimer);
   COL_VAR(LUAcomponentId(L));
   int TimerId = LUAexpectedIntParam(L, 1, "timer_id");
   if (LUALisEnvironmentTest(L) || LUALisEnvironmentStartup(L)) { return 0; }// this is a no-op in IDE mode

   COLevent Event;
   BLUAinstance2* pInstance = BLUAinstanceGet(L);
   pInstance->m_pLoop->post(COLnewClosure0(&LTMRcancelTimerOnMain, pInstance, TimerId, &Event));
   COL_TRC("Block until LTMRcancelTimerOnMain() signals");
   Event.wait();
   return 0;
}


// MAIN THREAD
void LTMRcancelAllTimers(BLUAinstance2* pInstance) {
   COL_FUNCTION(LTMRcancelAllTimers);
   COL_TRC("Removing all timers");
   COLmap<int, bool>& TimerMap = pInstance->m_TimerMap;
   COL_VAR(TimerMap.size());
   auto Timer = TimerMap.begin();
   while (Timer) {
      pInstance->m_pLoop->cancelTimer(Timer.key());
      COL_TRC("Timer " << Timer.key() << " canceled");
      Timer = TimerMap.erase(Timer);
   }
   COL_VAR(TimerMap.size());
}
