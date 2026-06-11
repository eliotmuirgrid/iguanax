#ifndef __SDB_DEQUEUE_MULTI_H__
#define __SDB_DEQUEUE_MULTI_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBdequeueMulti
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
#include <COL/COLmap.h>
#include <SDB/SDBdir.h>
#include <SDB/SDBid.h>
#include <time.h>
#include <memory>

class SCKloop;
class SDBmessage;
class SDBqueueReader;

class SDBqueueReaderState {
public:
   SDBqueueReader* pReader    { NULL };
   SDBid           Position;
   int             Code       { 0 };
   bool            Fetching   { 0 };   // fetching from logs or not
};

class SDBdequeueMulti {
public:
   ~SDBdequeueMulti();
   bool       Busy = false;  // whether we are dequeueing from file in a worker thread
   bool       DirtyCache = false;   // must dequeue from file if cache is modified
   time_t     LastCacheActivity = 0;   // last time dequeued from cache
   COLstring  EncryptionKey;
   COLstring  LogDir1;
   COLstring  LogDir2;
   SDBid      Position;     // current dequeue position
   bool       DequeueFromLogs = true;
   SDBqueueReaderState Reader1;
   SDBqueueReaderState Reader2;
   bool        Responded    { false };   // last time respond
   COLmap<SDBid, SDBmessage*> Messages;
   COLauto<COLclosure2<int, SDBmessage*>> pOnDequeued;
   COLlist<std::shared_ptr<SDBmessage>>* pQueueCache = NULL;  // reference to queue cache in SDBcomponent, don't delete in destructor
};

// create a dequeue
SDBdequeueMulti* SDBcreateDequeueMulti(const SDBmessageId& Config, COLlist<std::shared_ptr<SDBmessage>>* pQueueCache, const COLstring& EncryptionKey, const COLstring& LogRootDir1 = SDBroot(), const COLstring& LogRootDir2 = SDBroot2());
void SDBsetOnDequeuedMulti(SDBdequeueMulti* pDequeue, COLauto<COLclosure2<int, SDBmessage*>> pCallback);

// marks the cache dirty so that next SDBdequeueMessage() fetches message from file
void SDBpolluteCacheMulti(SDBdequeueMulti* pDequeue);

// fetches the next SDB_MESSAGE from the queue. The same message is return for subsequent calls until the message is SDBremoveCommittedMessage()'ed
void SDBdequeueMessageMulti(SDBdequeueMulti* pDequeue, SCKloop* pLoop);
// removes a committed messages from the cache so the next message can be dequeued. Typically called after the PositionCommitted event
void SDBremoveCommittedMessageMulti(SDBdequeueMulti* pDequeue, SDBid Position);

// returns false if dequeue from file is in progress. try again later
void SDBmovePositionMulti(SDBdequeueMulti* pDequeue, SDBid NewPosition);


// unit test
bool SDBinitDequeuePositionMulti(SDBdequeueMulti* pDequeue, SDBqueueReaderState* pOtherReader, SCKloop* pLoop, COLclosure2<int, SDBmessage*>* pCallback);
bool SDBdequeueFromMessagesMulti(SDBdequeueMulti* pDequeue, SCKloop* pLoop, COLclosure2<int, SDBmessage*>* pCallback);
void SDBmessageDequeuedFromFileMulti(int Code, SDBqueueReaderState* pOtherReader, SDBdequeueMulti* pDequeue, SCKloop* pLoop, COLclosure2<int, SDBmessage*>* pCallback);

#endif // end of defensive include
