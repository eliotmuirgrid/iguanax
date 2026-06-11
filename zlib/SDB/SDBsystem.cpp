// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBsystem
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday 27 February 2023 - 01:02PM
// ---------------------------------------------------------------------------

#include <SDB/SDBsystem.h>
#include <SDB/SDBrecovery.h>
#include <SDB/SDBqueuePosition.h>
#include <SDB/SDBpurgeAuto.h>
#include <SDB/SDBjournalSwap.h>
#include <SDB/SDBinit.h>
#include <SDB/SDBflush.h>
#include <SDB/SDBenvelope.h>
#include <SDB/SDBcache.h>
#include <SDB/SDBcapi.h>

#include <SCK/SCKloop.h>

#include <FIL/FILutils.h>

#include <COL/COLthreadPool.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


SDBsystem::SDBsystem(SCKloop* Loop, CFGmap& Configs, const COLstring& EncryptionKey) : pLoop(Loop), ConfigMap(Configs), LogEncryptionKey(EncryptionKey) {
   COL_METHOD(SDBsystem::SDBsystem);
}

SDBsystem::~SDBsystem(){
   COL_METHOD(SDBsystem::~SDBsystem);
   ComponentMap.clear();
}


void SDBregisterComponent(SDBsystem* pSystem, const COLstring& ComponentId) {
   COL_FUNCTION(SDBregisterComponent);
   COLASSERT(pSystem->ComponentMap.count(ComponentId) == 0);
   pSystem->ComponentMap[ComponentId];
}

void SDBsetMessageQueued(SDBsystem* pSystem, const COLstring& QueueId, COLclosure1<SDBid>* pCallback) {
   COL_FUNCTION(SDBsetMessageQueued);
   COLASSERT(pSystem->ComponentMap.count(QueueId) == 1);
   pSystem->ComponentMap[QueueId].pMessageCommitted = pCallback;
}

static void SDBsetMessageSequenceIdAndQueueId(SDBmessage* pMessage, SDBid& LastLogId) {
   COLuint64 NextSequenceId = LastLogId.SequenceId;
   if (SDB_INVALID_SID == NextSequenceId) {
      NextSequenceId = SDB_DEFAULT_SID;
   }
   NextSequenceId++;
   COLuint64 NextQueueId = LastLogId.QueueId;
   if (SDB_INVALID_QID == NextQueueId) {
      NextQueueId = SDB_DEFAULT_QID;
   }
   // note that for non-queued messages, their QID is set to the previous queued message's QID.
   // e.g. QID for message M I D E M I W M are 3 3 3 3 4 4 4 5
   if (SDB_MESSAGE == pMessage->Type) { NextQueueId++; }
   pMessage->Id.SequenceId = NextSequenceId;
   pMessage->Id.QueueId = NextQueueId;
   LastLogId = pMessage->Id;
}

SDBid SDBlogMessage(SDBsystem* pSystem, std::shared_ptr<SDBmessage> pMessage){
   COL_FUNCTION(SDBlogMessage);
   const COLstring& ComponentId = pMessage->Id.MessageId.ComponentId;
   COL_VAR(ComponentId);
   COLASSERT(pSystem->ComponentMap.count(ComponentId) == 1);
   SDBcomponent& Component = pSystem->ComponentMap[ComponentId];
   if (pMessage->Id.MessageId.DateTime != Component.LastDateTime) {
      Component.NextMessageId = 1;
      Component.LastDateTime = pMessage->Id.MessageId.DateTime;
   } else {
      Component.NextMessageId++;
   }
   pMessage->Id.MessageId.Index = Component.NextMessageId;
   COLstring QueueId = pMessage->Id.MessageId.ComponentId;
   COLASSERT(pSystem->ComponentMap.count(QueueId));
   // increment SID and assign
   SDBsetMessageSequenceIdAndQueueId(pMessage.get(), pSystem->ComponentMap[QueueId].LastLogId);
   // pMessages is shared between pLogMessages and pJournalMessages/pQueueCache
   Component.pLogMessages->push_back(std::shared_ptr<SDBmessage>(pMessage));  // add message to component's buffer
   if (SDB_MESSAGE == pMessage->Type) {
      Component.pJournalMessages->push_back(std::shared_ptr<SDBmessage>(pMessage));
   }
   SDBserializeContent((SDBcontent*)pMessage.get(), &pSystem->JournalBuffer);
   if (pSystem->pCheckNotifications) {
      pSystem->pCheckNotifications->run(pMessage.get());
   }
   COL_VAR(*pMessage);
   return pMessage->Id;
}


