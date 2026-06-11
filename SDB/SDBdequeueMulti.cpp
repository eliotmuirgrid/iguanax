// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBdequeueMulti
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Thursday 16 March 2023 - 09:55PM
// ---------------------------------------------------------------------------

#include <SDB/SDBdequeueMulti.h>

#include <SCK/SCKloop.h>

#include <SDB/SDBmessage.h>
#include <SDB/SDBqueueReader.h>
#include <SDB/SDBcapi.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void SDBdeleteReader(SDBqueueReaderState* pState) {
   SDBdeleteReader(pState->pReader);
   pState->pReader = NULL;
   pState->Position.reset();
   pState->Code = 0;
}

static void SDBresetMessages(SDBdequeueMulti* pDequeue) {
   auto Entry = pDequeue->Messages.begin();
   while (Entry != pDequeue->Messages.end()) {
      delete Entry.value();
      Entry = pDequeue->Messages.erase(Entry);
   }
}

static void SDBresetReaderState(SDBdequeueMulti* pDequeue) {
   SDBdeleteReader(&pDequeue->Reader1);
   SDBdeleteReader(&pDequeue->Reader2);
   SDBresetMessages(pDequeue);
}

SDBdequeueMulti::~SDBdequeueMulti() {
   COL_METHOD(SDBdequeueMulti::~SDBdequeueMulti);
   SDBresetReaderState(this);
}

SDBdequeueMulti* SDBcreateDequeueMulti(const SDBmessageId& Config, COLlist<std::shared_ptr<SDBmessage>>* pQueueCache, const COLstring& EncryptionKey, const COLstring& LogDir1, const COLstring& LogDir2) {
   COL_FUNCTION(SDBcreateDequeueMulti);
   COL_VAR(Config);
   SDBdequeueMulti* pDequeue    = new SDBdequeueMulti();
   pDequeue->pQueueCache   = pQueueCache;
   pDequeue->LogDir1       = LogDir1;
   pDequeue->LogDir2       = LogDir2;
   pDequeue->EncryptionKey = EncryptionKey;
   pDequeue->Reader1.pReader = SDBcreateDequeueReader(Config, EncryptionKey, LogDir1);
   pDequeue->Reader2.pReader = SDBcreateDequeueReader(Config, EncryptionKey, LogDir2);
   pDequeue->LastCacheActivity = 0;
   return pDequeue;
}

void SDBsetOnDequeuedMulti(SDBdequeueMulti* pDequeue, COLauto<COLclosure2<int, SDBmessage*>> pCallback) {
   COL_FUNCTION(SDBsetOnDequeuedMulti);
   pDequeue->pOnDequeued = pCallback;
}

void SDBpolluteCacheMulti(SDBdequeueMulti* pDequeue) {
   pDequeue->DirtyCache = true;
}

static void SDBdequeueRespond(SDBdequeueMulti* pDequeue, SCKloop* pLoop, COLclosure2<int, SDBmessage*>* pCallback, int Code, SDBmessage* pMessage) {
   COL_FUNCTION(SDBdequeueRespond);
   // Use timer to solve the deep nesting problem if pCallback calls SDBdequeueMessage()
   // pCallback->run(Code, pMessage);
   pDequeue->Responded = true;
   COL_VAR(pCallback);
   pLoop->addTimer(COLnewClosure0(pCallback, &COLclosure2<int, SDBmessage*>::run, Code, pMessage));
   // pLoop->post(COLnewClosure0(pCallback, &COLclosure2<int, SDBmessage*>::run, Code, pMessage));
}

