// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBjournalWrite
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Tuesday 25 April 2023 - 05:09PM
// ---------------------------------------------------------------------------

#include <SDB/SDBcache.h>
#include <SDB/SDBfile.h>
#include <SDB/SDBjournalThread.h>
#include <SDB/SDBjournalWrite.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBsystem.h>
#include <SDB/SDBcryptoFile.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


static void SDBsendQueueNotEmptyNotifications(SDBcomponent& Component){
   COL_FUNCTION(SDBsendQueueNotEmptyNotifications);
   COLhashmap<COLstring, SDBconsumer>& Consumers = Component.Consumers;
   for (auto Consumer = Consumers.begin(); Consumer != Consumers.end(); Consumer++) {
      COLauto<COLclosure0> pCallback = Consumer.value().pQueueNotEmpty;
      Consumer.value().pQueueNotEmpty = NULL;
      if (pCallback) { pCallback->run(); }
   }
}

static void SDBrunPositionCommittedCallback(SDBsystem* pSystem, const COLstring& QueueId, COLhashmap<COLstring, SDBid>* pPositions) {
   COL_FUNCTION(SDBrunPositionCommittedCallback);
   COLhashmap<COLstring, SDBconsumer>& Consumers = pSystem->ComponentMap[QueueId].Consumers;
   for (auto Position = pPositions->begin(); Position != pPositions->end(); Position++) {
      const COLstring& ConsumerId = Position.key();
      auto Consumer = Consumers.find(ConsumerId);
      if (Consumer == Consumers.end()) {
         // positions committed due to unlinking components mean Consumer is already gone when we get here
         COL_WRN("Consumer " << ConsumerId << " is not found. Did it just get deleted?");
         continue;
      }
      if (Consumer.value().pPositionCommitted) {
         Consumer.value().pPositionCommitted->run();
      } else {
         COL_WRN("No PositionCommitted callback registered for " << ConsumerId << ". User cleared or repositioned queue?");
      }
   }
}

static void SDBupdateQueuePositionAndDequeue(SDBcomponent& Component, COLhashmap<COLstring, SDBid>* pNewQueuePositions) {
   COL_FUNCTION(SDBupdateQueuePositionAndDequeue);
   COLhashmap<COLstring, SDBconsumer>& Consumers = Component.Consumers;
   for (auto Position = pNewQueuePositions->begin(); Position != pNewQueuePositions->end(); Position++) {
      const COLstring& ConsumerId = Position.key();
      auto Consumer = Consumers.find(ConsumerId);
      if (Consumer == Consumers.end()) {
         // positions committed due to unlinking components mean Consumer is already gone when we get here
         COL_WRN("Consumer " << ConsumerId << " is not found. Did it just get deleted?");
         continue;
      }
      COL_VAR2(Consumer.value().Position, Position.value());
      // positions committed due to dequeue require Consumer.value().Position < Position.value()
      // positions committed due to hooking up components require Consumer.value().Position == Position.value()
      // positions committed due to queue repositioning require Consumer.value().Position > or < Position.value()
      Consumer.value().Position = Position.value();
      // delete consumer position is committed when user removes a component link, which requires both components
      // to be off so pDequeue is NULL, but check !SDBdeleteConsumer() just in case
      if (Consumer.value().pDequeue.get() && !SDBdeleteConsumer(Position.value().MessageId)) {
         SDBremoveCommittedMessage(Consumer.value().pDequeue, Position.value());
      }
   }
}

static void SDBjournalFlushDone(SDBsystem* pSystem, COLmap<COLstring, COLauto<COLhashmap<COLstring, SDBid>>>* pPositionMap, COLmap<COLstring, COLauto<COLlist<std::shared_ptr<SDBmessage>>>>* pMessagesMap, COLclosure0* pCallback){
   COL_FUNCTION(SDBjournalFlushDone);
   COL_VAR(pMessagesMap->size());
   for (auto Item = pMessagesMap->begin(); Item != pMessagesMap->end(); Item++) {
      const COLstring& ComponentId = Item.key();
      COL_VAR(ComponentId);
      if (pSystem->ComponentMap.count(ComponentId) == 0) {
         COL_ERR("Flushed messages for the non-existent component " << ComponentId);
         continue;
      }
      COLauto<COLlist<std::shared_ptr<SDBmessage>>> pMessageList = Item.value();
      COL_VAR(pMessageList->size());
      SDBmessage* pLastDataMessage = pMessageList->back().get();
      SDBcomponent& Component = pSystem->ComponentMap[ComponentId];
      Component.LastJournaledId = pLastDataMessage->Id;
      Component.pMessageCommitted->run(Component.LastJournaledId);
      SDBcacheEnqueuedMessages(Component, pMessageList);
      SDBsendQueueNotEmptyNotifications(Component);
   }
   delete pMessagesMap;

   COL_VAR(pPositionMap->size());
   for (auto i = pPositionMap->begin(); i != pPositionMap->end(); i++) {
      if (pSystem->ComponentMap.count(i->first) == 1){
         SDBcomponent& Component = pSystem->ComponentMap[i->first];
         Component.LogFlushPositions = true;
         SDBupdateQueuePositionAndDequeue(Component, i->second);
      }
      SDBrunPositionCommittedCallback(pSystem, i->first, i->second);
   }
   delete pPositionMap;

   pCallback->runAndDelete();
}

