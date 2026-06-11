// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBdequeueSingle
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Thursday 16 March 2023 - 09:55PM
// ---------------------------------------------------------------------------

#include <SDB/SDBdequeueSingle.h>

#include <SCK/SCKloop.h>

#include <SDB/SDBmessage.h>
#include <SDB/SDBreader.h>
#include <SDB/SDBcapi.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

SDBdequeueSingle::~SDBdequeueSingle() {
   COL_METHOD(SDBdequeueSingle::~SDBdequeueSingle);
   if (pReader) { SDBdeleteReader(pReader); }
}

SDBdequeueSingle* SDBcreateDequeueSingle(const SDBmessageId& Config, COLlist<std::shared_ptr<SDBmessage>>* pQueueCache, const COLstring& EncryptionKey, const COLstring& LogDir) {
   COL_FUNCTION(SDBcreateDequeueSingle);
   COL_VAR(Config);
   SDBdequeueSingle* pDequeue    = new SDBdequeueSingle();
   pDequeue->Position.MessageId = Config;
   pDequeue->pQueueCache   = pQueueCache;
   pDequeue->LogDir        = LogDir;
   pDequeue->EncryptionKey = EncryptionKey;
   // A message may be dequeued multiple times if we failed to process and commit it. Enable ManualRemove for the reader
   pDequeue->pReader = SDBcreateDequeueReader(Config, EncryptionKey, true, LogDir);
   pDequeue->LastCacheActivity = 0;
   // turn off prefetch so it can't interfere with SDBmovePosition() during clear queue count or reposition queue
   pDequeue->pReader->PreFetch = false;
   return pDequeue;
}

void SDBsetOnDequeuedSingle(SDBdequeueSingle* pDequeue, COLauto<COLclosure2<int, SDBmessage*>> pCallback) {
   COL_FUNCTION(SDBsetOnDequeuedSingle);
   pDequeue->pOnDequeued = pCallback;
}

void SDBpolluteCacheSingle(SDBdequeueSingle* pDequeue) {
   pDequeue->DirtyCache = true;
}

static SDBmessage* SDBdequeueFromCache(SDBdequeueSingle* pDequeue) {
   COL_FUNCTION(SDBdequeueFromCache);
   COLlist<std::shared_ptr<SDBmessage>>* pCache = pDequeue->pQueueCache;
   SDBmessage* pMessage = NULL;
   for (auto Message = pCache->begin(); Message != pCache->end(); Message++) {
      COL_VAR2((*Message).get()->Id, pDequeue->Position);
      if ((*Message).get()->Id <= pDequeue->Position) { continue; }
      pMessage = (*Message).get();
      break;
   }
   // Don't pop the message from cache in case we need to dequeue it again. It is removed by SDBremoveCommittedMessage()
   if (pMessage) { time(&pDequeue->LastCacheActivity); }
   return pMessage;
}

static void SDBdequeueHandleCorruptedRegions(SDBreader* pReader, const COLstring& ComponentId) {
   COL_FUNCTION(SDBdequeueHandleCorruptedRegions);
   COLstring Error = SDBreaderCorruptedRegionErrorMessage(pReader);
   if (!Error.is_null()) {
      SDBIlogComponent(Error, ComponentId, "#error");
      pReader->Corruptions.clear();    // clear corruptions to avoid logging them multiple times
   }
}

// TODO: refactor the if mess below
static void SDBmessageDequeuedFromFileSingle(int Code, SDBmessage* pMessage, SDBdequeueSingle* pDequeue, SCKloop* pLoop, COLclosure2<int, SDBmessage*>* pCallback) {
   COL_FUNCTION(SDBmessageDequeuedFromFileSingle);
   COLlist<std::shared_ptr<SDBmessage>>* pCache = pDequeue->pQueueCache;
   COL_VAR2(Code, pCache->size());
   SDBdequeueHandleCorruptedRegions(pDequeue->pReader, pDequeue->Position.MessageId.ComponentId);
   if (Code) {
      COLASSERT(!pMessage);   // pMessage should be NULL when Code != 0
      if (SDB_END_OF_LOG != Code) {
         COL_ERR("Encountered a dequeue error. " << Code);
         pCallback->run(Code, NULL);
         return;
      }
      if (pCache->size() == 0) {
         COL_TRC("Dequeued all messages from file and there is nothing in the cache. Done");
         pCallback->run(Code, NULL);
         return;
      } else {
         COL_TRC("Reached the end of file. Switching dequeue source from log file to cache.");
         SDBdeleteReader(pDequeue->pReader);
         pDequeue->pReader = NULL;
         pMessage = SDBdequeueFromCache(pDequeue);
         if (pMessage == NULL) {
            COL_TRC("No new message found in cache. Return EOF.");
            pCallback->run(SDB_END_OF_LOG, NULL);
            return;
         }
      }
   } else if (pCache->size() && pCache->front().get()->Id == pMessage->Id) {
      COL_TRC("Found dequeued message in cache. Switching dequeue source from log file to cache.");
      SDBdeleteReader(pDequeue->pReader);
      pDequeue->pReader = NULL;
      pMessage = SDBdequeueFromCache(pDequeue);
      COLASSERT(pMessage);    // I don't think pMessage can be NULL here. Let's see if I missed something
   }
   // pCallback->run(0, pMessage);
   // use timer to solve the deep nesting problem if pCallback calls SDBdequeueMessage()
   pLoop->addTimer(COLnewClosure0(pCallback, &COLclosure2<int, SDBmessage*>::run, 0, pMessage));
   // addTimer() is more efficient, but it breaks in unit tests (run on worker threads)
   // pLoop->post(COLnewClosure0(pCallback, &COLclosure2<int, SDBmessage*>::run, 0, pMessage));
}