// try to dequeue from messages
// if pDequeue->Messages empty, caller should try read logs
bool SDBdequeueFromMessagesMulti(SDBdequeueMulti* pDequeue, SCKloop* pLoop, COLclosure2<int, SDBmessage*>* pCallback) {
   COL_FUNCTION(SDBdequeueFromMessagesMulti);
   if (0 == pDequeue->Messages.size()) {
      COL_TRC("No message left");
      return false;
   }
   int Code = 0;
   SDBmessage* pMessage = pDequeue->Messages.begin().value();
   COL_VAR2(pDequeue->Position.QueueId, pMessage->Id.QueueId);
   if (pDequeue->Position.QueueId + 1 != pMessage->Id.QueueId) {
      COL_TRC("Expecting message with QID " << pDequeue->Position.QueueId + 1 << ". Got " << pMessage->Id.QueueId);
      COL_VAR2(pDequeue->Reader1.Position.QueueId, pDequeue->Reader2.Position.QueueId);
      if (pDequeue->Reader1.Position.QueueId < pDequeue->Position.QueueId + 1) {
         COL_TRC("it may be in " << pDequeue->Reader1.pReader->Config.LogRootDir << ", try fetch");
         return false;
      }
      if (pDequeue->Reader2.Position.QueueId < pDequeue->Position.QueueId + 1) {
         COL_TRC("it may be in " << pDequeue->Reader2.pReader->Config.LogRootDir << ", try fetch");
         return false;
      }
      COL_TRC("Message with next QID not found in either log dirs, report error");
      Code = SDB_BAD_QID;     // next queued message missing in both log dirs
      pMessage = nullptr;
   } else {
      COL_TRC("message with next QID found, serving request");
      time(&pDequeue->LastCacheActivity);
   }
   SDBdequeueRespond(pDequeue, pLoop, pCallback, Code, pMessage);
   return true;
}

static void SDBdequeueFromCache(SDBdequeueMulti* pDequeue, SCKloop* pLoop, COLclosure2<int, SDBmessage*>* pCallback) {
   COL_FUNCTION(SDBdequeueFromCache);
   COLlist<std::shared_ptr<SDBmessage>>* pCache = pDequeue->pQueueCache;
   int Code = 0;
   SDBmessage* pMessage = NULL;
   for (auto Message = pCache->begin(); Message != pCache->end(); Message++) {
      COL_VAR3((*Message).get()->Id, pDequeue->Position, pDequeue->Position.QueueId);
      if ((*Message).get()->Id <= pDequeue->Position) { continue; }
      pMessage = (*Message).get();
      break;
   }
   if (pMessage) {
      time(&pDequeue->LastCacheActivity);
   } else {
      Code = SDB_END_OF_LOG;
   }
   SDBdequeueRespond(pDequeue, pLoop, pCallback, 0, pMessage);
}

static void SDBdequeueHandleCorruptedRegions(SDBqueueReader* pReader, const COLstring& ComponentId) {
   COL_FUNCTION(SDBdequeueHandleCorruptedRegions);
   COLstring Error = SDBreaderCorruptedRegionErrorMessage(pReader);
   if (!Error.is_null()) {
      SDBIlogComponent(Error, ComponentId, "#error");
      pReader->Corruptions.clear();    // clear corruptions to avoid logging them multiple times
   }
}

// if we are first, and set dequeue position, continue
//                  but can't set position, wait for other
// if we are 2nd,   and pos not set, it means first couldn't set it
//                  and set dequeue pos, continue
//                  but can't set pos, report error
static bool SDBinitDequeueRegularPosition(SDBdequeueMulti* pDequeue) {
   COL_FUNCTION(SDBinitDequeueRegularPosition);
   const SDBmessageId& StartPosition = pDequeue->Reader1.pReader->StartPosition;
   SDBmessage* pMessage = pDequeue->Messages.begin().value();
   COL_VAR2(StartPosition, pMessage->Id);
   if (StartPosition == pMessage->Id.MessageId) {
      COL_TRC("start position found. remove the 1st message");
      pDequeue->Position = pMessage->Id;
      if (SDB_INVALID_QID == pDequeue->Position.QueueId) {   // older logs doesn't have QID and SID
         COL_WRN("message doesn't have QID, init to default value");
         pDequeue->Position.QueueId = SDB_DEFAULT_QID;
      }
      COL_VAR(pDequeue->Position);
      // Remove the start position message because it had been dequeued already
      delete pMessage;
      pDequeue->Messages.erase(pDequeue->Messages.begin());
      return true;
   }
   COL_TRC("Failed to set position");
   return false;
}

