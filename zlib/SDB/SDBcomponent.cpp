// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBcomponent
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Fri 12 Sep 2025 13:37:47 EDT
// ---------------------------------------------------------------------------
#include <SDBU/SDBUmessageExtract.h>

#include <SDB/SDBcomponent.h>
#include <SDB/SDBcontentDelete.h>
#include <SDB/SDBcryptoFile.h>
#include <SDB/SDBfileRead.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBpositionMap.h>
#include <SDB/SDBpositionMap1.h>
#include <SDB/SDBreader.h>
#include <SDB/SDBserviceLog.h>

#include <SCK/SCKloop.h>

#include <FIL/FILutils.h>

#include <CFG/CFGmap.h>

#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;



SDBconsumer::SDBconsumer() {
   // COL_METHOD(SDBconsumer::SDBconsumer);
}

SDBconsumer::~SDBconsumer() {
   // COL_METHOD(SDBconsumer::~SDBconsumer);
}

SDBcomponent::SDBcomponent() {
   // COL_METHOD(SDBcomponent::SDBcomponent);
   pQueueCache       = new COLlist<std::shared_ptr<SDBmessage>>();
   pLogMessages      = new COLlist<std::shared_ptr<SDBmessage>>();
   pJournalMessages  = new COLlist<std::shared_ptr<SDBmessage>>();
   pJournalPositions = new COLhashmap<COLstring, SDBid>();
}

SDBcomponent::~SDBcomponent() {
   // COL_METHOD(SDBcomponent::~SDBcomponent);
   // free SDB_MESSAGE messages in pQueueCache
   while (pQueueCache->size()) {
      pQueueCache->pop_front();
   }
}

COLostream& operator<<(COLostream& Stream, const COLhashmap<COLstring, SDBid>& PositionMap) {
   if (PositionMap.size() == 0) {
      Stream << "No positions";
   } else {
      Stream << newline;
      for (auto Consumer : PositionMap) {
         const SDBid& Position = Consumer->second;
         Stream << "   * '" << Consumer->first << "' @ " << Position.MessageId << " SID=" << Position.SequenceId << " QID=" << Position.QueueId << newline;
      }
   }
   return Stream;
}

COLostream& operator<<(COLostream& Stream, const SDBcomponentStatus& Obj) {
   Stream << newline
          << "LastLogId  = " << Obj.LastLogId  << newline
          << "LastDataId = " << Obj.LastDataId << newline
          << "BOQqueueId = " << Obj.BOQqueueId << newline
          << "Positions  = " << Obj.PositionMap;
   return Stream;
}

static COLhashmap<COLstring, COLarray<COLstring>>* SDBgetQueueConsumerMap(const CFGmap& ConfigMap) {
   COLhashmap<COLstring, COLarray<COLstring>>* pMap = new COLhashmap<COLstring, COLarray<COLstring>>();
   for (const auto& Config : ConfigMap.ConfigMap) {
      for (const auto& Source : Config.second.CoreConfig.Sources) {
         (*pMap)[Source].push_back(Config.first);
      }
   }
   return pMap;
}