void SDBdequeueMessageSingle(SDBdequeueSingle* pDequeue, SCKloop* pLoop) {
   COL_FUNCTION(SDBdequeueMessageSingle);
   COLclosure2<int, SDBmessage*>* pCallback = pDequeue->pOnDequeued;
   COLASSERT(pCallback != NULL);
   if (pDequeue->pReader) {
      // we need to dequeue from log file for queued messages that are only in log files
      SDBreaderFetchMessage(pDequeue->pReader, pLoop, COLnewClosure2(&SDBmessageDequeuedFromFileSingle, pDequeue, pLoop, pCallback));
   } else if (pDequeue->DirtyCache) {
      COL_TRC("Cache had been modified. Must dequeue from file");
      pDequeue->DirtyCache = false;
      // A message may be dequeued multiple times if we failed to process and commit it. Enable ManualRemove for the reader
      pDequeue->pReader = SDBcreateDequeueReader(pDequeue->Position.MessageId, pDequeue->EncryptionKey, true, pDequeue->LogDir);
      pDequeue->LastCacheActivity = 0;
      SDBreaderFetchMessage(pDequeue->pReader, pLoop, COLnewClosure2(&SDBmessageDequeuedFromFileSingle, pDequeue, pLoop, pCallback));
   } else {
      // reader only gets destroyed when all log-only messages have been dequeued
      SDBmessage* pMessage = SDBdequeueFromCache(pDequeue);
      if (pMessage == NULL) {
         COL_TRC("End of Queue");
         pCallback->run(SDB_END_OF_LOG, NULL);
         return;
      }
      COL_VAR(*pMessage);
      // Don't pop the message from cache in case we need to dequeue it again. It is removed by SDBremoveCommittedMessage()
      // pCallback->run(0, pMessage);
      // use timer to solve the deep nesting problem if pCallback calls SDBdequeueMessage()
      pLoop->addTimer(COLnewClosure0(pCallback, &COLclosure2<int, SDBmessage*>::run, 0, pMessage));
      // addTimer() is more efficient, but it breaks in unit tests (run on worker threads)
      // pLoop->post(COLnewClosure0(pCallback, &COLclosure2<int, SDBmessage*>::run, 0, pMessage));
   }
   pDequeue->Busy = true;
}

void SDBremoveCommittedMessageSingle(SDBdequeueSingle* pDequeue, SDBid Position) {
   COL_FUNCTION(SDBremoveCommittedMessageSingle);
   COL_VAR(Position);
   pDequeue->Position = Position;
   if (pDequeue->pReader) {
      COL_TRC("Remove committed message from reader cache");
      SDBreaderRemoveMessageFromCache(pDequeue->pReader, Position);
   } else {
      COL_TRC("Remove committed message from system cache");
      SDBIcleanUpQueueCache(Position);
   }
   pDequeue->Busy = false;
}

void SDBmovePositionSingle(SDBdequeueSingle* pDequeue, SDBid NewPosition, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBmovePositionSingle);
   COLASSERT(pDequeue->Position.MessageId.ComponentId == NewPosition.MessageId.ComponentId);
   if(pDequeue->Position == NewPosition) { return; }
   COLlist<std::shared_ptr<SDBmessage>>* pCache = pDequeue->pQueueCache;
   if(pCache->size()) {
      SDBid CurrentPosition   = pDequeue->Position;
      SDBid MinCachedPosition = pCache->front().get()->Id;
      SDBid MaxCachedPosition = pCache->back().get()->Id;
      if(NewPosition > CurrentPosition && NewPosition >= MinCachedPosition && NewPosition <= MaxCachedPosition) {
         COL_TRC("Moving to position within the cached region");
         while(pCache->size()) {
            const SDBid& Id = pCache->front().get()->Id;
            if(Id > NewPosition) { break; }
            pCache->pop_front();
         }
      }
   } else {
      COL_TRC("Moving to position outside the cached region");
      // recreate reader with manual message removal
      if(pDequeue->pReader) { SDBdeleteReader(pDequeue->pReader); }
      pDequeue->pReader = SDBcreateDequeueReader(NewPosition.MessageId, pDequeue->EncryptionKey, true, LogRootDir);
      pDequeue->LastCacheActivity = 0;
      // turn off prefetch so it can't interfere with SDBmovePosition() during clear queue count or reposition queue
      pDequeue->pReader->PreFetch = false;
   }
   pDequeue->Position = NewPosition;
}
