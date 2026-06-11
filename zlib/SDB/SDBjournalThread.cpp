// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBjournalThread
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Tuesday 25 April 2023 - 05:09PM
// ---------------------------------------------------------------------------

#include <SDB/SDBjournalSwap.h>
#include <SDB/SDBjournalThread.h>
#include <SDB/SDBjournalWrite.h>
#include <SDB/SDBfile.h>

#include <SCK/SCKloop.h>

#include <FIL/FILutils.h>

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
void SDBjournalThreadWorker(SCKloop* pLoop, COLthreadQueue<SDBjournalTask>* pQueue, COLclosure0* pStarted, COLstring EncryptionKey, COLstring LogRootDir) {
   COL_FUNCTION(SDBjournalThreadWorker);
   int  FD = -1;
   bool KeepRunning = true;
   SDBcheckLogRootDir(LogRootDir);
   if (pStarted) { pLoop->post(pStarted); }
   while (KeepRunning) {
      SDBjournalTask Task = pQueue->take();
      try {
         switch (Task.Id) {
         case SDB_JOURNAL_SWAP:
            SDBreplaceJournalFile(FD, Task.pPosition, EncryptionKey, LogRootDir);
            break;
         case SDB_JOURNAL_WRITE:
            SDBwriteJournalFile(FD, Task.pData, EncryptionKey, LogRootDir);
            break;
         case SDB_JOURNAL_STOP:     // Received shutdown request
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
   if (-1 != FD) { SDBcloseLogFile(FD); }
}

// MAIN THREAD
void SDBstartJournalThread(SCKloop* pLoop, COLthreadQueue<SDBjournalTask>* pQueue, COLclosure0* pStarted, const COLstring& EncryptionKey, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBstartJournalThread);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBjournalThreadWorker, pLoop, pQueue, pStarted, EncryptionKey, LogRootDir));
}

// MAIN THREAD
void SDBstopJournalThread(COLthreadQueue<SDBjournalTask>* pQueue, COLclosure0* pStopped) {
   COL_FUNCTION(SDBstopJournalThread);
   pQueue->put(SDBjournalTask(SDB_JOURNAL_STOP, pStopped));
}
