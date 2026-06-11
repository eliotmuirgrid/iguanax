// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBlogWrite
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Tuesday 25 April 2023 - 05:09PM
// ---------------------------------------------------------------------------

#include <SDB/SDBcache.h>
#include <SDB/SDBenvelope.h>
#include <SDB/SDBlogIdle.h>
#include <SDB/SDBlogThread.h>
#include <SDB/SDBlogWrite.h>
#include <SDB/SDBsystem.h>
#include <SDB/SDBcryptoFile.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


static void SDBlogFlushDone(COLauto<COLmap<SDBcomponent*, SDBid>> pCommittedIdMap, COLclosure0* pCallback){
   COL_FUNCTION(SDBlogFlushDone);
   for (auto i = pCommittedIdMap->begin(); i != pCommittedIdMap->end(); i++) {
      SDBcomponent* pComponent = i.key();
      const SDBid& CommittedId = i.value();
      if (!CommittedId.MessageId.isNull()) {
         SDBevictLoggedMessages(*pComponent, CommittedId);
      }
      // else committed positions only
   }
   pCallback->runAndDelete();
}

static void SDBlogWritten(COLmap<SDBcomponent*, SDBid>* pCommittedIdMap, int* pCount, int Total, COLclosure0* pCallback) {
   COL_FUNCTION(SDBlogWritten);
   COL_VAR2(Total, *pCount);
   if (*pCount == Total) {
      // TODO: probably need to write to at least to 2 log dirs to tolerate network partitions
      COL_TRC("A log dir finished the log write. moving on");
      SDBlogFlushDone(pCommittedIdMap, pCallback);
   }
   (*pCount)--;
   if (*pCount == 0) {
      COL_TRC("all log dirs finished the log write");
      delete pCount;
   }
}

static int SDBgetLogHandle(SDBlogHandle& LogHandle, const COLstring& LogFile) {
   if (LogFile != LogHandle.FilePath) {
      if (-1 != LogHandle.FD) { SDBcloseLogFile(LogHandle.FD); }
      SDBcheckLogDir(LogFile);
      LogHandle.FD = SDBopenLogFile(LogFile);
      LogHandle.FilePath = LogFile;
   }
   time(&LogHandle.LastAccess);
   return LogHandle.FD;
}

static int SDBserializeLogPositions(const COLstring& ComponentId, const COLhashmap<COLstring, SDBid>* pLogPositions, COLstring* pBuffer) {
   COL_FUNCTION(SDBserializeLogPositions);
   int Size = pBuffer->size();
   SDBpositionMap PosMap;
   PosMap.PositionMap[ComponentId] = *pLogPositions;
   COL_VAR(PosMap);
   SDBserializeContent((SDBcontent*)&PosMap, pBuffer);
   return pBuffer->size() - Size;
}

static void SDBflushLogPositionsOnly(const COLhashmap<COLstring, SDBid>* pLogPositions, const COLstring& EncryptionKey, SDBlogHandle& Handle, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBflushLogPositionsOnly);
   COLstring ComponentId = pLogPositions->begin().value().MessageId.ComponentId;
   COLstring Buffer;
   SDBserializeLogPositions(ComponentId, pLogPositions, &Buffer);
   // Write positions to the latest log file
   COLstring LogFile = SDBfindLogFile(ComponentId, time(NULL) * 1000, LogRootDir);
   if (LogFile.is_null()) {
      COL_TRC("We don't have any log files. Create the first");
      LogFile = SDBlogFile(ComponentId, SDBtime(), LogRootDir);
   }
   int FD = SDBgetLogHandle(Handle, LogFile);

   const char* pBuffer = Buffer.get_buffer();
   int Size = Buffer.size();
   COLstring EncryptedData;
   if (!EncryptionKey.is_null()) {
      COL_TRC("Encrypting data");
      SDBencryptAndSerialize(Buffer, EncryptionKey, &EncryptedData);
      pBuffer = EncryptedData.get_buffer();
      Size = EncryptedData.size();
   }
   COL_TRC("Writing data to file");
   int Amount = SDBwriteLogFile(FD, pBuffer, Size);
   if (getenv(SDB_SYNC_WRITE_ENV_NAME)) {
      COL_TRC("Flush position to log file " << LogFile);
      SDBcommitLogFile(FD);
   }
}

// serializes messages at the beginning of pMessageList that have the same date time
static int SDBserializeLogMessagesWithDateTime(COLlist<std::shared_ptr<SDBmessage>>* pMessageList, COLstring* pBuffer) {
   COL_FUNCTION(SDBserializeLogMessagesWithDateTime);
   int Size = pBuffer->size();
   time_t DateTime = pMessageList->front()->Id.MessageId.DateTime / 1000;
   while (pMessageList->size()) {  // serialize all messages with the same timestamp
      SDBmessage* pMessage = pMessageList->front().get();
      COL_VAR(*pMessage);
      if (DateTime != pMessage->Id.MessageId.DateTime / 1000) { break; }
      SDBserializeContent((SDBcontent*)pMessage, pBuffer);
      // SDB_MESSAGE messages are shared with the component queue cache, popping here won't necessarily destroy them
      pMessageList->pop_front();
   }
   return pBuffer->size() - Size;
}