bool SDBinitDequeuePositionMulti(SDBdequeueMulti* pDequeue, SDBqueueReaderState* pOtherReader, SCKloop* pLoop, COLclosure2<int, SDBmessage*>* pCallback) {
   COL_FUNCTION(SDBinitDequeuePositionMulti);
   COL_VAR(pDequeue->Messages.size());
   if (pDequeue->Messages.size() == 0) { return false; }
   COL_TRC("Reader fetched the first batch of messages. Set position");
   // Reader always returns the start position message as the first message unless start position is BOQ
   const SDBmessageId& StartPosition = pDequeue->Reader1.pReader->StartPosition;     // note Reader1 and Reader2 should have the same StartPosition
   if (SDBisDefaultPosition(StartPosition)) {
      if (pOtherReader->Fetching) {
         // we are the first reader but we may not have the actual first message in the queue
         // wait for the 2nd reader so we have more confidence.
         COL_TRC("start position is beginning of queue. wait for the other reader");
         return false;
      } else {
         COL_TRC("start position is beginning of queue. keep the 1st message");
         pDequeue->Position = pDequeue->Messages.begin().value()->Id;
         COL_VAR(pDequeue->Position);
         if (SDB_INVALID_QID == pDequeue->Position.QueueId) {   // older logs doesn't have QID and SID
            COL_WRN("message doesn't have QID, init to default value");
            pDequeue->Position.QueueId = SDB_DEFAULT_QID;
         } else {
            COL_WRN("set dequeue position to a fictitious position that's before the first message in the queue");
            // To make sure the 1st message is included, construct a fictitious position by decrementing everything by 1
            // and use it as the pDequeue->Position
            pDequeue->Position.MessageId.Index--;
            pDequeue->Position.SequenceId--;
            pDequeue->Position.QueueId--;
            // note that if the very first message is an enqueued message, its SequenceId = 1 and QueueId = 1
            COLASSERT(pDequeue->Position.SequenceId >= 0);
            COLASSERT(pDequeue->Position.QueueId >= 0);
         }
         COL_VAR(pDequeue->Position);
         return true;
      }
   } else {
      if (SDBinitDequeueRegularPosition(pDequeue)) {
         return true;
      }
      if (pOtherReader->Fetching) {
         COL_TRC("wait for the other reader");
      } else {
         COL_TRC("Can't find start position message. report SDB_READER_BAD_CONFIG");
         SDBdequeueRespond(pDequeue, pLoop, pCallback, SDB_READER_BAD_CONFIG, NULL);
      }
      return false;
   }
}

