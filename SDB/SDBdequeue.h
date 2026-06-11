#ifndef __SDB_DEQUEUE_H__
#define __SDB_DEQUEUE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBdequeue
//
// Description:
//
// Dequeue routines
//
// Author: John Qi
// Date:   Thursday 16 March 2023 - 09:55PM
// ---------------------------------------------------------------------------

#include <SDB/SDBdequeueMulti.h>
#include <SDB/SDBdequeueSingle.h>

class SDBdequeue {
public:
   ~SDBdequeue();

   SDBdequeueSingle* pDequeueS { NULL };
   SDBdequeueMulti*  pDequeueM { NULL };
   bool HA { false };

   void setLastCacheActivity(time_t Time)  { HA ? pDequeueM->LastCacheActivity = Time : pDequeueS->LastCacheActivity = Time; }   // for unit test
   time_t lastCacheActivity() const { return HA ? pDequeueM->LastCacheActivity        : pDequeueS->LastCacheActivity; }
   bool busy()                      { return HA ? pDequeueM->Busy                     : pDequeueS->Busy; }
   bool dirtyCache()                { return HA ? pDequeueM->DirtyCache               : pDequeueS->DirtyCache; }
   SDBid position()                 { return HA ? pDequeueM->Position                 : pDequeueS->Position; }
};

// create a dequeue
SDBdequeue* SDBcreateDequeue(const SDBmessageId& Config, COLlist<std::shared_ptr<SDBmessage>>* pQueueCache, const COLstring& EncryptionKey);
void SDBsetOnDequeued(SDBdequeue* pDequeue, COLauto<COLclosure2<int, SDBmessage*>> pCallback);

// marks the cache dirty so that next SDBdequeueMessage() fetches message from file
void SDBpolluteCache(SDBdequeue* pDequeue);

// fetches the next SDB_MESSAGE from the queue. The same message is return for subsequent calls until the message is SDBremoveCommittedMessage()'ed
void SDBdequeueMessage(SDBdequeue* pDequeue, SCKloop* pLoop);

// removes a committed messages from the cache so the next message can be dequeued. Typically called after the PositionCommitted event
void SDBremoveCommittedMessage(SDBdequeue* pDequeue, SDBid Position);

// returns false if dequeue from file is in progress. try again later
void SDBmovePosition(SDBdequeue* pDequeue, SDBid NewPosition);

#endif // end of defensive include
