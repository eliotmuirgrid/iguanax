//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAawait
//
// Description:
//
// Implementation.
//
// Author: John Q
// Date:   Tue 11 Oct 2022 9:32:55 EDT
//---------------------------------------------------------------------------
#include "LUAawait.h"

#include <COL/COLclosure.h>
#include <COL/COLerror.h>
#include <COL/COLlocker.h>
#include <COL/COLlog.h>
#include <COL/COLmutex.h>
#include <COL/COLthread.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

struct LUAsynchronize {
   bool         Waiting { false };
   COLmutex     Mutex;
   COLcondition CV;
   COLthreadID  WaitThread;
   // timeout
   int      TimerId  { 0 };
   bool&    Shutdown;   // reference to BLUAinstance m_ForceKill
   bool&    Expired;    // whether LUAawait() timed out
   time_t   Timeout;    // when timeout should happen
   SCKloop* pLoop;
   LUAsynchronize(SCKloop* vpLoop, bool& rShutdown, int vTimeout, bool& rExpired) :
   pLoop(vpLoop), Shutdown(rShutdown), Timeout(0), Expired(rExpired) {
      if (vTimeout > 0) { Timeout = ::time(NULL) + vTimeout; }
   };
};

static void LUAstopWait(std::weak_ptr<struct LUAsynchronize> pWeak) {
   COL_FUNCTION(LUAstopWait);
   if (std::shared_ptr<struct LUAsynchronize> pShare = pWeak.lock()) {
      COL_TRC("pLUAsynchronize = " << pShare.get());
      time_t Now = ::time(NULL);
      // if not(timeout or shutdown) check again in 1 second.
      if ((!pShare->Timeout || Now < pShare->Timeout) && !pShare->Shutdown) {
         COL_TRC("Check again in a second");
         pShare->TimerId = pShare->pLoop->addTimer(COLnewClosure0(&LUAstopWait, pWeak), 1000);
         return;
      }
      COLASSERT(pShare->WaitThread != COLthreadGetCurrentThreadID());
      COL_TRC("I am tired of waiting");
      COL_TRC("Notifying LUAawait()...");
      COLlocker Lock(pShare->Mutex);
      pShare->Expired = true;
      pShare->Waiting = false;
      pShare->CV.broadcast();
   } else {
      COL_TRC("LUAwait completed without timeout");
      // we reach here if
      // 1. LUAstopWait() is schedule to run next on main thread
      // 2. current function on main thread calls LUAasync()
      // 3. LUAasync() returns and LUAawait() on Lua thread is signaled to continue
      // 4. context switch to Lua thread and LUAawait() returns destroying std::weak_ptr<struct LUAsynchronize> in the process
      // 5. context switch back to main thread and enters LUAstopWait()
      // 6. std::weak_ptr<struct LUAsynchronize> destroyed already, we get here
   }
}

static void LUArunOnMain(COLclosure1<std::weak_ptr<struct LUAsynchronize>>* pFunction, std::shared_ptr<struct LUAsynchronize> pShare) {
   COL_FUNCTION(LUArunOnMain);
   pFunction->runAndDelete(pShare);    // pFunction calls LUAasync() when done
   std::weak_ptr<struct LUAsynchronize> pWeak = pShare;
   pShare->TimerId = pShare->pLoop->addTimer(COLnewClosure0(&LUAstopWait, pWeak), 1000);
}

bool LUAawait(COLclosure1<std::weak_ptr<struct LUAsynchronize>>* pFunction, SCKloop* pLoop, bool& ForceKill, int Timeout) {
   COL_FUNCTION(LUAawait);
   bool Expired = false;
   std::shared_ptr<struct LUAsynchronize> pShare = std::make_shared<struct LUAsynchronize>(pLoop, ForceKill, Timeout, Expired);
   COL_TRC("pLUAsynchronize = " << pShare.get());
   pShare->WaitThread = COLthreadGetCurrentThreadID();
   COLlocker Lock(pShare->Mutex);
   pLoop->post(COLnewClosure0(&LUArunOnMain, pFunction, pShare));
   COL_TRC("Waiting for LUAasync()...");
   COLASSERT(pShare->Waiting == false);
   pShare->Waiting = true;
   while (pShare->Waiting) {
      pShare->CV.wait(&pShare->Mutex);
   }
   return !Expired;
}

void LUAasync(std::weak_ptr<struct LUAsynchronize> pWeak) {
   COL_FUNCTION(LUAasync);
   if (std::shared_ptr<struct LUAsynchronize> pShare = pWeak.lock()) {
      COL_TRC("pLUAsynchronize = " << pShare.get());
      COLASSERT(pShare->WaitThread != COLthreadGetCurrentThreadID());
      COL_TRC("Notifying LUAawait()...");
      COLlocker Lock(pShare->Mutex);
      if (pShare->TimerId) {
         COL_TRC("Cancel timeout timer");
         pShare->pLoop->cancelTimer(pShare->TimerId);
         pShare->TimerId = 0;
      }
      pShare->Waiting = false;
      pShare->CV.broadcast();
   } else {
      COL_TRC("LUAwait() timed out already");
      // we reach here if
      // 1. LUAstopWait() is called because LUAawait() timed out
      // 2. LUAstopWait() signals LUAawait() to continue
      // 3. context switch to Lua thread and LUAawait() returns, destroying std::weak_ptr<struct LUAsynchronize> in the process
      // 4. context switch to main thread, the task completes and LUAasync() is called
      // 5. std::weak_ptr<struct LUAsynchronize> destroyed already, we get here
   }
}