static bool SDBserializeLogMessagesForFile(const COLstring& FilePath, COLlist<std::shared_ptr<SDBmessage>>* pMessageList, const COLhashmap<COLstring, SDBid>* pLogPositions, bool FlushPositions, COLstring* pBuffer, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBserializeLogMessagesForFile);
   COL_VAR2(FilePath, FlushPositions);
   SDBmessage* pMessage = pMessageList->front().get();
   COLstring ComponentId = pMessage->Id.MessageId.ComponentId;
   time_t   FileTime = SDBlogFileTime(FilePath, ComponentId, LogRootDir);
   COLint64 FileSize = FILfileExists(FilePath) ? FILfileSize(FilePath) : 0;
   int BufferSize = pBuffer->size();
   // the first group of messages with the same epoch time must go into the file
   if (FileTime == pMessage->Id.MessageId.DateTime/1000) {
      // messages with the same epoch time must be in the same file
      FileSize += SDBserializeLogMessagesWithDateTime(pMessageList, pBuffer);
   }
   while (pMessageList->size()) {
      // make sure the date time of the next group of messages doesn't require epoch rollover
      if (SDBepochRollover(FileTime, pMessageList->front()->Id.MessageId.DateTime/1000)) {
         COL_TRC("Log file rollover required due to epoch time: " << pMessageList->front()->Id.MessageId.DateTime/1000);
         break;
      }
      // make sure the the next group of messages with the same epoch time doesn't push file size over the limit
      if (FileSize + SDBcalculateLogSize(pMessageList) > SDB_LOG_ROLLOVER_SIZE) {
         COL_TRC("Log file rollover required due to file size: " << FileSize);
         break;
      }
      // serialize the next group of messages with the same epoch time
      FileSize += SDBserializeLogMessagesWithDateTime(pMessageList, pBuffer);
   }
   // the special 'Iguana-Service' component doesn't have any consumers, its pLogPosition will always be NULL
   // pLogPositions could be empty when flushing to the very first log file of a component
   if (pBuffer->size() && FlushPositions && pLogPositions && pLogPositions->size()) {
      COL_TRC("Serializing consumer queue positions");
      SDBserializeLogPositions(ComponentId, pLogPositions, pBuffer);
   }
   return pBuffer->size() > BufferSize;
}

COLstring SDBserializeLogMessages(COLlist<std::shared_ptr<SDBmessage>>* pMessageList, const COLhashmap<COLstring, SDBid>* pLogPositions, bool FlushPositions, COLstring* pBuffer, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBserializeLogMessages);
   // if we have messages with the same timestamp as FileTime, we have to write them to the current file, no matter the size
   SDBmessage* pMessage = pMessageList->front().get();
   COLuint64   MsgTime  = pMessage->Id.MessageId.DateTime;
   COLstring   LastFile = SDBfindLogFile(pMessage->Id.MessageId.ComponentId, MsgTime, LogRootDir);
   COL_VAR(LastFile);
   if (!LastFile.is_null() && SDBserializeLogMessagesForFile(LastFile, pMessageList, pLogPositions, FlushPositions, pBuffer, LogRootDir)) { return LastFile; }
   COLstring   NextFile = SDBlogFile(pMessage->Id.MessageId.ComponentId, MsgTime, LogRootDir);
   // force position flush because NextFile is a new log file, which needs the queue position map
   // if NextFile is the first log file for a component, the queue position map may be empty if the component has no consumers
   SDBserializeLogMessagesForFile(NextFile, pMessageList, pLogPositions, true, pBuffer, LogRootDir);
   return NextFile;
}

static void SDBflushLogMessagesAndPositions(SDBlogTaskData& Data, const COLstring& EncryptionKey, SDBlogHandle& Handle, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBflushLogMessagesAndPositions);
   COLstring Buffer;
   while (Data.pMessages->size()) {
      COLstring LogFile = SDBserializeLogMessages(Data.pMessages, Data.pPositions, Data.FlushPosition, &Buffer, LogRootDir);
      COL_VAR(LogFile);
      int FD = SDBgetLogHandle(Handle, LogFile);

      const char* pBuffer = Buffer.get_buffer();
      int Size = Buffer.size();
      COLstring EncryptedData;
      if (!EncryptionKey.is_null()) {
         COL_TRC("Encrypting data");
         SDBencryptAndSerialize(Buffer, EncryptionKey, &EncryptedData);
         pBuffer = EncryptedData.get_buffer();
         Size = EncryptedData.size();
      }
      COL_TRC("Writing data to file");
      int Amount = SDBwriteLogFile(FD, pBuffer, Size);
      if (getenv(SDB_SYNC_WRITE_ENV_NAME)) {
         COL_TRC("Flush message and position to log file " << LogFile);
         SDBcommitLogFile(FD);
      }
      Buffer.clear();
   }
}

