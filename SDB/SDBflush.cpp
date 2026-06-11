// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBflush
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Thursday 09 March 2023 - 03:49PM
// ---------------------------------------------------------------------------
#include <time.h>

#include <SDB/SDBflush.h>

#include <SCK/SCKloop.h>

#include <SDB/SDBsystem.h>
#include <SDB/SDBenvelope.h>
#include <SDB/SDBfile.h>
#include <SDB/SDBjournalSwap.h>
#include <SDB/SDBjournalThread.h>
#include <SDB/SDBjournalWrite.h>
#include <SDB/SDBlogThread.h>
#include <SDB/SDBlogWrite.h>
#include <SDB/SDBpositionMap.h>

#include <FIL/FILutils.h>

#include <COL/COLthreadPool.h>
#include <COL/COLlist.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

class SDBflushState {
public:
   ~SDBflushState()  { if (pShutdown) { delete pShutdown; } }
   bool isFlushing() { return FlushTimer == 0; }
   int FlushTimer = 0;
   bool FlushAgain = false;
   COLint64 JournalSize = 0;     // an estimate of the journal file size, determines when we flush log files
   COLclosure0* pShutdown = NULL;
   time_t LogFlushTime = time(NULL);   // last log flush time
};

// is it better to put states in SDBsystem instead?
static SDBflushState FlushState;
static void SDBflushSystem(SDBsystem* pSystem);

static void SDBscheduleNextFlush(SDBsystem* pSystem) {
   COL_FUNCTION(SDBscheduleNextFlush);
   FlushState.FlushTimer = pSystem->pLoop->addTimer(COLnewClosure0(&SDBflushSystem, pSystem), 1000, false, "SDBsystemFlush");
}

static void SDBsystemFlushed(SDBsystem* pSystem) {
   COL_FUNCTION(SDBsystemFlushed);
   if (FlushState.FlushAgain) {
      FlushState.FlushAgain = false;
      COL_TRC("Flush system again as requested.");
      SDBflushSystem(pSystem);
   } else if (FlushState.pShutdown) {
      COL_TRC("Shutting down");
      COLclosure0* pShutdown = FlushState.pShutdown;
      FlushState.pShutdown = NULL;
      // pShutdown may set FlushState.pShutdown, so nullify it before running pShutdown
      pShutdown->runAndDelete();
   } else {
      COL_TRC("Schedule next flush");
      SDBscheduleNextFlush(pSystem);
   }
}

static void SDBprepareLogFlushList(SDBsystem* pSystem, COLlist<COLstring>* pLogList) {
   COL_FUNCTION(SDBprepareLogFlushList);
   for (auto i = pSystem->ComponentMap.begin(); i != pSystem->ComponentMap.end(); i++){
      if (!i.value().LogFlushPositions && i.value().pLogMessages->size() == 0) { continue; }
      COL_TRC("Adding log " << i.key() << " to flush list");
      pLogList->push_back(i.key());
   }
}

int SDBgetMaxLogFlushInterval() {
   COL_FUNCTION(SDBgetMaxLogFlushInterval);
   const char* pValue = getenv(SDB_MAX_LOG_FLUSH_INTERVAL_ENV_NAME);
   int Interval = SDB_DEFAULT_MAX_LOG_FLUSH_INTERVAL;
   if (pValue) { Interval = atoi(pValue); }
   if (Interval < 0) { Interval = SDB_DEFAULT_MAX_LOG_FLUSH_INTERVAL; }
   COL_VAR(Interval);
   return Interval;
}

static bool SDBskipLogFlush(time_t Now, bool FLushedJournal, SDBsystem* pSystem, COLlist<COLstring>* pLogList) {
   COL_FUNCTION(SDBskipLogFlush);
   SDBprepareLogFlushList(pSystem, pLogList);
   // don't flush if we have nothing to flush
   if (pLogList->size() == 0) { COL_TRC("Yes because of nothing to flush"); return true; }
   // must flush if we are shutting down
   if (FlushState.pShutdown) { COL_TRC("No because of shutdown"); return false; }
   // must flush if we have too much data in memory
   if (FlushState.JournalSize > SDB_LOG_FLUSH_SIZE_THRESHOLD) { COL_TRC("No because of size"); return false; }
   // must flush if it's been too long since the last flush
   if (Now - FlushState.LogFlushTime >= SDBgetMaxLogFlushInterval()) { COL_TRC("No because of time");  return false; }
   // opportunistically flush if there was nothing flushed to the journal. i.e. system not busy
   if (!FLushedJournal) { COL_TRC("No because of idle"); return false; }
   COL_TRC("Yes because of not forced to flush now");
   return true;
}

