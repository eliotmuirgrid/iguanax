// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCache
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Mon 13 Feb 2023 10:01:00 EST
// ---------------------------------------------------------------------------
#include "testCache.h"

#include <memory>

#include <UNIT/UNITapp.h>

#include <SDB/SDBcache.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBmessageId.h>
#include <SDB/SDBsystem.h>

#include <SDB/SDBfile.h>
#include <SDB/SDBnumber.h>
#include <SDB/SDBepochTime.h>

#include <SDBT/SDBTdir.h>

#include <FIL/FILutils.h>

#include <COL/COLarray.h>
#include <COL/COLlist.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testCacheAdd(){
   COL_FUNCTION(testCacheAdd);
   SDBcomponent Queue;
   COLlist<std::shared_ptr<SDBmessage>> MessageList;
   MessageList.push_back(std::shared_ptr<SDBmessage>(new SDBmessage("test", "data")));

   // do not cache message if queue has no consumer
   SDBcacheEnqueuedMessages(Queue, &MessageList);
   UNIT_ASSERT_EQUALS(0, Queue.pQueueCache->size());

   // cache message if queue has consumers
   Queue.Consumers["consumer"];
   SDBcacheEnqueuedMessages(Queue, &MessageList);
   UNIT_ASSERT_EQUALS(1, Queue.pQueueCache->size());
}

// generates Count messages that have the same epoch timestamp
static COLarray<SDBid> SDBTgenerateTestData(COLlist<std::shared_ptr<SDBmessage>>* pList, int Count = 5, int Size = 1000) {
   COL_FUNCTION(SDBTgenerateTestData);
   COLarray<SDBid> IdList;
   COLstring Payload(Size, '-');
   COLuint64 Time = time(NULL)*1000;
   for (int i = 1; i <= Count; ++i) {
      std::shared_ptr<SDBmessage> pMessage(new SDBmessage("test", Payload, Time, i));
      IdList.push_back(pMessage->Id);
      pList->push_back(pMessage);
   }
   return IdList;
}

static void testCacheRemove(){
   COL_FUNCTION(testCacheRemove);
   SDBcomponent Queue;
   COLarray<SDBid> IdList = SDBTgenerateTestData(Queue.pQueueCache, 5);
   // evict the first 3 messages
   SDBevictMessages(Queue, IdList[2]);
   UNIT_ASSERT_EQUALS(4, Queue.pQueueCache->front().get()->Id.MessageId.Index);

   // evict all 5 messages
   SDBevictMessages(Queue, IdList[4]);
   UNIT_ASSERT_EQUALS(0, Queue.pQueueCache->size());

   // set consumer A and B's dequeue position to message 3 and 5 respectively.
   // evict messages 1-4 from cache and make sure consumer A's DirtyCache is set.
   SDBTgenerateTestData(Queue.pQueueCache, 5);
   auto& Consumers = Queue.Consumers;
   Consumers["A"].pDequeue = SDBcreateDequeue(IdList[0].MessageId, NULL, "");
   Consumers["A"].Position = IdList[2];
   Consumers["B"].pDequeue = SDBcreateDequeue(IdList[0].MessageId, NULL, "");
   Consumers["B"].Position = IdList[4];
   SDBevictMessages(Queue, IdList[3]);
   UNIT_ASSERT( Consumers["A"].pDequeue->dirtyCache());
   UNIT_ASSERT(!Consumers["B"].pDequeue->dirtyCache());
}

static void testEvictee(){
   COL_FUNCTION(testEvictee);
   SDBid Evictee;
   COLlist<std::shared_ptr<SDBmessage>> Cache;
   // 13 messages in cache
   COLarray<SDBid> IdList = SDBTgenerateTestData(&Cache, 13, SDB_MAX_CACHE_SIZE/10);
   // test case: cache size > SDB_MAX_CACHE_SIZE, limit at message 1
   Evictee = SDBdetermineEvictee(&Cache, IdList[0]);
   UNIT_ASSERT_EQUALS(IdList[0], Evictee);
   // test case: cache size > SDB_MAX_CACHE_SIZE, limit at message 12, we can evict the first 4 to keep memory < SDB_MAX_CACHE_SIZE
   Evictee = SDBdetermineEvictee(&Cache, IdList[11]);
   UNIT_ASSERT_EQUALS(IdList[3], Evictee);
   // test case: cache size > SDB_MAX_CACHE_SIZE, limit at message 3, we can evict the first 3
   Evictee = SDBdetermineEvictee(&Cache, IdList[2]);
   UNIT_ASSERT_EQUALS(IdList[2], Evictee);

   Cache.clear();
   IdList.clear();
   IdList = SDBTgenerateTestData(&Cache, 10);
   // test case: cache size < SDB_MAX_CACHE_SIZE, evict nothing
   Evictee = SDBdetermineEvictee(&Cache, IdList[7]);
   UNIT_ASSERT(Evictee.MessageId.isNull());
   // test case: Limit < first cached message, evict nothing
   SDBid Limit = IdList[0];
   Limit.MessageId.Index--;
   Evictee = SDBdetermineEvictee(&Cache, Limit);
   UNIT_ASSERT(Evictee.MessageId.isNull());
}

static void testCleanup() {
   COL_FUNCTION(testCleanup);
   SDBcomponent Queue;
   time_t Now = time(NULL);
   // a slow consumer
   SDBconsumer& A = Queue.Consumers["A"];
   A.pDequeue = SDBcreateDequeue(SDBmessageId("test"), NULL, "");
   A.pDequeue->setLastCacheActivity(Now - SDB_SLOW_CONSUMER_LIMIT - 5);
   A.Position.MessageId = SDBmessageId("test", A.pDequeue->lastCacheActivity() * 1000, 1);
   // a normal consumer
   SDBconsumer& B = Queue.Consumers["B"];
   B.pDequeue = SDBcreateDequeue(SDBmessageId("test"), NULL, "");
   B.pDequeue->setLastCacheActivity(Now - 5);
   B.Position.MessageId = SDBmessageId("test", B.pDequeue->lastCacheActivity() * 1000, 2);
   // a dead consumer
   SDBconsumer& C = Queue.Consumers["C"];

   Queue.LastJournaledId.MessageId = SDBmessageId("test", Now * 1000, 3);

   SDBid Limit = SDBdetermineCleanupee(Queue);
   UNIT_ASSERT_EQUALS(B.Position, Limit);
}

void testCache(UNITapp* pApp){
   COL_FUNCTION(testCache);
   pApp->add("cache/add", &testCacheAdd);
   pApp->add("cache/remove", &testCacheRemove);
   pApp->add("cache/evictee", &testEvictee);
   pApp->add("cache/cleanup", &testCleanup);
}
