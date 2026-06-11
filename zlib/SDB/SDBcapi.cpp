// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBcapi
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Monday 24 April 2023 - 01:17PM
// ---------------------------------------------------------------------------

#include <memory>

#include <SDB/SDBcapi.h>
#include <SDB/SDBdequeue.h>
#include <SDB/SDBepochTime.h>
#include <SDB/SDBenvVar.h>
#include <SDB/SDBflush.h>
#include <SDB/SDBinit.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBpurge.h>
#include <SDB/SDBqueueCount.h>
#include <SDB/SDBqueuePosition.h>
#include <SDB/SDBreader.h>
#include <SDB/SDBstats.h>
#include <SDB/SDBsystem.h>

#include <COL/COLmap.h>
#include <COL/COLioCheck.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool Initialized = false;    // whether SDBcontentMapInit() had been called
static SDBsystem* pLogQueue  = NULL;
static COLmap<COLstring, int> SDBITtestQueueCount;
static bool SDBITisTest = false;

static void SDBIlogQueueStarted(SDBsystem* pSystem, COLclosure0* pDone) {
   COL_FUNCTION(SDBIlogQueueStarted);
   pLogQueue = pSystem;
   pDone->runAndDelete();
}

void SDBIstartLogQueue(COLclosure0* pDone, SDBsystem* pSystem) {
   COL_FUNCTION(SDBIstartLogQueue);
   SDBstartSystem(pSystem, COLnewClosure0(&SDBIlogQueueStarted, pSystem, pDone));
}

bool SDBIlogQueueIsRunning() {
   return pLogQueue != NULL;
}

static void SDBIlogQueueStopped(SDBsystem* pSystem, COLclosure0* pDone) {
   COL_FUNCTION(SDBIlogQueueStopped);
   pLogQueue = NULL;
   pDone->runAndDelete();
}

void SDBIstopLogQueue(COLclosure0* pDone, SDBsystem* pSystem) {
   COL_FUNCTION(SDBIstopLogQueue);
   SDBstopSystem(pSystem, COLnewClosure0(&SDBIlogQueueStopped, pSystem, pDone));
}

static SDBid SDBIlog(SDBtype LogType, COLstring* pData, const COLstring& ComponentId, const COLstring& HashTags, const SDBmessageId& RefId, COLvar* pMeta, COLstring* pError) {
   COL_FUNCTION(SDBIlog);
   if (SDBITisTest) {
      COL_TRC("In context of unit test - do nothing.");
      return SDBid();
   }
#ifndef IFW_RELEASE
   COLASSERT(COLisMainThread());
#endif
   if (!pLogQueue) {
      COL_ERR("The Logging system hasn't been initialized yet. Message not logged.");
      return SDBid();
   }
   if (!Initialized) {
      SDBcontentMapInit();
      Initialized = true;
   }
   if (SDBmaxMessageSizeMB() * SDB_SIZE_MB < pData->size()) {
      COLstring Error = "The Log System limits the maximum message size to " + COLintToString(SDBmaxMessageSizeMB())
       + " MB.\nYou can increase the limit with the IFW_MAX_MESSAGE_SIZE_MB environment variable.\nMessage Preview:\n"
       + pData->substr(0, 888888);    // make sure error message < 1 MB
      COL_VAR(Error);
      if (pError) { Error.swap(pError); }
      return SDBid();
   }
   SDBmessage* pMessage = new SDBmessage();
   pMessage->Data.swap(pData);
   if (pMeta) { pMessage->Meta.swap(pMeta); }
   pMessage->Type      = LogType;
   pMessage->Id.MessageId = SDBmessageId(ComponentId);
   pMessage->RelatedId = RefId;
   pMessage->Tags      = HashTags;
   COL_VAR(pMessage->Tags);
   return SDBlogMessage(pLogQueue, std::shared_ptr<SDBmessage>(pMessage));
}

