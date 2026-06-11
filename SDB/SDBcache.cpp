// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBcache
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Thursday 09 March 2023 - 03:49PM
// ---------------------------------------------------------------------------
#include <time.h>

#include <SDB/SDBcache.h>
#include <SDB/SDBenvelope.h>
#include <SDB/SDBsystem.h>

#include <SCK/SCKloop.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int SDBcalculateLogSize(const COLlist<std::shared_ptr<SDBmessage>>* pMessageList) {
   COL_FUNCTION(SDBcalculateLogSize);
   int Size = 0;
   time_t DateTime = pMessageList->front()->Id.MessageId.DateTime / 1000;
   for (auto Message = pMessageList->cbegin(); Message != pMessageList->cend(); Message++) {
      if (DateTime != (*Message)->Id.MessageId.DateTime / 1000) { break; }
      Size += SDBenvelopeSizeFromContent((SDBcontent*)(*Message).get());
   }
   COL_VAR(Size);
   return Size;
}

void SDBcacheEnqueuedMessages(SDBcomponent& Queue, COLlist<std::shared_ptr<SDBmessage>>* pComittedMessages) {
   COL_FUNCTION(SDBcacheEnqueuedMessages);
   if (0 == Queue.Consumers.size()) {
      COL_TRC("No one cares. NOP");
      return;
   }
   // TODO: may need to evict messages here if the cache is already too big
   COLlist<std::shared_ptr<SDBmessage>>* pCache = Queue.pQueueCache;
   while (pComittedMessages->size()) {
      pCache->push_back(pComittedMessages->front());
      pComittedMessages->pop_front();
   }
}

SDBid SDBdetermineEvictee(COLlist<std::shared_ptr<SDBmessage>>* pCache, const SDBid& Limit) {
   COL_FUNCTION(SDBdetermineEvictee);
   SDBid Result;    // Null
   if (pCache->size() == 0) { return Result; }
   auto pMessage = pCache->begin();
   int CacheSize = SDBcalculateLogSize(pCache);
   do {
      SDBmessage* pMsg = (*pMessage).get();
      const SDBid& MsgId = pMsg->Id;
      if (MsgId > Limit) { break; }          // don't evict anything pass Limit
      if (CacheSize < SDB_MAX_CACHE_SIZE) { break; }   // make sure we have 64 MiB in cache to improve performance
      COL_VAR(CacheSize);
      CacheSize -= SDBmessageSize(pMsg);
      Result = MsgId;
      pMessage++;
   } while (pMessage != pCache->end());
   return Result;
}

void SDBevictMessages(SDBcomponent& Queue, const SDBid& Upto) {
   COL_FUNCTION(SDBevictMessages);
   COLlist<COLstring> ConsumerList;
   for (auto Consumer = Queue.Consumers.begin(); Consumer != Queue.Consumers.end(); Consumer++) {
      if (Consumer.value().Position < Upto) {
         ConsumerList.push_back(Consumer.key());
      }
   }
   SDBremoveMessagesFromList(Queue.pQueueCache, Upto);
   // update consumers
   for (auto Consumer = ConsumerList.begin(); Consumer != ConsumerList.end(); Consumer++) {
      SDBdequeue* pDequeue = Queue.Consumers[*Consumer].pDequeue;
      if (pDequeue == NULL) { continue; }
      SDBpolluteCache(pDequeue);
   }
}

void SDBevictLoggedMessages(SDBcomponent& Queue, const SDBid& LastLoggedId) {
   COL_FUNCTION(SDBevictLoggedMessages);
   SDBid Limit = SDBdetermineEvictee(Queue.pQueueCache, LastLoggedId);
   if (Limit.MessageId.isNull()) {
      // COLcout << "Don't need to evict any messages." << newline;
      COL_TRC("Don't need to evict any messages.");
      return;
   }
   // COLcout << "Evicting messages from " << Queue.pQueueCache->front().get()->Id << " to " << Limit << newline;
   SDBevictMessages(Queue, Limit);
}

void SDBremoveMessagesFromList(COLlist<std::shared_ptr<SDBmessage>>* pList, const SDBid& UpTo) {
   COL_FUNCTION(SDBremoveMessagesFromList);
   while (pList->size()) {
      SDBmessage* pMessage = pList->front().get();
      SDBid CurrenId(pMessage->Id);
      COL_VAR(CurrenId);
      if (CurrenId > UpTo) { return; }
      pList->pop_front();
      if (CurrenId == UpTo) { return; }
   }
}

SDBid SDBdetermineCleanupee(SDBcomponent& Queue) {
   COL_FUNCTION(SDBdetermineCleanupee);
   time_t Now = time(NULL);
   SDBid Result = Queue.LastJournaledId;
   const auto& Consumers = Queue.Consumers;
   for (auto Consumer = Consumers.cbegin(); Consumer != Consumers.cend(); Consumer++) {
      const SDBdequeue* pDequeue = Consumer.value().pDequeue;
      if (pDequeue == NULL) {
         COL_TRC("Consumer " << Consumer.key() << " is OFF. Skip");
         continue;
      }
      if (Now - pDequeue->lastCacheActivity() >= SDB_SLOW_CONSUMER_LIMIT) {
         COL_TRC("Clean up messages cached for " << Consumer.key());
         continue;
      }
      // find the position of the slowest consumer that is not considered a slow consumer
      COL_VAR(Consumer.value().Position);
      if (Consumer.value().Position < Result) {
         Result = Consumer.value().Position;
      }
   }
   return Result;
}

static void SDBcleanUpCacheForQueue(SDBcomponent& Queue) {
   COL_FUNCTION(SDBcleanUpCacheForQueue);
   SDBid Limit = SDBdetermineCleanupee(Queue);
   if (Limit.MessageId.isNull()) { return; }
   COL_TRC("Remove messages up to and including " << Limit << " from cache");
   SDBevictMessages(Queue, Limit);
}

void SDBcleanUpCache(SDBsystem* pSystem) {
   COL_FUNCTION(SDBcleanUpCache);
   auto& Components = pSystem->ComponentMap;
   for (auto Queue = Components.begin(); Queue != Components.end(); Queue++) {
      if (Queue.value().pQueueCache->size() == 0) { continue;}
      SDBcleanUpCacheForQueue(Queue.value());
   }
   pSystem->CacheCleanupTimer = pSystem->pLoop->addTimer(COLnewClosure0(&SDBcleanUpCache, pSystem), 10000, false, "SDBcleanupCache"); // 10s too long?
}

void SDBstopCacheCleanupCycle(SDBsystem* pSystem) {
   COL_FUNCTION(SDBstopCacheCleanupCycle);
   if (pSystem->CacheCleanupTimer) {
      pSystem->pLoop->cancelTimer(pSystem->CacheCleanupTimer);
   }
}