// what if one of the reader reports SDB_READER_BAD_CONFIG? do we continue with one reader or error?
// called by both log dir readers
// handle SDB_READER_BAD_CONFIG
// if dequeue request already served (by the other reader), we are done
// else try to serve
// if success, don't care even if Code != 0
// if fail, check the other reader
// if the other reader hasn't finished fetching yet, wait for it
// if the other reader finished fetching before us, deal with various Code cases
void SDBmessageDequeuedFromFileMulti(int Code, SDBqueueReaderState* pOtherReader, SDBdequeueMulti* pDequeue, SCKloop* pLoop, COLclosure2<int, SDBmessage*>* pCallback) {
   COL_FUNCTION(SDBmessageDequeuedFromFileMulti);
   if (pDequeue->Position.MessageId.ComponentId.is_null()) {
      if (!SDBinitDequeuePositionMulti(pDequeue, pOtherReader, pLoop, pCallback)) { return; }
   }
   if (pDequeue->Responded) {
      COL_TRC("Already responded to the dequeue request. done");
      return; // unless the current reader is behind (reader SID < dequeue SID)
   }
   // either we are the first reader returned or the first reader couldn't serve due to error
   if (SDB_READER_BAD_CONFIG == Code) {
      if (SDB_READER_BAD_CONFIG == pOtherReader->Code) {
         COL_TRC("Both reader reported SDB_READER_BAD_CONFIG");
         SDBdequeueRespond(pDequeue, pLoop, pCallback, SDB_READER_BAD_CONFIG, NULL);
         return;
      }
      if (pOtherReader->Fetching) {
         COL_TRC("One reader reported SDB_READER_BAD_CONFIG, wait for the other");
         return;
      }
      // TODO:
      // 1st reader populates pDequeueMessages
      // SDBdequeueMessage() called again but next QID message missing
      // SDBdequeueFromMessagesMulti() waits for 2nd reader to return
      // 2nd reader report SDB_READER_BAD_CONFIG
      // we reach here and do nothing
   }
   COLlist<std::shared_ptr<SDBmessage>>* pCache = pDequeue->pQueueCache;
   if (pCache->size() && pCache->front().get()->Id == pDequeue->Messages.begin().value()->Id) {
      COL_TRC("Found dequeued message in cache. Switching dequeue source from file to cache");
      // Note: as long as one of the reader catches up to the cache, we start dequeue from cache
      pDequeue->DequeueFromLogs = false;
      // In case DirtyCache is set while we dequeue from logs, reset it when we switch to cache
      pDequeue->DirtyCache = false;
      SDBresetReaderState(pDequeue);   // TODO: may cause crash when other reader runs callback
      SDBdequeueFromCache(pDequeue, pLoop, pCallback);
      return;
   }
   if (SDBdequeueFromMessagesMulti(pDequeue, pLoop, pCallback)) {
      COL_TRC("Served dequeue request. done");
      // as long as we can serve the dequeue request, we don't care about other errors
      return;
   }
   // reader returned code 0 with messages, but missing next QID
   // other reader may have returned already, but its last QID < dequeue QID
   if (pOtherReader->Fetching) {
      COL_TRC("Can't serve. Wait for other reader");
      return;
   }
   if (SDB_END_OF_LOG == Code && SDB_END_OF_LOG == pOtherReader->Code) {
      COL_TRC("Both reader reported end of log");
      if (pCache->size()) {   // dequeued yesterday's log from file, and today's logs are in cache
         COL_TRC("Reached the end of log. Switching dequeue source from file to cache.");
         pDequeue->DequeueFromLogs = false;
         SDBresetReaderState(pDequeue);
         SDBdequeueFromCache(pDequeue, pLoop, pCallback);
      } else {
         COL_TRC("Dequeued all messages from file and nothing in the cache. EOQ");
         SDBdequeueRespond(pDequeue, pLoop, pCallback, SDB_END_OF_LOG, NULL);
      }
      return;
   }
   if (pOtherReader->Code) {
      COL_TRC("Both reader reported error");
      // Don't know which reader error we should use. Combine them.
      Code = abs(pDequeue->Reader1.Code) * 100000 + abs(pDequeue->Reader2.Code);
      SDBdequeueRespond(pDequeue, pLoop, pCallback, Code, NULL);
   }
}

static void SDBmergeMessageFetchedByReader(SDBdequeueMulti* pDequeue, SDBqueueReader* pReader) {
   COL_FUNCTION(SDBmergeMessageFetchedByReader);
   COL_VAR(pDequeue->Position);
   auto ppMessage = pReader->Messages.begin();
   while (ppMessage != pReader->Messages.end()) {
      SDBmessage* pMessage = *ppMessage;
      if (pDequeue->Position.MessageId.ComponentId.is_null()) {
         COL_TRC("dequeue position not set yet");
         if (pDequeue->Messages.count(pMessage->Id)) {
            COL_TRC("message already exists");
            delete pMessage;
         } else {
            COL_TRC("add message");
            pDequeue->Messages[pMessage->Id] = pMessage;
         }
      }
      else if (pMessage->Id > pDequeue->Position && pDequeue->Messages.count(pMessage->Id) == 0) {
         COL_TRC("Message after dequeue position and not added yet");
         pDequeue->Messages[pMessage->Id] = pMessage;
      } else {
         COL_TRC("Message before dequeue position or already added");
         delete pMessage;
      }
      ppMessage = pReader->Messages.erase(ppMessage);
   }
}