SDBid SDBIlogComponent(COLstring* pData, const COLstring& ComponentId, const COLstring& Tags, const SDBmessageId& RefId, COLvar* pMeta, COLstring* pError) {
   return SDBIlog(SDB_TRACE, pData, ComponentId, Tags, RefId, pMeta, pError);
}

SDBid SDBIlogComponent(COLstring Data,  const COLstring& ComponentId, const COLstring& Tags, const SDBmessageId& RefId, COLvar Meta, COLstring* pError) {
   return SDBIlog(SDB_TRACE, &Data, ComponentId, Tags, RefId, &Meta, pError);
}

SDBid SDBIlogService(const COLstring& Data, const COLstring& Tags) {
   COL_FUNCTION(SDBIlogService);
   if (!Initialized) {
      SDBcontentMapInit();
      Initialized = true;
   }
   return SDBlogService(Data, Tags);
}

void SDBIcreateQueue(const COLstring& QueueId) {
   COL_FUNCTION(SDBIcreateQueue);
#ifndef IFW_RELEASE
   COLASSERT(COLisMainThread());
#endif
   pLogQueue->ComponentMap[QueueId].LastJournaledId.setBOQ(QueueId);
}

void SDBIcreateQueueConsumer(const COLstring& QueueId, const COLstring& ConsumerId) {
   COL_FUNCTION(SDBIcreateQueueConsumer);
   COL_VAR2(QueueId, ConsumerId);
   if (pLogQueue == NULL) { return; }
#ifndef IFW_RELEASE
   COLASSERT(COLisMainThread());
#endif
   // create new entry in ComponentMap and init its PositionId
   SDBcomponent& Queue = pLogQueue->ComponentMap[QueueId];
   if (Queue.Consumers.count(ConsumerId) == 0) {
      COL_VAR(Queue.LastJournaledId);
      Queue.Consumers[ConsumerId].Position = Queue.LastJournaledId;
      // default start position to current End-Of-Queue
      SDBcommitPositionNow(pLogQueue, Queue.LastJournaledId, ConsumerId, false);
   } else {
      COL_WRN("Consumer " << ConsumerId << " already exists for queue " << QueueId);
   }
}

void SDBIdeleteQueueConsumer(const COLstring& QueueId, const COLstring& ConsumerId) {
   COL_FUNCTION(SDBIdeleteQueueConsumer);
   COL_VAR2(QueueId, ConsumerId);
#ifndef IFW_RELEASE
   COLASSERT(COLisMainThread());
#endif
   auto& Consumers = pLogQueue->ComponentMap[QueueId].Consumers;
   if (Consumers.count(ConsumerId) == 0) {
      COL_WRN("Consumer " << ConsumerId << " doesn't exist for queue " << QueueId);
      return;
   }
   // bad things could happen if dequeue is still happening
   // commit special dequeue postion to mark this consumer for deletion
   SDBcommitPositionNow(pLogQueue, SDBid(SDBdeleteConsumer(QueueId), 0, 0), ConsumerId, false);
   // we need to keep to consumer until the special SDBdeleteConsumer position is flushed
   // to the log file. Remove it in SDBwriteLog() later
}

void SDBIsetMessageEnqueued(const COLstring& QueueId, COLclosure1<SDBid>* pCallback) {
#ifndef IFW_RELEASE
   COLASSERT(COLisMainThread());
#endif
   SDBsetMessageQueued(pLogQueue, QueueId, pCallback);
}

SDBid SDBIenqueueMessage(COLstring* pData, const COLstring& QueueId, const COLstring& Tags, const SDBmessageId& RelatedId, COLvar* pMeta, COLstring* pError) {
#ifndef IFW_RELEASE
   COLASSERT(COLisMainThread());
#endif
   SDBid Id = SDBIlog(SDB_MESSAGE, pData, QueueId, Tags, RelatedId, pMeta, pError);
   if (!Id.MessageId.isNull()) { SDBflushSystemNow(pLogQueue); }
   return Id;
}