static void SDBjournalFlushed(bool FLushedJournal, SDBsystem* pSystem) {
   COL_FUNCTION(SDBjournalFlushed);
   time_t Now = time(NULL);
   COLlist<COLstring> LogList;
   if (SDBskipLogFlush(Now, FLushedJournal, pSystem, &LogList)) {
      COL_TRC("Skip log flush because of not shutting down, or not enough data, or not enough time, or busy flushing journal");
      SDBsystemFlushed(pSystem);
      return;
   }
   COL_TRC("Flushing " << FlushState.JournalSize << " bytes of data to logs");
   COL_VAR(LogList.size());
   FlushState.JournalSize = 0;      // reset journal size
   FlushState.LogFlushTime = Now;   // record log flush time
   COLclosure0* pDone = COLnewClosure0(&SDBsystemFlushed, pSystem);
   COLclosure0* pFlushed = COLnewClosure0(&SDBswapJournal, pSystem, pDone);
   SDBwriteLogs(pSystem, pFlushed, LogList);
}

static void SDBflushSystem(SDBsystem* pSystem) {
   COL_FUNCTION(SDBflushSystem);
   FlushState.FlushTimer = 0;
   bool FlushJournal = pSystem->JournalBuffer.size() > 0;
   COLclosure0* pJournalFlushed = COLnewClosure0(&SDBjournalFlushed, FlushJournal, pSystem);
   if (FlushJournal) {
      FlushState.JournalSize += pSystem->JournalBuffer.size();    // grow journal size
      SDBwriteJournal(pSystem, pJournalFlushed);
   } else {
      COL_TRC("Journal buffer empty. Flush log buffers");
      pJournalFlushed->runAndDelete();
   }
}

void SDBflushSystemNow(SDBsystem* pSystem) {
   COL_FUNCTION(SDBflushSystem);
   if (FlushState.isFlushing()) {
      COL_TRC("Flush in progress. Requesting another flush immediately after the current one.");
      FlushState.FlushAgain = true;
   } else {
      COL_TRC("Flush system now");
      pSystem->pLoop->cancelTimer(FlushState.FlushTimer);
      SDBflushSystem(pSystem);
   }
}

static void SDBflushCycleStarted(SDBsystem* pSystem, COLclosure0* pCallback) {
   COL_FUNCTION(SDBflushCycleStarted);
   SDBscheduleNextFlush(pSystem);
   pCallback->runAndDelete();
}

void SDBstartFlushCycle(SDBsystem* pSystem, COLclosure0* pCallback) {
   COL_FUNCTION(SDBstartFlushCycle);
   COLclosure0* pStarted = COLnewClosure0(&SDBflushCycleStarted, pSystem, pCallback);
   if (!SDBroot2().is_null()) {
      SDBstartLogThread2(pSystem, NULL, pSystem->LogEncryptionKey, SDBroot2());
      SDBstartJournalThread(pSystem->pLoop, &pSystem->JournalTaskQueue2, NULL, pSystem->LogEncryptionKey, SDBroot2());
   }
   // Don't think it's necessary to stage log thread start and journal thread start
   SDBstartLogThread(pSystem, NULL, pSystem->LogEncryptionKey, SDBroot());
   SDBstartJournalThread(pSystem->pLoop, &pSystem->JournalTaskQueue, pStarted, pSystem->LogEncryptionKey, SDBroot());
}

static void SDBshutdownComplete(SDBsystem* pSystem, COLclosure0* pCallback) {
   COL_FUNCTION(SDBshutdownComplete);
   if (!SDBroot2().is_null()) {
      SDBstopLogThread2(pSystem, NULL);
      SDBstopJournalThread(&pSystem->JournalTaskQueue2, NULL);
   }
   // Don't think it's necessary to stage log thread stop and journal thread stop
   SDBstopLogThread(pSystem, NULL);
   SDBstopJournalThread(&pSystem->JournalTaskQueue, pCallback);
}

static void SDBshutDownNow(SDBsystem* pSystem, COLclosure0* pCallback) {
   COL_FUNCTION(SDBshutDownNow);
   FlushState.pShutdown = COLnewClosure0(&SDBshutdownComplete, pSystem, pCallback);
   // need to flush one last time to ensure we have nothing in our buffers
   SDBflushSystem(pSystem);
}

void SDBstopFlushCycle(SDBsystem* pSystem, COLclosure0* pCallback) {
   COL_FUNCTION(SDBstopFlushCycle);
   COLclosure0* pShutdown = COLnewClosure0(&SDBshutDownNow, pSystem, pCallback);
   if (FlushState.isFlushing()) {
      COL_TRC("Flushing in progress. Shutdown requested.");
      FlushState.pShutdown = pShutdown;
   } else {
      COL_TRC("System idling. Shutting down now.");
      pSystem->pLoop->cancelTimer(FlushState.FlushTimer);
      pShutdown->runAndDelete();
   }
}