static void SDBmessageFetchedByReader(int Code, SDBqueueReaderState* pReaderState, SDBqueueReaderState* pOtherReader, SDBdequeueMulti* pDequeue, SCKloop* pLoop, COLclosure2<int, SDBmessage*>* pCallback) {
   COL_FUNCTION(SDBmessageFetchedByReader);
   pReaderState->Fetching = false;
   // COLcout << pReaderState->pReader->Config.LogRootDir << " " << Code << " " << pReaderState->pReader->Messages.size() << " " << pOtherReader->Code << " " << pOtherReader->Fetching << newline;
   COL_TRC("Got results from " << pReaderState->pReader->Config.LogRootDir << ". Code = " << Code);
   pReaderState->Code = Code;
   // Note: not handling SDB_READER_BAD_CONFIG here because we need to wait for the other reader.
   // SDB_READER_BAD_CONFIG is handled later in SDBmessageDequeuedFromFileMulti()
   SDBqueueReader* pReader = pReaderState->pReader;
   if (pReader->Messages.size()) {     // merge messages into Messages, even if Code != 0
      pReaderState->Position = pReader->Messages.back()->Id;    // save reader position
      COL_VAR(pReaderState->Position);
      SDBmergeMessageFetchedByReader(pDequeue, pReader);
      if (!pDequeue->Position.MessageId.ComponentId.is_null() && pReaderState->Position <= pDequeue->Position && Code == 0) {     // pointless to fetch more if has error
         COL_TRC("Slow reader, fetched messages have been dequeued long ago. Fetch more");
         // optimization: in the unlikely case of pDequeue->Position being in a different file, reset reader
         pReaderState->Fetching = true;
         SDBreaderFetchMessage(pReaderState->pReader, pLoop, COLnewClosure1(&SDBmessageFetchedByReader, pReaderState, pOtherReader, pDequeue, pLoop, pCallback));
         return;
      }
   }
   SDBdequeueHandleCorruptedRegions(pReader, pDequeue->Position.MessageId.ComponentId);
   // now deal with Code in SDBmessageDequeuedFromFileMulti
   SDBmessageDequeuedFromFileMulti(Code, pOtherReader, pDequeue, pLoop, pCallback);
   COL_VAR2(pOtherReader->Position, pDequeue->Position);
   if (SDB_END_OF_LOG == Code && !pDequeue->Position.isNull() && !pOtherReader->Position.isNull() && pOtherReader->Position <= pDequeue->Position && pOtherReader->Code == 0) {
      COL_TRC("this reader reach end of log, see if other reader has more messages");
      pOtherReader->Fetching = true;
      SDBreaderFetchMessage(pOtherReader->pReader, pLoop, COLnewClosure1(&SDBmessageFetchedByReader, pOtherReader, pReaderState, pDequeue, pLoop, pCallback));
   }
}

void SDBdequeueMessageMulti(SDBdequeueMulti* pDequeue, SCKloop* pLoop) {
   COL_FUNCTION(SDBdequeueMessageMulti);
   COLclosure2<int, SDBmessage*>* pCallback = pDequeue->pOnDequeued;
   COLASSERT(pCallback != NULL);
   if (pDequeue->DequeueFromLogs) {
      if (!SDBdequeueFromMessagesMulti(pDequeue, pLoop, pCallback)) {
         // we need to dequeue from log file for queued messages that are only in log files
         pDequeue->Responded = false;
         SDBreaderFetchMessage(pDequeue->Reader1.pReader, pLoop, COLnewClosure1(&SDBmessageFetchedByReader, &pDequeue->Reader1, &pDequeue->Reader2, pDequeue, pLoop, pCallback));
         pDequeue->Reader1.Fetching = true;
         SDBreaderFetchMessage(pDequeue->Reader2.pReader, pLoop, COLnewClosure1(&SDBmessageFetchedByReader, &pDequeue->Reader2, &pDequeue->Reader1, pDequeue, pLoop, pCallback));
         pDequeue->Reader2.Fetching = true;
      }
   } else if (pDequeue->DirtyCache) {
      COL_TRC("Cache had been modified. Must dequeue from file");
      // TODO: dequeue position is still in cache possible?
      pDequeue->DirtyCache = false;
      pDequeue->DequeueFromLogs = true;
      pDequeue->Reader1.pReader = SDBcreateDequeueReader(pDequeue->Position.MessageId, pDequeue->EncryptionKey, pDequeue->LogDir1);
      pDequeue->Reader2.pReader = SDBcreateDequeueReader(pDequeue->Position.MessageId, pDequeue->EncryptionKey, pDequeue->LogDir2);
      SDBresetMessages(pDequeue);
      pDequeue->LastCacheActivity = 0;
      pDequeue->Responded = false;
      SDBreaderFetchMessage(pDequeue->Reader1.pReader, pLoop, COLnewClosure1(&SDBmessageFetchedByReader, &pDequeue->Reader1, &pDequeue->Reader2, pDequeue, pLoop, pCallback));
      pDequeue->Reader1.Fetching = true;
      SDBreaderFetchMessage(pDequeue->Reader2.pReader, pLoop, COLnewClosure1(&SDBmessageFetchedByReader, &pDequeue->Reader2, &pDequeue->Reader1, pDequeue, pLoop, pCallback));
      pDequeue->Reader2.Fetching = true;
   } else {
      SDBdequeueFromCache(pDequeue, pLoop, pCallback);
   }
   pDequeue->Busy = true;
}