void SDBflushLogMessages(SDBlogHandle& Handle, SDBlogTaskData& Data, const COLstring& EncryptionKey, const COLstring& LogRootDir) {
   if (Data.pMessages->size()) {
      SDBflushLogMessagesAndPositions(Data, EncryptionKey, Handle, LogRootDir);
   } else {
      SDBflushLogPositionsOnly(Data.pPositions, EncryptionKey, Handle, LogRootDir);
   }
}

// WORKER THREAD
void SDBwriteLogFiles(COLmap<COLstring, SDBlogHandle>& HandleMap, COLmap<COLstring, SDBlogTaskData>* pData, const COLstring& EncryptionKey, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBwriteLogFiles);
   for (auto i = pData->begin(); i != pData->end(); i++) {
      const COLstring& ComponentId = i.key();
      SDBlogTaskData& Data = i.value();
      SDBflushLogMessages(HandleMap[ComponentId], Data, EncryptionKey, LogRootDir);
   }
   SDBcloseIdleHandles(HandleMap);
}

static void SDBprepareDataForComponent(const COLstring& ComponentId, SDBcomponent& Component, SDBlogTaskData& TaskData, SDBid& DoneData){
   COL_FUNCTION(SDBprepareDataForComponent);
   // swap the buffer
   TaskData.pMessages = Component.pLogMessages;
   Component.pLogMessages = new COLlist<std::shared_ptr<SDBmessage>>();
   if (TaskData.pMessages->size()) {
      DoneData = TaskData.pMessages->back().get()->Id;
   }
   COLauto<COLhashmap<COLstring, SDBid>> pLogPositions = new COLhashmap<COLstring, SDBid>();
   auto Consumer = Component.Consumers.begin();
   while (Consumer != Component.Consumers.end()) {
      (*pLogPositions)[Consumer.key()] = Consumer.value().Position;
      if (SDBdeleteConsumer(Consumer.value().Position.MessageId)) {
         COL_TRC("Consumer " << Consumer.key() << " marked for deletion. Removing it now");
         Consumer = Component.Consumers.erase(Consumer);
      } else {
         Consumer++;
      }
   }
   // In case we need to create new log files due to rollovers, passing in pLogPositions so that we have
   // consumer queue position map at the beginning of every log file.
   // In case we don't have any new queue positions to flush, determined by Component.LogFlushPositions,
   // don't flush pLogPositions to save space.
   TaskData.pPositions = pLogPositions;
   TaskData.FlushPosition = Component.LogFlushPositions;
   // LogFlushPositions is set by journal flush, which happens more frequently than log flush.
   // Reset it now so that we don't miss any flush events.
   Component.LogFlushPositions = false;
}

// MAIN THREAD
void SDBwriteLogs(SDBsystem* pSystem, COLclosure0* pCallback, const COLlist<COLstring>& ComponentList){
   COL_FUNCTION(SDBwriteLogs);
   COLauto<COLmap<COLstring, SDBlogTaskData>>   pTaskData = new COLmap<COLstring, SDBlogTaskData>();
   COLmap<SDBcomponent*, SDBid>* pDoneData = new COLmap<SDBcomponent*, SDBid>();
   for (auto i = ComponentList.begin(); i != ComponentList.end(); i++) {
      const COLstring& ComponentId = *i;
      COL_VAR(ComponentId);
      COLASSERT(pSystem->ComponentMap.count(ComponentId) == 1);
      SDBcomponent& Component = pSystem->ComponentMap[ComponentId];
      SDBprepareDataForComponent(ComponentId, Component, (*pTaskData)[ComponentId], (*pDoneData)[&Component]);
   }
   int* pCount = new int(1);

   if (!SDBroot2().is_null()) {     // write to 2nd dir log
      COL_TRC("write to log file in " << SDBroot2());
      (*pCount)++;
      COLmap<COLstring, SDBlogTaskData>* pTaskData2 = new COLmap<COLstring, SDBlogTaskData>(*pTaskData);
      COL_VAR(*pCount);
      COLclosure0* pFlushed = COLnewClosure0(&SDBlogWritten, pDoneData, pCount, *pCount, pCallback);
      pSystem->LogTaskQueue2.put(SDBlogTask(SDB_LOG_WRITE, pFlushed, pTaskData2));
   }
   COL_TRC("write to log file in " << SDBroot());
   COLclosure0* pFlushed = COLnewClosure0(&SDBlogWritten, pDoneData, pCount, *pCount, pCallback);
   pSystem->LogTaskQueue.put(SDBlogTask(SDB_LOG_WRITE, pFlushed, pTaskData));
}