// MAIN
static void SDBcomponentsInitialized(COLclosure1<COLhashmap<COLstring, SDBcomponent>*>* pCallback, COLhashmap<COLstring, COLarray<COLstring>>* pQueueConsumerMap, COLmap<COLstring, SDBcomponentStatus>* pStatusMap) {
   COL_FUNCTION(SDBcomponentsInitialized);
   COLhashmap<COLstring, SDBcomponent>* pComponentMap = new COLhashmap<COLstring, SDBcomponent>();
   COLhashmap<COLstring, SDBcomponent>&  ComponentMap = *pComponentMap;
   // populate pComponentMap using pStatusMap
   for (const auto& Entry : *pStatusMap) {
      SDBcomponent& Component = ComponentMap[Entry.first];
      const SDBcomponentStatus& Status = Entry.second;
      Component.LastJournaledId = Status.LastDataId;
      Component.LastLogId = Status.LastLogId;
      Component.BOQqueueId = Status.BOQqueueId;
      for (const auto& Consumer : Status.PositionMap) {
         // if consumer is at the delete-consumer position, it had already been removed from queue
         // don't add it to queue's consumer map
         if (SDBdeleteConsumer(Consumer.second.MessageId)) {
            COL_TRC("Consumer " << Consumer.first << " had been removed from " << Entry.first);
         } else {
            Component.Consumers[Consumer.first].Position = Consumer.second;
         }
      }
   }
   // make sure every consumer in pQueueConsumerMap has a position
   // if we can't find one from the logs due to corruption/deletion, use queue's LastJournaledId.
   COLstring Warning;
   COLostream Stream(Warning);
   for (const auto& Component : (*pQueueConsumerMap)) {
      const COLstring& QueueId = Component.first;
      COL_VAR(QueueId);
      auto& Queue = ComponentMap[QueueId];
      auto& Consumers = Queue.Consumers;
      for (const auto& ConsumerId : Component.second) {
         if (Consumers.count(ConsumerId)) { continue; }
         COL_TRC("Queue position absent for consumer " << ConsumerId << ". Set to queue's LastJournaledId");
         Stream << " * Default consumer '" << ConsumerId << "' to " << Queue.LastJournaledId.MessageId << " SID=" << Queue.LastJournaledId.SequenceId << " QID=" << Queue.LastJournaledId.QueueId << newline;
         Consumers[ConsumerId].Position = Queue.LastJournaledId;
      }
   }
   if (!Warning.is_null()) {
      SDBlogService("Detected consumer(s) without a valid queue position:\n" + Warning, "#warning");
   }
   pCallback->runAndDelete(pComponentMap);
}

// MAIN
static void SDBcomponentInitialized(COLstring ComponentId, SDBcomponentStatus Status, COLmap<COLstring, SDBcomponentStatus>* pStatusMap, int Count, COLhashmap<COLstring, COLarray<COLstring>>* pQueueConsumerMap, COLclosure1<COLhashmap<COLstring, SDBcomponent>*>* pCallback) {
   COL_FUNCTION(SDBcomponentInitialized);
   COL_TRC(ComponentId << " => " << Status);
   (*pStatusMap)[ComponentId] = Status;
   if (pStatusMap->size() == Count) {
      SDBcomponentsInitialized(pCallback, pQueueConsumerMap, pStatusMap);
      delete pQueueConsumerMap;
      delete pStatusMap;
   }
}

static void SDBdecryptBuffer(COLstring& Data, const COLstring& EncKey, COLstring* pPlainData) {
   COL_FUNCTION(SDBdecryptBuffer);
   // TODO: record and report corrupted regions?
   do {
      int Amount = SDBparseAndDecrypt(Data.c_str(), Data.size(), EncKey, pPlainData);
      if (Amount > 0) {
         COL_TRC("extracted and decrypted an encryption packet.");
         Data.shift(Amount);
         continue;
      }
      if (-1 == Amount) { COL_TRC("buffer contains a partial message.");  return; }
      COL_TRC("detected data corruption");
      int Delta = SDBfindNextEncMarker(Data.data(), Data.size());
      if (-1 == Delta)  { COL_TRC("buffer contains corrupted data only"); return; }
      COL_TRC("skip corrupted region");
      Data.shift(Delta);
   } while (true);
}

static bool SDBmessageExtract(const COLstring& LogFile, const COLstring& EncryptionKey, COLlist<SDBcontent*>* pList){
   COL_FUNCTION(SDBmessageExtract);
   if (!FILfileExists(LogFile)){
      COL_TRC("No logs exist for this component.");
      return false;
   }
   COLstring Data;
   if (!SDBfileRead(LogFile, &Data)) { COL_TRC("couldn't access for some reason"); return false; }
   if (!EncryptionKey.is_null()) {
      COL_TRC("Encryption enabled, will need to decrypt Data first");
      COLstring Plaintext;
      SDBdecryptBuffer(Data, EncryptionKey, &Plaintext);
      if (Plaintext.is_null())       { COL_TRC("fail to decrypt any data"); return false; }
      Data.swap(&Plaintext);
   }
   SDBextractMessages(Data.c_str(), Data.size(), nullptr, pList);
   return true;
}

