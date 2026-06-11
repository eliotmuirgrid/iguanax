#ifndef __SDB_DEQUEUE_SINGLE_H__
#define __SDB_DEQUEUE_SINGLE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBdequeueSingle
//
// Description:
//
// Dequeue routines
//
// Author: John Qi
// Date:   Thursday 16 March 2023 - 09:55PM
// ---------------------------------------------------------------------------

#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLlist.h>
#include <SDB/SDBdir.h>
#include <SDB/SDBid.h>
#include <memory>

class SCKloop;
class SDBmessage;
class SDBreader;

class SDBdequeueSingle {
public:
   ~SDBdequeueSingle();
   bool       Busy = false;  // whether we are dequeueing from file in a worker thread
   bool       DirtyCache = false;   // must dequeue from file if cache is modified
   time_t     LastCacheActivity = 0;   // last time dequeued from cache
   COLstring  EncryptionKey;
   COLstring  LogDir;
   SDBreader* pReader { nullptr };
   SDBid      Position;
   COLauto<COLclosure2<int, SDBmessage*>> pOnDequeued;
   COLlist<std::shared_ptr<SDBmessage>>* pQueueCache { nullptr };  // reference to queue cache in SDBcomponent, don't delete in destructor
};

// create a dequeue
SDBdequeueSingle* SDBcreateDequeueSingle(const SDBmessageId& Config, COLlist<std::shared_ptr<SDBmessage>>* pQueueCache, const COLstring& EncryptionKey, const COLstring& LogRootDir = SDBroot());
void SDBsetOnDequeuedSingle(SDBdequeueSingle* pDequeue, COLauto<COLclosure2<int, SDBmessage*>> pCallback);

// marks the cache dirty so that next SDBdequeueMessage() fetches message from file
void SDBpolluteCacheSingle(SDBdequeueSingle* pDequeue);

// fetches the next SDB_MESSAGE from the queue. The same message is return for subsequent calls until the message is SDBremoveCommittedMessage()'ed
void SDBdequeueMessageSingle(SDBdequeueSingle* pDequeue, SCKloop* pLoop);
// removes a committed messages from the cache so the next message can be dequeued. Typically called after the PositionCommitted event
void SDBremoveCommittedMessageSingle(SDBdequeueSingle* pDequeue, SDBid Position);

// returns false if dequeue from file is in progress. try again later
void SDBmovePositionSingle(SDBdequeueSingle* pDequeue, SDBid NewPosition, const COLstring& LogRootDir = SDBroot());

#endif // end of defensive include
