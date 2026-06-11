// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBlogWrite
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Tuesday 25 April 2023 - 05:09PM
// ---------------------------------------------------------------------------

#include <SDB/SDBcache.h>
#include <SDB/SDBenvelope.h>
#include <SDB/SDBfile.h>
#include <SDB/SDBflush.h>
#include <SDB/SDBlogThread.h>
#include <SDB/SDBlogWrite.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBsystem.h>

#include <SCK/SCKloop.h>

#include <FIL/FILutils.h>

#include <COL/COLclosure.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#define SDB_LOG_CLOSE_IDLE_HANDLE_POLL_TIME_MS 2000

// WORKER THREAD
void SDBcloseIdleHandles(COLmap<COLstring, SDBlogHandle>& HandleMap) {
   COL_FUNCTION(SDBcloseIdleHandles);
   auto i = HandleMap.begin();
   time_t Now = time(NULL);
   while (i != HandleMap.end()) {
      SDBlogHandle& Handle = i.value();
      // Close the file handle if it's not used in the most recent flush
      // Could change to a better cache algo in the future
      if (Now - Handle.LastAccess > SDBgetMaxLogFlushInterval()) {
         i = HandleMap.erase(i);    // SDBlogHandle dtor closes the file handle
      } else {
         i++;
      }
   }
}

static void SDBcloseIdleLogHandles(COLthreadQueue<SDBlogTask>* pLogTaskQueue){
   COL_FUNCTION(SDBcloseIdleLogHandles);
   // If there is a write task before us, it will have close idle file handles
   // If there is a shutdown task before us, it will close all handles
   if (pLogTaskQueue->size()) { COL_TRC("Tasks in the queue before us. Skip"); return; }
   pLogTaskQueue->put(SDBlogTask(SDB_LOG_IDLE, NULL));
}

// MAIN THREAD
int SDBstartIdleHandleTimer(SCKloop* pLoop, COLthreadQueue<SDBlogTask>* pLogTaskQueue) {
   return pLoop->addTimer(COLnewClosure0(&SDBcloseIdleLogHandles, pLogTaskQueue), SDB_LOG_CLOSE_IDLE_HANDLE_POLL_TIME_MS, true);
}

// MAIN THREAD
void SDBstopIdleHandleTimer(SCKloop* pLoop, int& TimerId) {
   pLoop->cancelTimer(TimerId);
   TimerId = 0;
}