static bool SDBgetPositionMapLastDataLogId(const COLstring& LogFile, const COLstring& EncryptionKey, COLhashmap<COLstring, SDBid>* pPositionMap, SDBid* pLastLogId, SDBid* pLastDataId) {
   COL_FUNCTION(SDBgetPositionMapLastDataLogId);
   COLlist<SDBcontent*> List;
   if (!SDBmessageExtract(LogFile, EncryptionKey, &List)) {
      COL_TRC("Failed to read " << LogFile);
      return false;
   }
   for (auto pContent : List) {
      switch (pContent->Type) {
         case SDB_POSMAP:  if (pPositionMap) { *pPositionMap = ((SDBpositionMap *) pContent)->PositionMap.begin().value(); } break;
         case SDB_POSMAP1: if (pPositionMap) { *pPositionMap = ((SDBpositionMap1*) pContent)->PositionMap.begin().value(); } break;
         default:
            if (!SDBisMessage(pContent->Type)) { break; }
            SDBmessage* pMessage = (SDBmessage*) pContent;
            // COL_VAR(*pMessage);
            if (pLastLogId)                                   { *pLastLogId  = pMessage->Id; }
            if (pLastDataId && SDB_MESSAGE == pContent->Type) { *pLastDataId = pMessage->Id; }
            break;
      }
   }
   SDBcontentDelete(&List);
   return true;
}

// used to scan log files from newest to oldest until position map, SID, and QID are set.
// if a status field is set in a newer log file, don't overwrite it when scanning an older log file.
bool SDBfindPositionMapLastDataAndLogId(const COLstring& LogFile, const COLstring& EncryptionKey, bool HasConsumers, SDBcomponentStatus& Status) {
   COL_FUNCTION(SDBfindPositionMapLastDataAndLogId);
   COL_VAR(LogFile);
   COLhashmap<COLstring, SDBid>* pPositionMap = &Status.PositionMap;
   SDBid*                        pLastDataId  = &Status.LastDataId;
   SDBid*                        pLastLogId   = &Status.LastLogId;
   // if already set, don't update them
   if ( pPositionMap->size())             { pPositionMap = nullptr; }
   if (!pLastLogId->MessageId.isNull())   { pLastLogId   = nullptr; }
   if (!pLastDataId->MessageId.isNull())  { pLastDataId  = nullptr; }
   if (!SDBgetPositionMapLastDataLogId(LogFile, EncryptionKey, pPositionMap, pLastLogId, pLastDataId)) { return false; }
   // pLastDataId : we need to find LastDataId
   // pLastDataId->MessageId.isNull() : but it's not set by SDBgetPositionMapLastDataLogId() i.e. no enqueue messages in LogFile
   // pLastLogId : we need to find LastLogId (this is only true for the last log file)
   // 0 == pLastLogId->QueueId : and not a single enqueued message yet because queue ID is still 0
   // However, if the last data message was enqueued before the introduction of queue ID in v113, we have to keep scanning
   // IX-4300 : Disabled by default for now unless the env var is set
   // we can reenable once all users have fully migrated to new format log
   if(getenv("IFW_FAST_COMPONENT_INIT") != NULL) {
      if ( pLastDataId && pLastDataId->MessageId.isNull() && pLastLogId && 0 == pLastLogId->QueueId) {
         COL_TRC("didn't find an enqueued message, and component log doesn't have any enqueued messages");
         *pLastDataId = SDBid(SDBmessageId(pLastLogId->MessageId.ComponentId, 0, 0), 0, 0);
      }
   }
   COL_VAR(Status);
   if (!Status.LastLogId.MessageId.isNull() && !Status.LastDataId.MessageId.isNull()) {
      if (!HasConsumers || Status.PositionMap.size()) { return true; }
   }
   return false;
}

void SDBgetPositionMapLastDataAndLogId(const COLstring& ComponentId, const COLstring& LogRootDir, const COLstring& EncryptionKey, bool HasConsumers, SDBcomponentStatus& Status) {
   COL_FUNCTION(SDBgetPositionMapLastDataAndLogId);
   COLstring LogFile = SDBfindNewest(SDBdir(ComponentId, LogRootDir));
   while (!LogFile.is_null() && !SDBfindPositionMapLastDataAndLogId(LogFile, EncryptionKey, HasConsumers, Status)) {
      LogFile = SDBfindPreviousLogFile(LogFile, ComponentId, LogRootDir);
   }
   if (LogFile.is_null()) { COL_TRC("No more logs for " << ComponentId); }
   if (Status.LastDataId.isNull()) {
      COL_TRC(ComponentId << " has no readable logs");
      Status.LastDataId.setBOQ(ComponentId);
   }
}

