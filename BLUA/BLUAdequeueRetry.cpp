//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAqueue
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Friday May 14 2021
//---------------------------------------------------------------------------

#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAqueue.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtest.h>

#include <SCK/SCKloop.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


static void BLUAinstanceRetryDequeue(BLUAinstance2* pInstance) {
   COL_FUNCTION(BLUAinstanceRetryDequeue);
   BLUAinstanceDequeueMessage(pInstance);
   pInstance->m_pDequeueRetryTimer = 0;
}

static void BLUAinstanceRetryDequeueOnMain(BLUAinstance2* pInstance, int Delay) {
   COL_FUNCTION(BLUAinstanceRetryDequeueOnMain);
   // Store the desired delay, and let BLUAinstanceDequeued() to set up the retry timer
   pInstance->m_DequeueRetryDelay = Delay;
   COL_TRC("Dequeue retry requested with delay " << Delay);
}

// MAIN THREAD
void BLUAinstanceRetryDequeueSetup(BLUAinstance2* pInstance) {
   COL_FUNCTION(BLUAinstanceRetryDequeueSetup);
   int Delay = pInstance->m_DequeueRetryDelay;
   if (-1 == Delay) {
      COL_WRN("Someone asked me to set up dequeue retry, but the delay is not set. Weird");
      return;
   }
   if (Delay) {
      if (pInstance->m_pDequeueRetryTimer) {
         // if a destination component erroneously calls its main function in main.lua,
         // we could end up with two retry timer overwriting one another
         COL_WRN("Component attempting to set up two dequeue retry timers. Cancelling the first timer");
         pInstance->m_pLoop->cancelTimer(pInstance->m_pDequeueRetryTimer);
      }
      pInstance->m_pDequeueRetryTimer = pInstance->m_pLoop->addTimer(COLnewClosure0(&BLUAinstanceRetryDequeue, pInstance), Delay);
      COL_VAR(pInstance->m_pDequeueRetryTimer);
      COL_TRC("Retry dequeue schedule in " << Delay << " ms.");
   } else { // no delay
      COL_TRC("Retry dequeue now.");
      BLUAinstanceDequeueMessage(pInstance);
   }
   // reset dequeue retry delay
   pInstance->m_DequeueRetryDelay = -1;
}

int BLUAretryDequeue(lua_State* L) {
   COL_FUNCTION(BLUAretryDequeue);
   int Delay = LUAoptionalIntParam(L, 1, "delay", 100);
   if (Delay < 0) {
      LUA_ERROR_STREAM("'delay' must be a positive integer or zero.");
   }
   if (LUALisEnvironmentTest(L) || LUALisEnvironmentStartup(L)) {
      COL_TRC("IDE mode. no-op.");
      return 0; // this is a no-op in IDE mode
   }
   COL_VAR(Delay);
   BLUAinstance2* pInstance = BLUAinstanceGet(L);
   pInstance->m_pLoop->post(COLnewClosure0(&BLUAinstanceRetryDequeueOnMain, pInstance, Delay));
   return 0;
}
