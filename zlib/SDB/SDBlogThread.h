#ifndef __SDB_LOG_THREAD_H__
#define __SDB_LOG_THREAD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBlogThread
//
// Description:
//
// Everything related to log io.
//
// Author: John Qi
// Date:   Tuesday 25 April 2023 - 05:09PM
// ---------------------------------------------------------------------------
#include <memory>
#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLhashmap.h>
#include <COL/COLmap.h>
#include <COL/COLthreadPool.h>
#include <SDB/SDBdir.h>
#include <SDB/SDBfile.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBmessageId.h>

class COLstring;
class SCKloop;
class SDBsystem;

typedef enum SDBlogTaskID {
   SDB_LOG_STOP,
   SDB_LOG_WRITE,
   SDB_LOG_IDLE,
} SDBlogTaskID;

class SDBlogHandle {
public:
   ~SDBlogHandle() { if (-1 != FD) { SDBcloseLogFile(FD); } }
   int FD = -1;
   COLstring FilePath;
   time_t LastAccess = 0;
};

class SDBlogTaskData{
public:
   SDBlogTaskData() {}
   SDBlogTaskData(
      COLauto<COLlist<std::shared_ptr<SDBmessage>>> pLogMessages,
      COLauto<COLhashmap<COLstring, SDBid>> pLogPositions,
      bool FlushPos
   ) : pMessages(pLogMessages), pPositions(pLogPositions), FlushPosition(FlushPos) {}
   SDBlogTaskData(const SDBlogTaskData& other) : FlushPosition(other.FlushPosition) {
      if (other.pMessages) {
         auto newList = COLauto<COLlist<std::shared_ptr<SDBmessage>>>(new COLlist<std::shared_ptr<SDBmessage>>);
         for (const auto& msg : *other.pMessages) {
            newList->push_back(msg);
         }
         pMessages = newList;
      }
      if (other.pPositions) {
         pPositions = COLauto<COLhashmap<COLstring, SDBid>>(new COLhashmap<COLstring, SDBid>(*other.pPositions));
      }
   }

   COLauto<COLlist<std::shared_ptr<SDBmessage>>> pMessages;
   COLauto<COLhashmap<COLstring, SDBid>> pPositions;
   bool FlushPosition = false;
};

class SDBlogTask {
public:
   SDBlogTask(SDBlogTaskID TaskId, COLclosure0* pTaskCallback, COLauto<COLmap<COLstring, SDBlogTaskData>> pTaskData = NULL)
    : Id(TaskId), pCallback(pTaskCallback), pData(pTaskData) {}
   SDBlogTaskID Id = SDB_LOG_WRITE;
   COLclosure0* pCallback = NULL;
   COLauto<COLmap<COLstring, SDBlogTaskData>> pData;
};

void SDBstartLogThread(SDBsystem* pSystem, COLclosure0* pStarted, COLstring EncryptionKey, const COLstring& LogRootDir = SDBroot());
void SDBstopLogThread(SDBsystem* pSystem, COLclosure0* pStopped);

void SDBstartLogThread2(SDBsystem* pSystem, COLclosure0* pStarted, COLstring EncryptionKey, const COLstring& LogRootDir = SDBroot2());
void SDBstopLogThread2(SDBsystem* pSystem, COLclosure0* pStopped);

#endif // end of defensive include