void SDBIsetPositionCommitted(const COLstring& QueueId, const COLstring& ConsumerId, COLclosure0* pCallback) {
   if (pLogQueue == NULL) {
      delete pCallback;
      return;
   }
#ifndef IFW_RELEASE
   COLASSERT(COLisMainThread());
#endif
   SDBsetPositionCommitted(pLogQueue, QueueId, ConsumerId, pCallback);
}

void SDBIcommitPosition(const SDBid& Position, const COLstring& ConsumerId) {
#ifndef IFW_RELEASE
   COLASSERT(COLisMainThread());
#endif
   SDBcommitPosition(pLogQueue, Position, ConsumerId);
   SDBflushSystemNow(pLogQueue);
}

SDBmessageId SDBIcurrentPosition(const COLstring& QueueId, const COLstring& ConsumerId) {
   return SDBcurrentPosition(pLogQueue, QueueId, ConsumerId);
}


void SDBIsetQueueNotEmpty(const COLstring& QueueId, const COLstring& ConsumerId, COLclosure0* pCallback) {
   if (pLogQueue == NULL) { return; }
#ifndef IFW_RELEASE
   COLASSERT(COLisMainThread());
#endif
   SDBsetQueueNotEmpty(pLogQueue, QueueId, ConsumerId, pCallback);
}

void SDBIcreateDequeue(const COLstring& QueueId, const COLstring& ConsumerId, COLauto<COLclosure2<int, SDBmessage*>> pCallback) {
   COL_FUNCTION(SDBIcreateDequeue);
   if (pLogQueue == NULL) { return; }
#ifndef IFW_RELEASE
   COLASSERT(COLisMainThread());
#endif
   SDBcreateDequeue(pLogQueue, QueueId, ConsumerId, SDBIencryptionKey(), pCallback);
}

void SDBIdeleteDequeue(const COLstring& QueueId, const COLstring& ConsumerId) {
   if (pLogQueue == NULL) { return; }
   COL_FUNCTION(SDBIdeleteDequeue);
#ifndef IFW_RELEASE
   COLASSERT(COLisMainThread());
#endif
   SDBdeleteDequeue(pLogQueue, QueueId, ConsumerId);
}

void SDBIdequeueMessage(const COLstring& QueueId, const COLstring& ConsumerId) {
#ifndef IFW_RELEASE
   COLASSERT(COLisMainThread());
#endif
   SDBdequeueMessage(pLogQueue, QueueId, ConsumerId);
}

int SDBIgetQueueCount(const COLstring& ConsumerId) {
   if (pLogQueue == NULL) {
      // Unit test logic
      if (SDBITtestQueueCount.count(ConsumerId)) {
         return SDBITtestQueueCount[ConsumerId];
      }
      return 0;
   }
   return SDBqueueCount(pLogQueue, ConsumerId);
}

COLarray<SDBmessageId> SDBIgetQueuePosition(const COLstring& ConsumerId) {
   if (pLogQueue == NULL) {
      return COLarray<SDBmessageId>();
   }
   return SDBqueuePosition(pLogQueue, ConsumerId);
}

COLarray<COLstring> SDBIgetQueueConsumers(const COLstring& QueueId) {
   COLarray<COLstring> ConsumerList;
   if (pLogQueue != NULL) {
      if (pLogQueue->ComponentMap.count(QueueId)) {
         const auto& Consumers = pLogQueue->ComponentMap[QueueId].Consumers;
         for (auto Consumer = Consumers.cbegin(); Consumer != Consumers.cend(); Consumer++) {
            ConsumerList.push_back(Consumer.key());
         }
      }
   }
   return ConsumerList;
}

COLarray<COLstring> SDBIgetQueueSources(const COLstring& Component) {
   COLarray<COLstring> SourceList;
   if (pLogQueue != NULL) {
      if (pLogQueue->ConfigMap.ConfigMap.count(Component)) {
         SourceList = pLogQueue->ConfigMap.ConfigMap[Component].CoreConfig.Sources;
      }
   }
   return SourceList;
}