void SDBcreateDequeue(SDBsystem* pSystem, const COLstring& QueueId, const COLstring& ConsumerId, const COLstring& EncryptionKey, COLauto<COLclosure2<int, SDBmessage*>> pCallback) {
   COL_FUNCTION(SDBcreateDequeue);
   COLASSERT(pSystem->ComponentMap.count(QueueId));
   SDBcomponent& SourceComponent = pSystem->ComponentMap[QueueId];
   auto Consumer = SourceComponent.Consumers.find(ConsumerId);
   if (Consumer != SourceComponent.Consumers.end() && Consumer.value().pDequeue.get()) {
      COL_WRN("A dequeue already exists for queue " << QueueId << " -> " << ConsumerId);
      return;
   }
   const SDBmessageId& Position = Consumer.value().Position.MessageId;
   COL_VAR(Position);
   SDBdequeue* pDequeue = SDBcreateDequeue(Position, SourceComponent.pQueueCache, EncryptionKey);
   SDBsetOnDequeued(pDequeue, pCallback);
   Consumer.value().pDequeue = pDequeue;
}

void SDBdeleteDequeue(SDBsystem* pSystem, const COLstring& QueueId, const COLstring& ConsumerId) {
   COL_FUNCTION(SDBdeleteDequeue);
   COLASSERT(pSystem->ComponentMap.count(QueueId));
   SDBcomponent& SourceComponent = pSystem->ComponentMap[QueueId];
   auto Consumer = SourceComponent.Consumers.find(ConsumerId);
   if (Consumer == SourceComponent.Consumers.end() || !Consumer.value().pDequeue.get()) {
      COL_WRN("Dequeue not found for queue " << QueueId << " -> " << ConsumerId);
      return;
   }
   // delete dequeue
   delete Consumer.value().pDequeue.release();
}

void SDBdequeueMessage(SDBsystem* pSystem, const COLstring& QueueId, const COLstring& ConsumerId) {
   COL_FUNCTION(SDBdequeueMessage);
   COLASSERT(pSystem->ComponentMap.count(QueueId));
   SDBcomponent& SourceComponent = pSystem->ComponentMap[QueueId];
   auto Consumer = SourceComponent.Consumers.find(ConsumerId);
   COLASSERT(Consumer != SourceComponent.Consumers.end());
   COLASSERT(Consumer.value().pDequeue.get());
   SDBdequeueMessage(Consumer.value().pDequeue, pSystem->pLoop);
}

void SDBsetPositionCommitted(SDBsystem* pSystem, const COLstring& QueueId, const COLstring& ConsumerId, COLclosure0* pCallback) {
   COL_FUNCTION(SDBsetPositionCommitted);
   COLASSERT(pSystem->ComponentMap.count(QueueId) == 1);
   SDBcomponent& SourceComponent = pSystem->ComponentMap[QueueId];
   auto Consumer = SourceComponent.Consumers.find(ConsumerId);
   COLASSERT(Consumer != SourceComponent.Consumers.end());
   Consumer.value().pPositionCommitted = pCallback;
}

void SDBsetQueueNotEmpty(SDBsystem* pSystem, const COLstring& QueueId, const COLstring& ConsumerId, COLclosure0* pCallback) {
   COL_FUNCTION(SDBsetQueueNotEmpty);
   COLASSERT(pSystem->ComponentMap.count(QueueId) == 1);
   SDBcomponent& SourceComponent = pSystem->ComponentMap[QueueId];
   auto Consumer = SourceComponent.Consumers.find(ConsumerId);
   COLASSERT(Consumer != SourceComponent.Consumers.end());
   Consumer.value().pQueueNotEmpty = pCallback;
}

