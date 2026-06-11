// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBqueueCount
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday 14 September 2023 - 01:46PM
// ---------------------------------------------------------------------------

#include <SDB/SDBqueueCount.h>
#include <SDB/SDBsystem.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


int SDBqueueCount(SDBsystem* pSystem, const COLstring& ComponentId) {
   COL_FUNCTION(SDBqueueCount);
   COLmap<COLstring, int> QueueCountDetail = SDBqueueCountDetail(pSystem, ComponentId);
   int QueueCount = 0;
   for (auto Entry : QueueCountDetail) {
      QueueCount += Entry.second;
   }
   return QueueCount;
}

COLmap<COLstring, int> SDBqueueCountDetail(SDBsystem* pSystem, const COLstring& ComponentId) {
   COL_FUNCTION(SDBqueueCountDetail);
   COL_VAR(ComponentId);
   COLmap<COLstring, int> QueueCountDetail;
   if (pSystem->ComponentMap.count(ComponentId) == 0) { return QueueCountDetail; }
   const COLarray<COLstring>& Sources = pSystem->ConfigMap.ConfigMap[ComponentId].CoreConfig.Sources;
   for (const auto& QueueId : Sources) {
      COL_VAR(QueueId);
      const SDBcomponent& Queue = pSystem->ComponentMap[QueueId];
      // COLASSERT(Queue.LastJournaledId.QueueId != SDB_INVALID_QID);
      COL_VAR2(Queue.LastJournaledId, Queue.BOQqueueId);
      COLuint64 Count = 0;
      if (Queue.Consumers.count(ComponentId)) {
         COL_TRC("Consumer has specific queue position");
         const SDBconsumer& Consumer = Queue.Consumers[ComponentId];
         // COLASSERT(Consumer.Position.MessageId.DateTime != 0);    // would fail if consumer created with BOQ postion, try remove them
         // COLASSERT(Consumer.Position.QueueId != SDB_INVALID_QID);
         COL_VAR(Consumer.Position);
         if (Consumer.Position.isBOQ()) {
            Count = Queue.LastJournaledId.QueueId - Queue.BOQqueueId;
         } else {
            if (Queue.LastJournaledId.MessageId == Consumer.Position.MessageId) { // IX-4214 This is to handle lack of QID and SID prior to log format change. Prevents large queue count bug 
               Count = 0;
            } else {
               Count = Queue.LastJournaledId.QueueId - Consumer.Position.QueueId;
            }
         }
      } else {
         // Consumer is not in Queue's Consumers. likely because it's not in the queue's position map
         COL_ERR("Consumer " << ComponentId << " doesn't have a valid " << QueueId << " position");
         // COLASSERT(Queue.BOQqueueId != SDB_INVALID_QID);
         // COL_VAR(Queue.BOQqueueId);
         // Count = Queue.LastJournaledId.QueueId - Queue.BOQqueueId;
      }
      COL_VAR2(QueueId, Count);
      QueueCountDetail[QueueId] = Count;
   }
   return QueueCountDetail;
}