COLstring SDBIgetComponentName(const COLstring& ComponentGuid) {
   if (pLogQueue != NULL) {
      if (pLogQueue->ConfigMap.ConfigMap.count(ComponentGuid)) {
         return pLogQueue->ConfigMap.ConfigMap[ComponentGuid].CoreConfig.Name;
      }
   }
   return "";
}

void SDBIclearQueueCount(const COLstring& ConsumerId) {
#ifndef IFW_RELEASE
   COLASSERT(COLisMainThread());
#endif
   SDBclearQueue(pLogQueue, ConsumerId);
}

bool SDBIrepositionQueue(const SDBid& Position, const COLstring& ConsumerId) {
#ifndef IFW_RELEASE
   COLASSERT(COLisMainThread());
#endif
   return SDBrepositionQueue(pLogQueue, Position, ConsumerId);
}

void SDBIincrementStatsCount(const COLstring& ComponentId, bool Inbound) {
   if (pLogQueue == NULL) { return; }
#ifndef IFW_RELEASE
   COLASSERT(COLisMainThread());
#endif
   SDBstatsIncrementCount(&pLogQueue->StatsMap, ComponentId, Inbound);
}

float SDBIgetComponentStats(const COLstring& ComponentId, bool Inbound) {
   if (pLogQueue == NULL) { return 0; }
   return SDBstatsMessagesPerSecond(&pLogQueue->StatsMap, ComponentId, Inbound);
}

void SDBIgetLogStats(COLclosure2<SDBlogStats, SDBlogStats>* pCallback) {
   if (pLogQueue == NULL) { return; }
   SDBstatsLogSpace(pLogQueue, pCallback);
}

COLstring SDBInameFromGuid(const COLstring& Guid){
   // COL_FUNCTION(SDBInameFromGuid);
   // COL_VAR(Guid);
   if (pLogQueue == NULL) { return ""; }
   if (Guid == SDB_SERVICE_COMPONENT_ID) { return SDB_SERVICE_COMPONENT_ID; }
   if (pLogQueue->ConfigMap.ConfigMap.count(Guid)) {
      return pLogQueue->ConfigMap.ConfigMap[Guid].CoreConfig.Name;
   }
   return "";
}

void SDBregisterNotificationCallback(SDBsystem* pSystem, COLclosure1<SDBmessage*>* pCallback){
   COL_FUNCTION(SDBregisterNotificationCallback);
   if (pCallback == NULL) { return; }
   pSystem->pCheckNotifications = pCallback;
}

bool SDBIlogEncryptionEnabled() {
   COL_FUNCTION(SDBIlogEncryptionEnabled);
   if (SDBITisTest) {
      COL_TRC("In context of unit test - encryption off.");
      return false;
   }
   COLASSERT(pLogQueue);
   return !pLogQueue->LogEncryptionKey.is_null();
}

COLstring SDBIencryptionKey() {
   COL_FUNCTION(SDBIencryptionKey);
   if (SDBITisTest) {
      COL_TRC("In context of unit test - no encryption key.");
      return "";
   }
   COLASSERT(pLogQueue);
   return pLogQueue->LogEncryptionKey;
}

// Removes cached messages up to and including TargetId from the target queue.
void SDBIcleanUpQueueCache(const SDBid& TargetId) {
   if (pLogQueue == NULL) { return; }
#ifndef IFW_RELEASE
   COLASSERT(COLisMainThread());
#endif
   SDBcleanUpQueueCache(pLogQueue, TargetId);
}


void SDBITsetTestQueueCount(const COLstring& QueueId, int Count) {
   SDBITtestQueueCount[QueueId] = Count;
}

void SDBITsetIsUnitTest(){
   COL_FUNCTION(SDBITsetIsUnitTest);
   SDBITisTest = true;
}
