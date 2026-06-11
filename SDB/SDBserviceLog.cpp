// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBserviceLog
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Friday 28 April 2023 - 11:02AM
// ---------------------------------------------------------------------------

#include <SDB/SDBcomponent.h>
#include <SDB/SDBcryptoFile.h>
#include <SDB/SDBdateTime.h>
#include <SDB/SDBlogWrite.h>
#include <SDB/SDBserviceLog.h>

#include <FIL/FILosDefs.h>
#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static SDBserviceLog g_ServiceLog;

static void SDBwriteToTxtFile(const SDBmessage* pMessage, int& TxtFD) {
   // COL_FUNCTION(SDBwriteToTxtFile);
   COLstring Time = SDBdate(pMessage->Id.MessageId.DateTime / 1000, "[%Y-%m-%d %H:%M:%S]");
   COLstring Data = Time + "\t" + pMessage->Data + "\n";
   delete pMessage;
   if (-1 == TxtFD) {
      TxtFD = SDBopenLogFile(FILpathAppend(FILworkingDir(), "ServiceErrorLog.txt"));
   }
   SDBwriteLogFile(TxtFD, Data.data(), Data.size());
   FILcommit(TxtFD);
}

static void SDBserviceTxtWorker(COLthreadQueue<SDBmessage*>* pQueue) {
   COL_FUNCTION(SDBserviceTxtWorker);
   COLstring CurrentLog;
   int TxtFD = -1;
   while (true) {
      SDBmessage* pMessage = pQueue->take();
      if (NULL == pMessage) {
         // COL_TRC("Received shutdown request. Shutting down...");
         break;
      }
      COL_TRC("Writing service log " << *pMessage);
      SDBwriteToTxtFile(pMessage, TxtFD);
   }
   // COL_TRC("Service log writer has shut down.");
   if (-1 != TxtFD) { SDBcloseLogFile(TxtFD); }
}

static void SDBwriteToLogFile(SDBmessage* pMessage, int& LogFD, COLstring& CurrentLog, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBwriteToLogFile);
   COLauto<COLlist<std::shared_ptr<SDBmessage>>> pMessageList = new COLlist<std::shared_ptr<SDBmessage>>();
   pMessageList->push_back(std::shared_ptr<SDBmessage>(pMessage));
   COLstring Buffer;
   COLstring LogFile = SDBserializeLogMessages(pMessageList, NULL, false, &Buffer, LogRootDir);
   COL_VAR(LogFile);
   if (LogFile != CurrentLog) {
      if (-1 != LogFD) { SDBcloseLogFile(LogFD); }
      SDBcheckLogDir(LogFile);
      LogFD = SDBopenLogFile(LogFile);
      CurrentLog = LogFile;
   }
   const char* pBuffer = Buffer.get_buffer();
   int Size = Buffer.size();
   COLstring EncryptedBuffer;
   if (!g_ServiceLog.EncryptionKey.is_null()) {
      COL_TRC("Encrypting data");
      SDBencryptAndSerialize(Buffer, g_ServiceLog.EncryptionKey, &EncryptedBuffer);
      pBuffer = EncryptedBuffer.get_buffer();
      Size = EncryptedBuffer.size();
   }
   int Amount = SDBwriteLogFile(LogFD, pBuffer, Size);
   FILcommit(LogFD);
}

static void SDBserviceLogWorker(COLthreadQueue<SDBmessage*>* pQueue, COLstring LogRootDir) {
   COL_FUNCTION(SDBserviceLogWorker);
   COLstring CurrentLog;
   int LogFD = -1;
   while (true) {
      SDBmessage* pMessage = pQueue->take();
      if (NULL == pMessage) {
         // COL_TRC("Received shutdown request. Shutting down...");
         break;
      }
      COL_TRC("Writing service log " << *pMessage);
      SDBwriteToLogFile(pMessage, LogFD, CurrentLog, LogRootDir);
   }
   // COL_TRC("Service log writer has shut down.");
   if (-1 != LogFD) { SDBcloseLogFile(LogFD); }
}


