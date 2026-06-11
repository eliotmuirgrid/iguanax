#ifndef __SDB_JOURNAL_THREAD_H__
#define __SDB_JOURNAL_THREAD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBjournalThread
//
// Description:
//
// Everything related to journal io.
//
// Author: John Qi
// Date:   Tuesday 25 April 2023 - 05:09PM
// ---------------------------------------------------------------------------

#include <COL/COLauto.h>
#include <COL/COLclosure6.h>
#include <COL/COLhashmap.h>
#include <COL/COLthreadPool.h>
#include <SDB/SDBdir.h>
#include <SDB/SDBmessageId.h>
#include <SDB/SDBpositionMap.h>

class COLstring;
class SCKloop;
class SDBsystem;

typedef enum SDBjournalTaskID {
   SDB_JOURNAL_STOP,
   SDB_JOURNAL_WRITE,
   SDB_JOURNAL_SWAP,
} SDBjournalTaskID;

class SDBjournalTask {
public:
   SDBjournalTask(SDBjournalTaskID TaskId, COLclosure0* pTaskCallback, COLauto<COLstring> pTaskData = NULL, COLauto<SDBpositionMap> pTaskPosition = NULL)
    : Id(TaskId), pCallback(pTaskCallback), pData(pTaskData), pPosition(pTaskPosition) {}
   SDBjournalTaskID Id = SDB_JOURNAL_WRITE;
   COLclosure0* pCallback = NULL;
   COLauto<COLstring> pData;
   COLauto<SDBpositionMap> pPosition;
};

void SDBstartJournalThread(SCKloop* pLoop, COLthreadQueue<SDBjournalTask>* pQueue, COLclosure0* pStarted, const COLstring& EncryptionKey, const COLstring& LogRootDir = SDBroot());
void SDBstopJournalThread(COLthreadQueue<SDBjournalTask>* pQueue, COLclosure0* pStopped);

#endif // end of defensive include