static SDBid SDBgetSmallestConsumerPosition(const COLhashmap<COLstring, SDBconsumer>& Consumers) {
   COL_FUNCTION(SDBgetSmallestConsumerPosition);
   // If there is only one queue consumer, just go ahead and clear the cache
   if (Consumers.size() <= 1) { return SDBid(); }
   auto Consumer = Consumers.cbegin();
   SDBid SmallestPosition = Consumer.value().Position;
   do {
      if (Consumer.value().Position < SmallestPosition) {
         SmallestPosition = Consumer.value().Position;
      }
      Consumer++;
   } while (Consumer != Consumers.cend());
   return SmallestPosition;
}

void SDBcleanUpQueueCache(SDBsystem* pSystem, SDBid TargetId) {
   COL_FUNCTION(SDBcleanUpQueueCache);
   SDBcomponent& Component = pSystem->ComponentMap[TargetId.MessageId.ComponentId];
   SDBid SmallestPosition = SDBgetSmallestConsumerPosition(Component.Consumers);
   COL_VAR2(SmallestPosition, TargetId);
   if (!SmallestPosition.MessageId.isNull() && SmallestPosition < TargetId) {
      TargetId = SmallestPosition;
      COL_TRC("Messages after " << TargetId << " are needed by other queue consumers. They will be skipped");
   }
   SDBremoveMessagesFromList(Component.pQueueCache, TargetId);
}

void SDBcommitPosition(SDBsystem* pSystem, SDBid Position, const COLstring& ConsumerId) {
   COL_FUNCTION(SDBcommitPosition);
   const COLstring& QueueId = Position.MessageId.ComponentId;
   if (pSystem->ClearQueue.count(QueueId)) {
      COL_TRC("Clear queue requested. Commit End-Of-Queue position(s) instead.");
      pSystem->ClearQueue.removeKey(QueueId);
      SDBclearQueueNow(pSystem, QueueId, ConsumerId);
      return;
   }
   SDBcommitPositionNow(pSystem, Position, ConsumerId);
   if (pSystem->RepositionQueue.count(ConsumerId)) {
      COL_TRC("Queue reposition requested. Commit New position.");
      SDBid NewPosition = pSystem->RepositionQueue[ConsumerId];
      pSystem->RepositionQueue.removeKey(ConsumerId);
      SDBrepositionQueueNow(pSystem, NewPosition, ConsumerId);
   }
}

SDBmessageId SDBcurrentPosition(SDBsystem* pSystem, const COLstring&  QueueId, const COLstring& ConsumerId) {
   const COLhashmap<COLstring, SDBcomponent>& Components = pSystem->ComponentMap;
   if (Components.count(QueueId) == 0) { return SDBnilPosition(QueueId); }
   const COLhashmap<COLstring, SDBconsumer>& Consumers = Components[QueueId].Consumers;
   if (Consumers.count(ConsumerId) == 0) { return SDBnilPosition(QueueId); }
   return Consumers[ConsumerId].Position.MessageId;
}

COLarray<SDBmessageId> SDBqueuePosition(SDBsystem* pSystem, const COLstring& ComponentId) {
   // COL_FUNCTION(SDBqueuePosition);
   COLarray<SDBmessageId> Positions;
   if (pSystem->ComponentMap.count(ComponentId) == 0) { return Positions; }
   const COLarray<COLstring>& Sources = pSystem->ConfigMap.ConfigMap[ComponentId].CoreConfig.Sources;
   for (int i = 0; i < Sources.size(); ++i) {
      const COLstring& QueueId = Sources[i];
      const auto& Consumers = pSystem->ComponentMap[QueueId].Consumers;
      if (Consumers.count(ComponentId)) {
         Positions.push_back(Consumers[ComponentId].Position.MessageId);
      }
   }
   // COL_VAR(Positions.size());
   return Positions;
}
