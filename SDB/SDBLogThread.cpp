// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBlogThread
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Tuesday 25 April 2023 - 05:09PM
// ---------------------------------------------------------------------------

#include <SDB/SDBlogIdle.h>
#include <SDB/SDBlogThread.h>
#include <SDB/SDBlogWrite.h>
#include <SDB/SDBsystem.h>

#include <SCK/SCKloop.h>

#include <FIL/FILutils.h>

#include <COL/COLclosure.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static int SDBcheckLogRootDir(const COLstring& LogRootDir) {
   COL_FUNCTION(SDBcheckLogRootDir);
   if (LogRootDir.is_null()) { return 0; }
   if (!FILfileExists(LogRootDir)) {
      FILmakeFullDir(LogRootDir, FIL_USER_RWX);
   }
   return FILisDirectory(LogRootDir) ? 0 : EEXIST;
}

// WORKER THREAD
void SDBlogThreadWorker(SCKloop* pLoop, COLthreadQueue<SDBlogTask>* pQueue, COLclosure0* pStarted, COLstring EncryptionKey, COLstring LogRootDir) {
   COL_FUNCTION(SDBlogThreadWorker);
   bool KeepRunning = true;
   COLmap<COLstring, SDBlogHandle> HandleMap;
   if (pStarted) { pLoop->post(pStarted); }
   while (KeepRunning) {
      SDBlogTask Task = pQueue->take();
      try {
         switch (Task.Id) {
         case SDB_LOG_WRITE:
            SDBwriteLogFiles(HandleMap, Task.pData, EncryptionKey, LogRootDir);
            break;
         case SDB_LOG_IDLE:
            // If there are WRITE tasks after us, don't close handles in case they are needed
            // If there is a STOP tasks after us, it will close all handles
            if (pQueue->size() == 0) {
               SDBcloseIdleHandles(HandleMap);
            }
            break;
         case SDB_LOG_STOP:     // Received shutdown request
         default:
            KeepRunning = false;
            break;
         }
      } catch (const COLerror& Error) {
         COL_ERR(Error);
         // TODO: handle io errors
      }
      if (Task.pCallback) { pLoop->post(Task.pCallback); }
   }
}

// MAIN THREAD
void SDBstartLogThread(SDBsystem* pSystem, COLclosure0* pStarted, COLstring EncryptionKey, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBstartLogThread);
   SCKloop* pLoop = pSystem->pLoop;
   COLthreadQueue<SDBlogTask>* pQueue = &pSystem->LogTaskQueue;
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBlogThreadWorker, pLoop, pQueue, pStarted, EncryptionKey, LogRootDir));
   pSystem->IdleLogHandleTimer = SDBstartIdleHandleTimer(pLoop, pQueue);
}

// MAIN THREAD
void SDBstopLogThread(SDBsystem* pSystem, COLclosure0* pStopped) {
   COL_FUNCTION(SDBstopLogThread);
   SCKloop* pLoop = pSystem->pLoop;
   COLthreadQueue<SDBlogTask>* pQueue = &pSystem->LogTaskQueue;
   pQueue->put(SDBlogTask(SDB_LOG_STOP, pStopped));
   SDBstopIdleHandleTimer(pLoop, pSystem->IdleLogHandleTimer);
}

// MAIN THREAD
void SDBstartLogThread2(SDBsystem* pSystem, COLclosure0* pStarted, COLstring EncryptionKey, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBstartLogThread2);
   SCKloop* pLoop = pSystem->pLoop;
   COLthreadQueue<SDBlogTask>* pQueue = &pSystem->LogTaskQueue2;
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBlogThreadWorker, pLoop, pQueue, pStarted, EncryptionKey, LogRootDir));
   pSystem->IdleLogHandleTimer2 = SDBstartIdleHandleTimer(pLoop, pQueue);
}

// MAIN THREAD
void SDBstopLogThread2(SDBsystem* pSystem, COLclosure0* pStopped) {
   COL_FUNCTION(SDBstopLogThread2);
   pSystem->LogTaskQueue2.put(SDBlogTask(SDB_LOG_STOP, pStopped));
   SDBstopIdleHandleTimer(pSystem->pLoop, pSystem->IdleLogHandleTimer2);
}
