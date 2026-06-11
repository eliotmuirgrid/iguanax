// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBqueuePosition
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Tuesday 04 April 2023 - 02:27PM
// ---------------------------------------------------------------------------

#include <SDB/SDBenvelope.h>
#include <SDB/SDBmessageId.h>
#include <SDB/SDBposition.h>
#include <SDB/SDBqueueCount.h>
#include <SDB/SDBqueuePosition.h>
#include <SDB/SDBsystem.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void SDBcommitPositionNow(SDBsystem* pSystem, const SDBid& Position, const COLstring& ConsumerId, bool DecrementQueueCount) {
   COL_FUNCTION(SDBcommitPositionNow);
   const COLstring& QueueId = Position.MessageId.ComponentId;
   COLASSERT(pSystem->ComponentMap.count(QueueId) == 1);
   SDBcomponent& Queue = pSystem->ComponentMap[QueueId];
   auto Consumer = Queue.Consumers.find(ConsumerId);
   COLASSERT(Consumer != Queue.Consumers.end());
   // record the position to be committed to the journal
   (*Queue.pJournalPositions)[Consumer.key()] = Position;
   // construct position and serialize to journal buffer
   SDBposition PositionData(ConsumerId, Position);
   SDBserializeContent((SDBcontent*)&PositionData, &pSystem->JournalBuffer);
   COL_TRC("New position: ");
   COL_VAR(Position);
   // queue count = queue's LastJournaledId.QueueId - queue consumer's Position.QueueId
   // the queue count is automatically updated when the consumer's position is updated after commit
}

void SDBrepositionQueueNow(SDBsystem* pSystem, const SDBid& Position, const COLstring& ConsumerId) {
   COL_FUNCTION(SDBrepositionQueueNow);
   COLASSERT(pSystem->ComponentMap.count(Position.MessageId.ComponentId) == 1);
   SDBcomponent& Queue = pSystem->ComponentMap[Position.MessageId.ComponentId];
   SDBconsumer& Consumer = Queue.Consumers[ConsumerId];
   SDBcommitPositionNow(pSystem, Position, ConsumerId, false);
   SDBcleanUpQueueCache(pSystem, Position);
   if (Consumer.pDequeue.get()) {
      // shouldn't be dequeueing at all since we dequeue, process message, commit position, dequeue next message
      SDBmovePosition(Consumer.pDequeue, Position);
   }
}

bool SDBrepositionQueue(SDBsystem* pSystem, const SDBid& Position, const COLstring& ConsumerId) {
   COL_FUNCTION(SDBrepositionQueue);
   COL_VAR2(ConsumerId, Position);
   if (Position.MessageId.isNull()) { COL_WRN("Invalid Position"); return false; }
   if (pSystem->ConfigMap.ConfigMap.count(ConsumerId) == 0) { COL_WRN("Invalid ConsumerId"); return false; }
   const COLarray<COLstring>& Sources = pSystem->ConfigMap.ConfigMap[ConsumerId].CoreConfig.Sources;
   if (Sources.indexOf(Position.MessageId.ComponentId) == -1) { COL_WRN("Invalid Queue"); return false; }
   if (pSystem->ComponentMap[Position.MessageId.ComponentId].Consumers[ConsumerId].Position == Position) {
      COL_TRC("Repositioning to the current position. NOP");
      return true;
   }
   // make sure we are not dequeueing from file. If we are dequeueing from memory
   // cache, we can't be here since both functions run on the main thread.
   for(const auto& Source : Sources) {
      auto& Queue = pSystem->ComponentMap[Source];
      if(Queue.Consumers.count(ConsumerId) == 0) { continue; }
      auto& Consumer = Queue.Consumers[ConsumerId];
      if(Consumer.Position == Queue.LastJournaledId) { continue; }
      if(Consumer.pDequeue != NULL && Consumer.pDequeue->busy()) {
         COL_TRC("Dequeue positions will be updated on next commit");
         pSystem->RepositionQueue[ConsumerId] = Position;
         return true;
      }
   }
   COL_TRC("Updating dequeue positions now");
   SDBrepositionQueueNow(pSystem, Position, ConsumerId);
   return true;
}


void SDBclearQueueNow(SDBsystem* pSystem, const COLstring& QueueId, const COLstring& ComponentId) {
   COL_FUNCTION(SDBclearQueueNow);
   COLASSERT(pSystem->ConfigMap.ConfigMap.count(ComponentId) == 1 && pSystem->ConfigMap.ConfigMap.count(QueueId) == 1);
   auto& Queue = pSystem->ComponentMap[QueueId];
   if (Queue.Consumers.count(ComponentId) == 0) { return; }
   auto& Consumer = Queue.Consumers[ComponentId];
   if (Consumer.Position == Queue.LastJournaledId) { return; }
   // move queue position to the last message
   COLASSERT(Consumer.Position < Queue.LastJournaledId);
   COL_TRC("Advancing dequeue position to EOQ for " << QueueId);
   SDBcommitPositionNow(pSystem, Queue.LastJournaledId, ComponentId, false);
   SDBcleanUpQueueCache(pSystem, Queue.LastJournaledId);
   if (Consumer.pDequeue.get()) {
      // shouldn't be dequeueing at all since we dequeue, process message, commit position, dequeue next message
      SDBmovePosition(Consumer.pDequeue, Queue.LastJournaledId);
   }
}

static bool SDBdelayClearQueue(const COLarray<COLstring>& Sources, SDBsystem* pSystem, const COLstring& ComponentId) {
   COL_FUNCTION(SDBdelayClearQueue);
   for(auto const& Source : Sources) {
      auto& Queue = pSystem->ComponentMap[Source];
      if(Queue.Consumers.count(ComponentId) == 0) { continue; }
      auto& Consumer = Queue.Consumers[ComponentId];
      if(Consumer.Position == Queue.LastJournaledId) { continue; }
      if(Consumer.pDequeue != NULL && Consumer.pDequeue->busy()) {
         COL_TRC("Dequeue positions will be updated on next commit");
         for(const auto& source : Sources) { pSystem->ClearQueue[source] = true; }
         return true;
      }
   }
   return false;
}

void SDBclearQueue(SDBsystem* pSystem, const COLstring& ComponentId) {
   COL_FUNCTION(SDBclearQueue);
   COLASSERT(pSystem->ComponentMap.count(ComponentId) == 1);
   SDBcomponent& Component = pSystem->ComponentMap[ComponentId];
   COL_TRC(Component.LastJournaledId);
   const COLarray<COLstring>& Sources = pSystem->ConfigMap.ConfigMap[ComponentId].CoreConfig.Sources;
   // make sure we are not dequeueing from file. If we are dequeueing from memory
   // cache, we can't be here since both functions run on the main thread.
   if(SDBdelayClearQueue(Sources, pSystem, ComponentId)) { return; }
   COL_TRC("Updating dequeue position");
   for(const auto& source : Sources) { SDBclearQueueNow(pSystem, source, ComponentId); }
}