bool SDBgetBOQqueueId(const COLstring& LogFile, const COLstring& EncryptionKey, COLuint64* pQueueId) {
   COL_FUNCTION(SDBgetBOQqueueId);
   // Optimization: don't load and parse the entire file when we just need the first message
   COLlist<SDBcontent*> List;
   if (!SDBmessageExtract(LogFile, EncryptionKey, &List)) {
      COL_TRC("Failed to read " << LogFile);
      return false;
   }
   bool Found = false;
   for (auto pContent : List) {
      if (!SDBisMessage(pContent->Type)) { continue; }
      // found first message
      SDBmessage* pMessage = (SDBmessage*) pContent;
      // COL_VAR(*pMessage);
      COLuint64 QueueId = pMessage->Id.QueueId;
      if (SDB_MESSAGE == pMessage->Type) { QueueId--; }
      // v1 messages don't have sequence queue id, use default
      if (SDB_INVALID_QID == pMessage->Id.QueueId) { QueueId = SDB_DEFAULT_QID; }
      *pQueueId = QueueId;
      Found = true;
      break;
   }
   SDBcontentDelete(&List);
   return Found;
}

static COLuint64 SDBfindBOQqueueId(const COLstring& ComponentId, const COLstring& LogRootDir, const COLstring& EncryptionKey) {
   COL_FUNCTION(SDBfindBOQqueueId);
   COL_VAR2(ComponentId, LogRootDir);
   COLstring LogFile = SDBfindOldest(SDBdir(ComponentId, LogRootDir));
   COL_VAR(LogFile);
   COLuint64 QueueId = SDB_INVALID_QID;
   while (!LogFile.is_null() && !SDBgetBOQqueueId(LogFile, EncryptionKey, &QueueId)) {
      LogFile = SDBfindNextLogFile(LogFile, ComponentId);
   }
   if (LogFile.is_null()) { COL_TRC("No more logs for " << ComponentId); }
   if (SDB_INVALID_QID == QueueId) {
      COL_TRC(ComponentId << " has no readable logs");
      QueueId = SDB_DEFAULT_QID;
   }
   COL_VAR(QueueId);
   return QueueId;
}

// WORKER
static void SDBinitializeComponent(COLstring ComponentId, COLstring LogRootDir, COLstring EncryptionKey, bool HasConsumers, SCKloop* pLoop, COLclosure2<COLstring, SDBcomponentStatus>* pDone) {
   COL_FUNCTION(SDBinitializeComponent);
   COL_VAR(ComponentId);
   SDBcomponentStatus Status;
   SDBgetPositionMapLastDataAndLogId(ComponentId, LogRootDir, EncryptionKey, HasConsumers, Status);
   // get the Beginning of Queue position.
   Status.BOQqueueId = SDBfindBOQqueueId(ComponentId, LogRootDir, EncryptionKey);
   pLoop->post(COLnewClosure0(pDone, &COLclosure2<COLstring, SDBcomponentStatus>::runAndDelete, ComponentId, Status));
}

void SDBinitializeComponents(const CFGmap& ConfigMap, const COLstring& EncryptionKey, SCKloop* pLoop, COLclosure1<COLhashmap<COLstring, SDBcomponent>*>* pCallback, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBinitializeComponents);
   // TODO: may need to limit the number of worker threads
   int Count = ConfigMap.ConfigMap.size();
   if (0 == Count) {
      pCallback->runAndDelete(new COLhashmap<COLstring, SDBcomponent>());
      return;
   }
   COLhashmap<COLstring, COLarray<COLstring>>* pQueueConsumerMap = SDBgetQueueConsumerMap(ConfigMap);
   COLmap<COLstring, SDBcomponentStatus>* pStatusMap = new COLmap<COLstring, SDBcomponentStatus>();
   for (const auto& Config : ConfigMap.ConfigMap) {
      const COLstring& ComponentId = Config.first;
      bool HasConsumers = pQueueConsumerMap->count(ComponentId);
      COLclosure2<COLstring, SDBcomponentStatus>* pDone = COLnewClosure2(&SDBcomponentInitialized, pStatusMap, Count, pQueueConsumerMap, pCallback);
      pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBinitializeComponent, ComponentId, LogRootDir, EncryptionKey, HasConsumers, pLoop, pDone));
   }
}