#ifndef __SDB_COMPONENT_H__
#define __SDB_COMPONENT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBcomponent
//
// Description:
//
// iguana component
//
// Author: John Q
// Date:   Fri 12 Sep 2025 13:37:47 EDT
// ---------------------------------------------------------------------------
#include <COL/COLminimumInclude.h>
#include <COL/COLhashmap.h>
#include <COL/COLlist.h>
#include <COL/COLclosure6.h>
#include <COL/COLauto.h>
#include <SDB/SDBdequeue.h>
#include <memory>

struct CFGmap;

class SDBconsumer{
public:
    SDBconsumer();
   ~SDBconsumer();
   SDBid                Position;               // queue position
   COLauto<SDBdequeue>  pDequeue;               // queue reader
   COLauto<COLclosure0> pPositionCommitted;
   COLauto<COLclosure0> pQueueNotEmpty;
};

class SDBcomponent{
public:
   SDBcomponent();
   ~SDBcomponent();
   SDBid     LastLogId;                                              // ID of last log message, but we only care about SequenceId and QueueId
   SDBid     LastJournaledId;                                        // ID of last data message written to journal file
   COLuint64 BOQqueueId    = 0;                                      // Beginning of Queue position with proper QueueId for queue count
   COLint64  LastDateTime  = 0;                                      // Last message's timestamp, used to determine if NextMessageId needs to be incremented or reset to 1
   COLint64  NextMessageId = 0;                                      // Used to distinguish messages with the same millisecond timestamp
   COLhashmap<COLstring, SDBconsumer> Consumers;                     // only contains consumers with valid positions (BOQ excluded)
   // caches
   COLauto<COLlist<std::shared_ptr<SDBmessage>>> pQueueCache;        // caches enqueued SDB_MESSAGE messages for fast dequeue
   // data structures related to journal and log flushing
   bool LogFlushPositions = false;                                   // tracks if queue positions need be flushed to the log file
   COLauto<COLlist<std::shared_ptr<SDBmessage>>> pLogMessages;       // tracks the SDB_MESSAGE messages to be flushed to the log file
   COLauto<COLlist<std::shared_ptr<SDBmessage>>> pJournalMessages;   // tracks the SDB_MESSAGE messages to be flushed to the journal
   COLauto<COLhashmap<COLstring, SDBid>>         pJournalPositions;  // tracks queue positions to be flushed to the journal
   // various callbacks after journal flush
   COLauto<COLclosure1<SDBid>> pMessageCommitted;
};

void SDBinitializeComponents(const CFGmap& ConfigMap, const COLstring& EncKey, SCKloop* pLoop, COLclosure1<COLhashmap<COLstring, SDBcomponent>*>* pCallback, const COLstring& LogRootDir = SDBroot());

class SDBcomponentStatus {
public:
   SDBid LastLogId;                                // only care about the sequence and queue id
   SDBid LastDataId;
   COLuint64 BOQqueueId { SDB_INVALID_QID };
   COLhashmap<COLstring, SDBid> PositionMap;
};

void SDBgetPositionMapLastDataAndLogId(const COLstring& ComponentId, const COLstring& LogRootDir, const COLstring& EncryptionKey, bool HasConsumers, SDBcomponentStatus& Status);



// unit test

bool SDBfindPositionMapLastDataAndLogId(const COLstring& LogFile, const COLstring& EncryptionKey, bool HasConsumers, SDBcomponentStatus& Status);
bool SDBgetBOQqueueId(const COLstring& LogFile, const COLstring& EncryptionKey, COLuint64* pQueueId);

#endif // end of defensive include