static void SDBjournalWritten(SDBsystem* pSystem, COLmap<COLstring, COLauto<COLhashmap<COLstring, SDBid>>>* pPositionMap, COLmap<COLstring, COLauto<COLlist<std::shared_ptr<SDBmessage>>>>* pMessagesMap, int* pCount, int Total, COLclosure0* pCallback) {
   COL_FUNCTION(SDBjournalWritten);
   COL_VAR2(Total, *pCount);
   if (*pCount == Total) {
      // TODO: probably need to write to at least to 2 log dirs to tolerate network partitions
      COL_TRC("A log dir finished the journal write. moving on");
      SDBjournalFlushDone(pSystem, pPositionMap, pMessagesMap, pCallback);
   }
   (*pCount)--;
   if (*pCount == 0) {
      COL_TRC("all log dirs finished the journal write");
      delete pCount;
   }
}

// WORKER THREAD
void SDBwriteJournalFile(int& FD, const COLstring* pData, const COLstring& EncryptionKey, const COLstring& LogRootDir) {
   if (-1 == FD) { FD = SDBopenLogFile(SDBjournalFile(LogRootDir)); }
   const char* pBuffer = pData->get_buffer();
   int Size = pData->size();
   COLstring EncryptedData;
   if (!EncryptionKey.is_null()) {
      COL_TRC("Encrypting data");
      SDBencryptAndSerialize(*pData, EncryptionKey, &EncryptedData);
      pBuffer = EncryptedData.get_buffer();
      Size = EncryptedData.size();
   }
   COL_TRC("Writing data to " << LogRootDir);
   int Amount = SDBwriteLogFile(FD, pBuffer, Size);
   if (getenv(SDB_SYNC_WRITE_ENV_NAME)) {
      COL_TRC("Flush journal file");
      SDBcommitLogFile(FD);
   }
}

// MAIN THREAD
void SDBwriteJournal(SDBsystem* pSystem, COLclosure0* pCallback){
   COL_FUNCTION(SDBwriteJournal);
   COLstring* pWriteBuffer = new COLstring();
   pWriteBuffer->setCapacity(pSystem->JournalBuffer.size());
   pWriteBuffer->swap(&pSystem->JournalBuffer);

   COLmap<COLstring, COLauto<COLhashmap<COLstring, SDBid>>>* pPositions = new COLmap<COLstring, COLauto<COLhashmap<COLstring, SDBid>>>();
   COLmap<COLstring, COLauto<COLlist<std::shared_ptr<SDBmessage>>>>* pMessages = new COLmap<COLstring, COLauto<COLlist<std::shared_ptr<SDBmessage>>>>();
   for (auto Item = pSystem->ComponentMap.begin(); Item != pSystem->ComponentMap.end(); Item++) {
      const COLstring& QueueId = Item.key();
      SDBcomponent& Component = Item.value();
      if (Component.pJournalPositions->size()) {
         (*pPositions)[QueueId] = Component.pJournalPositions;
         Component.pJournalPositions = new COLhashmap<COLstring, SDBid>();
      }
      if (Component.pJournalMessages->size() > 0) {
         (*pMessages)[QueueId]  = Component.pJournalMessages;
         Component.pJournalMessages = new COLlist<std::shared_ptr<SDBmessage>>();
      }
   }
   int* pCount = new int(1);

   if (!SDBroot2().is_null()) {     // write to 2nd dir journal
      COL_TRC("write to journal file in " << SDBroot2());
      (*pCount)++;
      COLstring* pWriteBuffer2 = new COLstring(*pWriteBuffer);
      // Have to copy pFlushed to pass the correct *pCount value to SDBjournalWritten()
      COL_VAR(*pCount);
      COLclosure0* pFlushed = COLnewClosure0(&SDBjournalWritten, pSystem, pPositions, pMessages, pCount, *pCount, pCallback);
      pSystem->JournalTaskQueue2.put(SDBjournalTask(SDB_JOURNAL_WRITE, pFlushed, pWriteBuffer2));
   }
   COL_TRC("write to journal file in " << SDBroot());
   COLclosure0* pFlushed = COLnewClosure0(&SDBjournalWritten, pSystem, pPositions, pMessages, pCount, *pCount, pCallback);
   pSystem->JournalTaskQueue.put(SDBjournalTask(SDB_JOURNAL_WRITE, pFlushed, pWriteBuffer));
}