void SDBremoveCommittedMessageMulti(SDBdequeueMulti* pDequeue, SDBid Position) {
   COL_FUNCTION(SDBremoveCommittedMessageMulti);
   COL_VAR(Position);
   COLASSERT(pDequeue->Messages.count(Position));
   pDequeue->Position = pDequeue->Messages[Position]->Id;
   if (pDequeue->DequeueFromLogs) {
      COL_TRC("Remove committed message from reader cache");
      delete pDequeue->Messages[Position];
      pDequeue->Messages.removeKey(Position);
      if (pDequeue->Messages.size()) {
         COL_VAR(*pDequeue->Messages.begin().value());
      }
   } else {
      COL_TRC("Remove committed message from system cache");
      SDBIcleanUpQueueCache(Position);
   }
   pDequeue->Busy = false;
}

void SDBmovePositionMulti(SDBdequeueMulti* pDequeue, SDBid NewPosition) {
   COL_FUNCTION(SDBmovePositionMulti);
   COLASSERT(pDequeue->Position.MessageId.ComponentId == NewPosition.MessageId.ComponentId);
   if (pDequeue->Position == NewPosition) { return; }
   if (pDequeue->DequeueFromLogs) {
      COL_TRC("Dequeue from logs");
      if (pDequeue->Messages.count(NewPosition)) {
         COL_TRC("Moving to position within the fetched logs");
         auto Entry = pDequeue->Messages.begin();
         while (Entry.value()->Id <= NewPosition) {
            if (Entry.value()->Id == NewPosition) {
               pDequeue->Position = Entry.value()->Id;
            }
            delete Entry.value();
            Entry = pDequeue->Messages.erase(Entry);
         }
         return;
      } else {
         COL_TRC("new position outside fetched logs, reset reader");
         pDequeue->DequeueFromLogs = false;
         SDBresetReaderState(pDequeue);
      }
   }
   else {
      COL_TRC("Dequeue from cache");
      COLlist<std::shared_ptr<SDBmessage>>* pCache = pDequeue->pQueueCache;
      SDBid CurrentPosition   = pDequeue->Position;
      SDBid MinCachedPosition = pCache->front().get()->Id;
      SDBid MaxCachedPosition = pCache->back().get()->Id;
      COL_VAR3(CurrentPosition, MinCachedPosition, MaxCachedPosition);
      if (NewPosition > CurrentPosition && NewPosition >= MinCachedPosition && NewPosition <= MaxCachedPosition) {
         COL_TRC("Moving to position within the cache");
         while (pCache->size()) {
            const SDBmessage* pMessage = pCache->front().get();
            // if NewPosition doesn't exist, that we don't hit pMessage->Id == NewPosition,
            // Position and QID set to message immediately before it
            if (pMessage->Id <= NewPosition) {
               pDequeue->Position = pMessage->Id;
               pCache->pop_front();
            } else {
               break;
            }
         }
         return;
      } else {
         COL_TRC("new position outside cache, set up reader");
         pDequeue->DirtyCache = false;
      }
   }
   // we effectively need to set up SDBdequeueMulti with NewPosition
   pDequeue->Reader1.pReader = SDBcreateDequeueReader(NewPosition.MessageId, pDequeue->EncryptionKey, pDequeue->LogDir1);
   pDequeue->Reader2.pReader = SDBcreateDequeueReader(NewPosition.MessageId, pDequeue->EncryptionKey, pDequeue->LogDir2);
   pDequeue->DequeueFromLogs = true;
   pDequeue->LastCacheActivity = 0;
   pDequeue->Position.reset();
   pDequeue->Position.QueueId = 0;
   // pDequeue->Position and QID will be set in the next SDBdequeueMessage() call
}