static COLuint64 SDBgetLastIguanaServiceSequenceId(const COLstring& LogEncryptionKey, const COLstring& LogRootDir) {
   SDBcomponentStatus Status;
   // we don't care about position map or last enqueued message id. Set them to disable them
   Status.PositionMap[SDB_SERVICE_COMPONENT_ID];
   Status.LastDataId.MessageId.ComponentId = SDB_SERVICE_COMPONENT_ID;
   SDBgetPositionMapLastDataAndLogId(SDB_SERVICE_COMPONENT_ID, LogRootDir, LogEncryptionKey, false, Status);
   int SequenceId = Status.LastLogId.SequenceId;
   if (SDB_INVALID_SID == SequenceId) {
      SequenceId = SDB_DEFAULT_SID;
   }
   return SequenceId;
}

static COLuint64 SDBinitializeIguanaService(const COLstring& LogEncryptionKey) {
   COLuint64 SequenceId = SDBgetLastIguanaServiceSequenceId(LogEncryptionKey, SDBroot());
   if (!SDBroot2().is_null()) {
      COLuint64 SequenceId2 = SDBgetLastIguanaServiceSequenceId(LogEncryptionKey, SDBroot2());
      if (SequenceId < SequenceId2) { SequenceId = SequenceId2; }
   }
   return SequenceId;
}

void SDBstartServiceLog(COLthreadPool* pPool, const COLstring& LogEncryptionKey) {
   SDBcontentMapInit();
   g_ServiceLog.SID = SDBinitializeIguanaService(LogEncryptionKey);
   g_ServiceLog.EncryptionKey = LogEncryptionKey;
   pPool->scheduleTask(COLnewClosure0(&SDBserviceTxtWorker, &g_ServiceLog.TxtQueue));
   pPool->scheduleTask(COLnewClosure0(&SDBserviceLogWorker, &g_ServiceLog.LogQueue, SDBroot()));
   if (!SDBroot2().is_null()) {
      pPool->scheduleTask(COLnewClosure0(&SDBserviceLogWorker, &g_ServiceLog.LogQueue2, SDBroot2()));
   }
}

void SDBstopServiceLog() {
   g_ServiceLog.TxtQueue.put(NULL);
   g_ServiceLog.LogQueue.put(NULL);
   if (!SDBroot2().is_null()) {
      g_ServiceLog.LogQueue2.put(NULL);
   }
}

SDBid SDBlogService(const COLstring& Data, const COLstring& Tags) {
   COL_FUNCTION(SDBlogService);
   COLlocker Lock(g_ServiceLog.Mutex);    // thread safety
   COLstring ServiceId = SDB_SERVICE_COMPONENT_ID; // IX-4287 trying to fix a crash
   SDBmessageId MessageId(ServiceId);
   if (MessageId.DateTime != g_ServiceLog.LastDateTime) {
      g_ServiceLog.NextMessageId = 1;
      g_ServiceLog.LastDateTime = MessageId.DateTime;
   } else {
      g_ServiceLog.NextMessageId++;
      MessageId.Index = g_ServiceLog.NextMessageId;
   }
   // Iguana-Service can't have queued messages, so leave pMessage->QID = 0
   SDBid Id(MessageId, ++g_ServiceLog.SID, 0);
   SDBmessage* pMessage = new SDBmessage();
   pMessage->Data = Data;
   pMessage->Type = SDB_TRACE;
   pMessage->Id = Id;
   pMessage->Tags = Tags;
   if (Tags.find("#startup") != npos || Tags.find("#shutdown") != npos) {
      // save startup and shutdown messages to ServiceErrorLog.txt
      SDBmessage* pTxt = new SDBmessage(*pMessage);
      g_ServiceLog.TxtQueue.put(pTxt);
   }
   if (!SDBroot2().is_null()) {
      // save to 2nd log dir if configured
      SDBmessage* pMessage2 = new SDBmessage(*pMessage);
      g_ServiceLog.LogQueue2.put(pMessage2);
   }
   g_ServiceLog.LogQueue.put(pMessage);
   return Id;
}
