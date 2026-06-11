// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testDequeue
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Thursday 16 March 2023 - 09:55PM
// ---------------------------------------------------------------------------
#include "testDequeue.h"

#include <memory>

#include <UNIT/UNITapp.h>

#include <SDBT/SDBTmessage.h>
#include <SDBT/SDBTreader.h>

#include <SDB/SDBmessage.h>
#include <SDB/SDBmessageId.h>
// #include <SDB/SDBreader.h> // for SDB_END_OF_LOG

#include <SCK/SCKloop.h>

#include <FIL/FILutils.h>

#include <COL/COLclosure.h>
#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <SDB/SDBenvelope.h>
#include <SDB/SDBfile.h>
#include <SDB/SDBdequeueMulti.h>
#include <SDB/SDBqueueReader.h>



static void SDBonDequeuedMessage(int Code, SDBmessage* pMessage, SDBdequeueMulti* pDequeue, SCKloop* pLoop, COLlist<std::shared_ptr<SDBmessage>>* pCache) {
   COL_FUNCTION(SDBonDequeuedMessage);
   COL_VAR2(Code, pMessage);
   if (0 == Code) {
      SDBremoveCommittedMessageMulti(pDequeue, pMessage->Id);
      SDBdequeueMessageMulti(pDequeue, pLoop);
   } else if (SDB_END_OF_LOG == Code) {
      pLoop->shutdown();
   } else {
      UNIT_ASSERT_MESSAGE(false, "You broke the queue reader.");
   }
}

SDBmessage* SDBTcreateTestMessage2(const COLstring& ComponentId, COLuint64 DateTime, COLuint64& SID, COLuint64& QID) {
   SDBmessage* pMessage = new SDBmessage();
   pMessage->Id.MessageId.ComponentId = ComponentId;
   pMessage->Id.MessageId.DateTime    = DateTime;
   pMessage->Id.MessageId.Index       = SID;
   pMessage->Id.SequenceId            = SID;
   pMessage->Data = "This is message number " + COLintToString(SID);
   switch (SID % 4) {
      case 0:  pMessage->Type = SDB_MESSAGE;   pMessage->Data += ", a Message log entry.";  pMessage->Id.QueueId = QID++;  break;
      case 1:  pMessage->Type = SDB_INFO;      pMessage->Data += ", an Info log entry.";    break;
      case 2:  pMessage->Type = SDB_WARNING;   pMessage->Data += ", a Warning log entry.";  break;
      case 3:  pMessage->Type = SDB_ERROR;     pMessage->Data += ", an Error log entry.";   break;
      default: COLASSERT(false);                                                            break;
   }
   SID++;
   return pMessage;
}

void SDBTcreateTestMessageList2(const COLstring& ComponentId, COLuint64 DateTime, COLarray<COLauto<SDBmessage>>* pOut) {
   COL_FUNCTION(SDBTcreateTestMessageList2);
   int Count = 100;
   COLuint64 SID = 1, QID = 1;
   for (int i = 0; i < Count; ++i) {
      pOut->push_back(SDBTcreateTestMessage2(ComponentId, DateTime, SID, QID));
   }
   COLASSERT(SID == Count + 1);
   COLASSERT(QID == Count / 4 + 1);
   COL_TRC("Created 100 message, including 25 SDB_MESSAGE messages");
}

static SDBmessageId SDBsetUpTestLogDirs(const COLstring& LogDirBaseName, COLstring* pLogDir1, COLstring* pLogDir2) {
   COLstring ComponentId = "Component_1";
   COLuint64 DateTime = SDBtime();

   COLarray<COLauto<SDBmessage>> MessageList;
   SDBTcreateTestMessageList2(ComponentId, DateTime, &MessageList);

   COLstring Buffer1, Buffer2;
   for (int i = 0; i < MessageList.size(); ++i) {
      SDBenvelopeWrite(&Buffer1, SDBTcreateSerializedTestMessage(MessageList[i]));
      SDBenvelopeWrite(&Buffer2, SDBTcreateSerializedTestMessage(MessageList[i]));
   }

   *pLogDir1 = LogDirBaseName + ".1";
   *pLogDir2 = LogDirBaseName + ".2";
   COLstring LogFile1 = SDBlogFile(ComponentId, DateTime, *pLogDir1);
   COLstring LogFile2 = SDBlogFile(ComponentId, DateTime, *pLogDir2);
   SDBcheckLogDir(LogFile1);
   SDBcheckLogDir(LogFile2);
   FILwriteFile(LogFile1, Buffer1);
   FILwriteFile(LogFile2, Buffer2);
   return MessageList[3]->Id.MessageId;
}

// both log dirs are identical
static void testOneOne() {
   COL_FUNCTION(testOneOne);
   // Complete logs in both log dirs
   COLstring LogDir1, LogDir2;
   SDBmessageId Config = SDBsetUpTestLogDirs("dir.oneone.multi.dequeue.test", &LogDir1, &LogDir2);

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   COLlist<std::shared_ptr<SDBmessage>> Cache; // empty
   SDBdequeueMulti* pDequeue = SDBcreateDequeueMulti(Config, &Cache, "", LogDir1, LogDir2);
   // SDBdequeue should dequeue all 25 SDB_MESSAGE messages from log file
   SDBsetOnDequeuedMulti(pDequeue, COLnewClosure2(SDBonDequeuedMessage, pDequeue, &Loop, &Cache));
   SDBdequeueMessageMulti(pDequeue, &Loop);
   Loop.start();

   delete pDequeue;
   // Clean up
   COLstring RemoveError;
   FILremoveFullDirNewSafe(LogDir1, &RemoveError);
   FILremoveFullDirNewSafe(LogDir2, &RemoveError);
}


static SDBmessageId SDBsetUpEvenOddLogDirs(const COLstring& LogDirBaseName, COLstring* pLogDir1, COLstring* pLogDir2) {
   COLstring ComponentId = "Component_1";
   COLuint64 DateTime = SDBtime();

   COLarray<COLauto<SDBmessage>> MessageList;
   SDBTcreateTestMessageList2(ComponentId, DateTime, &MessageList);

   COLstring Buffer1, Buffer2;
   for (int i = 0; i < MessageList.size(); ++i) {
      if (i % 2 == 0) {
         SDBenvelopeWrite(&Buffer1, SDBTcreateSerializedTestMessage(MessageList[i]));
      } else {
         SDBenvelopeWrite(&Buffer2, SDBTcreateSerializedTestMessage(MessageList[i]));
      }
   }

   *pLogDir1 = LogDirBaseName + ".1";
   *pLogDir2 = LogDirBaseName + ".2";
   COLstring LogFile1 = SDBlogFile(ComponentId, DateTime, *pLogDir1);
   COLstring LogFile2 = SDBlogFile(ComponentId, DateTime, *pLogDir2);
   SDBcheckLogDir(LogFile1);
   SDBcheckLogDir(LogFile2);
   FILwriteFile(LogFile1, Buffer1);
   FILwriteFile(LogFile2, Buffer2);
   return MessageList[3]->Id.MessageId;
}

// even SID in one dir, odd SID in other
// all queued messages happens to be in even SID dir only
static void testEvenOdd() {
   COL_FUNCTION(testEvenOdd);
   COLstring LogDir1, LogDir2;
   SDBmessageId Config = SDBsetUpEvenOddLogDirs("dir.evenodd.multi.dequeue.test", &LogDir1, &LogDir2);

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   COLlist<std::shared_ptr<SDBmessage>> Cache; // empty
   SDBdequeueMulti* pDequeue = SDBcreateDequeueMulti(Config, &Cache, "", LogDir1, LogDir2);
   // SDBdequeue should dequeue all 25 SDB_MESSAGE messages from log file
   SDBsetOnDequeuedMulti(pDequeue, COLnewClosure2(SDBonDequeuedMessage, pDequeue, &Loop, &Cache));
   SDBdequeueMessageMulti(pDequeue, &Loop);
   Loop.start();

   delete pDequeue;
   // Clean up
   COLstring RemoveError;
   FILremoveFullDirNewSafe(LogDir1, &RemoveError);
   FILremoveFullDirNewSafe(LogDir2, &RemoveError);
}


static SDBmessageId SDBsetUpFrontBackLogDirs(const COLstring& LogDirBaseName, COLstring* pLogDir1, COLstring* pLogDir2) {
   COLstring ComponentId = "Component_1";
   COLuint64 DateTime = SDBtime();

   COLarray<COLauto<SDBmessage>> MessageList;
   SDBTcreateTestMessageList2(ComponentId, DateTime, &MessageList);

   COLstring Buffer1, Buffer2;
   for (int i = 0; i < MessageList.size(); ++i) {
      if (i < MessageList.size() / 2) {
         SDBenvelopeWrite(&Buffer1, SDBTcreateSerializedTestMessage(MessageList[i]));
      } else {
         SDBenvelopeWrite(&Buffer2, SDBTcreateSerializedTestMessage(MessageList[i]));
      }
   }

   *pLogDir1 = LogDirBaseName + ".1";
   *pLogDir2 = LogDirBaseName + ".2";
   COLstring LogFile1 = SDBlogFile(ComponentId, DateTime, *pLogDir1);
   COLstring LogFile2 = SDBlogFile(ComponentId, DateTime, *pLogDir2);
   SDBcheckLogDir(LogFile1);
   SDBcheckLogDir(LogFile2);
   FILwriteFile(LogFile1, Buffer1);
   FILwriteFile(LogFile2, Buffer2);
   return MessageList[3]->Id.MessageId;
}

// first half SID in one dir, second half in other
// first half of queued messages in one dir, 2nd half in the other
// even if the dequeue start position is only in one log dir, dequeue should NOT report SDB_READER_BAD_CONFIG
static void testFrontBack() {
   COL_FUNCTION(testFrontBack);
   COLstring LogDir1, LogDir2;
   SDBmessageId Config = SDBsetUpFrontBackLogDirs("dir.frontback.multi.dequeue.test", &LogDir1, &LogDir2);

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   COLlist<std::shared_ptr<SDBmessage>> Cache; // empty
   SDBdequeueMulti* pDequeue = SDBcreateDequeueMulti(Config, &Cache, "", LogDir1, LogDir2);
   // SDBdequeue should dequeue all 25 SDB_MESSAGE messages from log file
   SDBsetOnDequeuedMulti(pDequeue, COLnewClosure2(SDBonDequeuedMessage, pDequeue, &Loop, &Cache));
   SDBdequeueMessageMulti(pDequeue, &Loop);
   Loop.start();

   delete pDequeue;
   // Clean up
   COLstring RemoveError;
   FILremoveFullDirNewSafe(LogDir1, &RemoveError);
   FILremoveFullDirNewSafe(LogDir2, &RemoveError);
}

static void SDBonBadStart(int Code, SDBmessage* pMessage, SDBdequeueMulti* pDequeue, SCKloop* pLoop, COLlist<std::shared_ptr<SDBmessage>>* pCache) {
   COL_FUNCTION(SDBonBadStart);
   COL_VAR2(Code, pMessage);
   pLoop->shutdown();
   UNIT_ASSERT_EQUALS(SDB_READER_BAD_CONFIG, Code);
}

// Dequeue start position is before the first message in the queue
static void testBadStart() {
   COL_FUNCTION(testFrontBack);
   COLstring LogDir1, LogDir2;
   SDBmessageId Config = SDBsetUpTestLogDirs("dir.badstart.multi.dequeue.test", &LogDir1, &LogDir2);
   // mess up Config to trigger bad startup position error
   Config.DateTime--;

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   COLlist<std::shared_ptr<SDBmessage>> Cache; // empty
   SDBdequeueMulti* pDequeue = SDBcreateDequeueMulti(Config, &Cache, "", LogDir1, LogDir2);
   // SDBdequeue should dequeue all 25 SDB_MESSAGE messages from log file
   SDBsetOnDequeuedMulti(pDequeue, COLnewClosure2(SDBonBadStart, pDequeue, &Loop, &Cache));
   SDBdequeueMessageMulti(pDequeue, &Loop);
   Loop.start();

   delete pDequeue;
   // Clean up
   COLstring RemoveError;
   FILremoveFullDirNewSafe(LogDir1, &RemoveError);
   FILremoveFullDirNewSafe(LogDir2, &RemoveError);
}

static void SDBTcreateTestMessageList3(const COLstring& ComponentId, COLuint64 DateTime, COLarray<COLauto<SDBmessage>>* pOut, COLuint64& SID, COLuint64& QID) {
   COL_FUNCTION(SDBTcreateTestMessageList3);
   int Count = 100;
   for (int i = 0; i < Count; ++i) {
      pOut->push_back(SDBTcreateTestMessage2(ComponentId, DateTime, SID, QID));
   }
   COL_TRC("Created 100 message, including 25 SDB_MESSAGE messages");
}

static SDBmessageId SDBsetUpTestLogDirs2(const COLstring& LogDirBaseName, COLstring* pLogDir1, COLstring* pLogDir2, COLuint64& SID, COLuint64& QID, COLuint64 DateTime = SDBtime()) {
   COLstring ComponentId = "Component_1";

   COLarray<COLauto<SDBmessage>> MessageList;
   SDBTcreateTestMessageList3(ComponentId, DateTime, &MessageList, SID, QID);

   COLstring Buffer1, Buffer2;
   for (int i = 0; i < MessageList.size(); ++i) {
      SDBenvelopeWrite(&Buffer1, SDBTcreateSerializedTestMessage(MessageList[i]));
      SDBenvelopeWrite(&Buffer2, SDBTcreateSerializedTestMessage(MessageList[i]));
   }

   *pLogDir1 = LogDirBaseName + ".1";
   *pLogDir2 = LogDirBaseName + ".2";
   COLstring LogFile1 = SDBlogFile(ComponentId, DateTime, *pLogDir1);
   COLstring LogFile2 = SDBlogFile(ComponentId, DateTime, *pLogDir2);
   SDBcheckLogDir(LogFile1);
   SDBcheckLogDir(LogFile2);
   FILwriteFile(LogFile1, Buffer1);
   FILwriteFile(LogFile2, Buffer2);
   return MessageList[3]->Id.MessageId;
}

static void msgDqd(int Code, SDBmessage* pMessage, SDBdequeueMulti* pDequeue, SCKloop* pLoop) {
   COL_FUNCTION(msgDqd);
   COL_VAR(Code);
   if (0 == Code) {
      COL_VAR(pMessage->Id);
      COL_VAR(pMessage->Id.SequenceId);
      COL_VAR(pMessage->Id.QueueId);
      SDBremoveCommittedMessageMulti(pDequeue, pMessage->Id);
      SDBdequeueMessageMulti(pDequeue, pLoop);
   }
   else if (SDB_END_OF_LOG == Code) {
      pLoop->shutdown();
   }
   else {
      UNIT_ASSERT_MESSAGE(false, "You broke the queue reader. Code=" << Code);
   }
}

// 3 days's worth of complete logs in both log dirs
static void testMultipleDays() {
   COL_FUNCTION(testMultipleDays);
   COLstring LogDir1, LogDir2;
   COLuint64 DateTime = SDBtime();
   COLuint64 SID = 1, QID = 1;
   COLstring DirBaseName = "dir.day.multi.dequeue.test" + COLuint64ToString(DateTime);
   SDBmessageId Config = SDBsetUpTestLogDirs2(DirBaseName, &LogDir1, &LogDir2, SID, QID, DateTime - SDB_DAY * 2);
                         SDBsetUpTestLogDirs2(DirBaseName, &LogDir1, &LogDir2, SID, QID, DateTime - SDB_DAY * 1);
                         SDBsetUpTestLogDirs2(DirBaseName, &LogDir1, &LogDir2, SID, QID, DateTime);
   COL_VAR(Config);

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   COLlist<std::shared_ptr<SDBmessage>> Cache; // empty
   SDBdequeueMulti* pDequeue = SDBcreateDequeueMulti(Config, &Cache, "", LogDir1, LogDir2);
   SDBsetOnDequeuedMulti(pDequeue, COLnewClosure2(&msgDqd, pDequeue, &Loop));
   SDBdequeueMessageMulti(pDequeue, &Loop);
   Loop.start();

   delete pDequeue;
   // Clean up
   COLstring RemoveError;
   FILremoveFullDirNewSafe(LogDir1, &RemoveError);
   FILremoveFullDirNewSafe(LogDir2, &RemoveError);
}

static void testInitPositionBOQ() {
   COL_FUNCTION(testInitPositionBOQ);
   COLuint64 DateTime = SDBtime();
   SDBmessageId BOQ("Component_1", 0, 0);
   // Create dequeue that starts at Beginning Of Queue
   SDBdequeueMulti* pDequeue = SDBcreateDequeueMulti(BOQ, NULL, "", "", "");

   COLthreadPool Pool;
   SCKloop Loop(&Pool);

   // Simulate reader failed to fetch, should early return
   COL_TRC("reader couldn't fetch any messages due to errors");
   UNIT_ASSERT_EQUALS(false, SDBinitDequeuePositionMulti(pDequeue, &pDequeue->Reader2, &Loop, NULL));
   UNIT_ASSERT_EQUALS("", pDequeue->Position.MessageId.ComponentId);
   UNIT_ASSERT_EQUALS(SDB_INVALID_QID, pDequeue->Position.QueueId);

   // Simulate first reader fetched a message and merged into pDequeue->Messages
   COL_TRC("1st reader fetched message QID = 2, 2nd reader still fetching");
   SDBmessage* pMessage2 = new SDBmessage("Component_1", "2nd msg", DateTime, 2);
   pMessage2->Id.SequenceId = 32;
   pMessage2->Id.QueueId = 2;
   pDequeue->Messages[pMessage2->Id] = pMessage2;

   // Simulate 2nd reader still fetching data
   pDequeue->Reader2.Fetching = true;

   // initDequeuePosition should fail and wait for 2nd reader
   UNIT_ASSERT_EQUALS(false, SDBinitDequeuePositionMulti(pDequeue, &pDequeue->Reader2, &Loop, NULL));
   UNIT_ASSERT_EQUALS("", pDequeue->Position.MessageId.ComponentId);
   UNIT_ASSERT_EQUALS(SDB_INVALID_QID, pDequeue->Position.QueueId);

   // Simulate 2nd reader fetched the same message and merged it into Message
   COL_TRC("2nd reader fetched message QID = 2 as well");
   pDequeue->Reader2.Fetching = false;

   // initDequeuePosition should succeed with position set to a fictitious position before pMessage2
   UNIT_ASSERT_EQUALS(true, SDBinitDequeuePositionMulti(pDequeue, &pDequeue->Reader2, &Loop, NULL));
   UNIT_ASSERT_EQUALS(pMessage2->Id.MessageId.Index - 1, pDequeue->Position.MessageId.Index);
   UNIT_ASSERT_EQUALS(pMessage2->Id.SequenceId -1, pDequeue->Position.SequenceId);
   UNIT_ASSERT_EQUALS(pMessage2->Id.QueueId -1, pDequeue->Position.QueueId);

   // Simulate 2nd reader fetched a message after the one fetched by 1st reader
   COL_TRC("2nd reader fetched message QID = 2 and 3");
   SDBmessage* pMessage3 = new SDBmessage("Component_1", "3rd msg", DateTime, 3);
   pMessage3->Id.SequenceId = 42;
   pMessage3->Id.QueueId = 3;
   pDequeue->Messages[pMessage3->Id] = pMessage3;

   // initDequeuePosition should succeed, with the same fictitious position before pMessage2
   UNIT_ASSERT_EQUALS(true, SDBinitDequeuePositionMulti(pDequeue, &pDequeue->Reader2, &Loop, NULL));
   UNIT_ASSERT_EQUALS(pMessage2->Id.MessageId.Index - 1, pDequeue->Position.MessageId.Index);
   UNIT_ASSERT_EQUALS(pMessage2->Id.SequenceId -1, pDequeue->Position.SequenceId);
   UNIT_ASSERT_EQUALS(pMessage2->Id.QueueId -1, pDequeue->Position.QueueId);

   // Simulate first message has no QID, i.e. logs from older versions of iguana that don't have QID and SID
   COL_TRC("message at BOQ does't have a QID because it's generated by older versions of iguana");
   pDequeue->Messages[pMessage2->Id]->Id.QueueId = SDB_INVALID_QID;

   // initDequeuePosition should succeed, 2nd msg set as the start position but QID = SDB_DEFAULT_QID
   UNIT_ASSERT_EQUALS(true, SDBinitDequeuePositionMulti(pDequeue, &pDequeue->Reader2, &Loop, NULL));
   UNIT_ASSERT_EQUALS(pMessage2->Id, pDequeue->Position);
   UNIT_ASSERT_EQUALS(SDB_DEFAULT_QID, pDequeue->Position.QueueId);

   // Simulate 2nd reader fetched the message before the one fetched by 1st reader
   COL_TRC("2nd reader fetched message QID = 1, 2, and 3");
   SDBmessage* pMessage1 = new SDBmessage("Component_1", "1st msg", DateTime, 1);
   pMessage1->Id.SequenceId = 30;
   pMessage1->Id.QueueId = 1;
   pDequeue->Messages[pMessage1->Id] = pMessage1;

   // initDequeuePosition should succeed, with position set to a fictitious position before pMessage1
   UNIT_ASSERT_EQUALS(true, SDBinitDequeuePositionMulti(pDequeue, &pDequeue->Reader2, &Loop, NULL));
   UNIT_ASSERT_EQUALS(pMessage1->Id.MessageId.Index - 1, pDequeue->Position.MessageId.Index);
   UNIT_ASSERT_EQUALS(pMessage1->Id.SequenceId -1, pDequeue->Position.SequenceId);
   UNIT_ASSERT_EQUALS(pMessage1->Id.QueueId -1, pDequeue->Position.QueueId);

   delete pDequeue;
}

static void StartPositionNotFound(int Code, SDBmessage* pMessage, SCKloop* pLoop) {
   COL_FUNCTION(StartPositionNotFound);
   UNIT_ASSERT_EQUALS(SDB_READER_BAD_CONFIG, Code);
   UNIT_ASSERT_EQUALS(0, pMessage);
   pLoop->shutdown();
}

static void testInitPositionRegular() {
   COL_FUNCTION(testInitPositionRegular);
   COLuint64 DateTime = SDBtime();
   SDBmessageId StartPosition("Component_1", DateTime, 42);
   SDBdequeueMulti* pDequeue = SDBcreateDequeueMulti(StartPosition, NULL, "", "", "");

   COLthreadPool Pool;
   SCKloop Loop(&Pool);

   // Simulate first reader doesn't have StartPosition message
   COL_TRC("1st reader fetched message after start position, 2nd reader still fetching");
   SDBmessage* pMessage2 = new SDBmessage("Component_1", "2nd msg", DateTime, 52);
   pMessage2->Id.SequenceId = 52;
   pMessage2->Id.QueueId = 12;
   pDequeue->Messages[pMessage2->Id] = pMessage2;

   // Simulate 2nd reader still fetching data
   pDequeue->Reader2.Fetching = true;

   // initDequeuePosition should fail and wait for 2nd reader
   UNIT_ASSERT_EQUALS(false, SDBinitDequeuePositionMulti(pDequeue, &pDequeue->Reader2, &Loop, NULL));
   UNIT_ASSERT_EQUALS("", pDequeue->Position.MessageId.ComponentId);
   UNIT_ASSERT_EQUALS(SDB_INVALID_QID, pDequeue->Position.QueueId);

   // Simulate 2nd reader fetched the same message and merged it into Message
   COL_TRC("2nd reader also fetched message after start position");
   pDequeue->Reader2.Fetching = false;

   // initDequeuePosition should fail and callback called with SDB_READER_BAD_CONFIG
   COLclosure2<int, SDBmessage*>* pCallback = COLnewClosure2(&StartPositionNotFound, &Loop);
   COL_VAR(pCallback);
   SDBsetOnDequeuedMulti(pDequeue, pCallback);
   UNIT_ASSERT_EQUALS(false, SDBinitDequeuePositionMulti(pDequeue, &pDequeue->Reader2, &Loop, pCallback));
   Loop.start();
   UNIT_ASSERT_EQUALS("", pDequeue->Position.MessageId.ComponentId);
   UNIT_ASSERT_EQUALS(SDB_INVALID_QID, pDequeue->Position.QueueId);

   // Simulate 2nd reader fetched the start position message
   COL_TRC("2nd reader fetched the start position message");
   COL_VAR(StartPosition);
   SDBmessage* pMessage1 = new SDBmessage("Component_1", "1st msg", StartPosition.DateTime, StartPosition.Index);
   COL_VAR(pMessage1->Id);
   pMessage1->Id.SequenceId = 42;
   pMessage1->Id.QueueId = 11;
   pDequeue->Messages[pMessage1->Id] = pMessage1;

   // initDequeuePosition should succeed, dequeue postion should be start pos and start pos message removed
   UNIT_ASSERT_EQUALS(true, SDBinitDequeuePositionMulti(pDequeue, &pDequeue->Reader2, &Loop, NULL));
   UNIT_ASSERT_EQUALS(StartPosition, pDequeue->Position.MessageId);
   UNIT_ASSERT_EQUALS(pMessage2->Id, pDequeue->Messages.begin().key());

   // Simulate 1st reader fetched the start position and next message, 2nd reader still fetching
   COL_TRC("1st reader fetched the start position message and the one after it, 2nd reader still fetching");
   // add back pMessage 1 removed in previous test
   pMessage1 = new SDBmessage("Component_1", "1st msg", StartPosition.DateTime, StartPosition.Index);
   COL_VAR(pMessage1->Id);
   pMessage1->Id.SequenceId = 42;
   pMessage1->Id.QueueId = 11;
   pDequeue->Messages[pMessage1->Id] = pMessage1;
   // simulate reader 2 fetching
   pDequeue->Reader2.Fetching = true;

   // initDequeuePosition should succeed, dequeue postion should be start pos and start pos message removed
   UNIT_ASSERT_EQUALS(true, SDBinitDequeuePositionMulti(pDequeue, &pDequeue->Reader2, &Loop, NULL));
   UNIT_ASSERT_EQUALS(StartPosition, pDequeue->Position.MessageId);
   UNIT_ASSERT_EQUALS(pMessage2->Id, pDequeue->Messages.begin().key());

   // Simulate message at beginning of queue doesn't have QID
   COL_TRC("message at BOQ does't have a QID because it's generated by older versions of iguana");
   // add back pMessage 1 removed in previous test
   pMessage1 = new SDBmessage("Component_1", "1st msg", StartPosition.DateTime, StartPosition.Index);
   COL_VAR(pMessage1->Id);
   pMessage1->Id.SequenceId = 42;
   pMessage1->Id.QueueId = SDB_INVALID_QID;
   pDequeue->Messages[pMessage1->Id] = pMessage1;

   // initDequeuePosition should succeed, dequeue postion should be start pos and start pos message removed
   UNIT_ASSERT_EQUALS(true, SDBinitDequeuePositionMulti(pDequeue, &pDequeue->Reader2, &Loop, NULL));
   UNIT_ASSERT_EQUALS(StartPosition, pDequeue->Position.MessageId);
   UNIT_ASSERT_EQUALS(pMessage2->Id, pDequeue->Messages.begin().key());
   UNIT_ASSERT_EQUALS(SDB_DEFAULT_QID, pDequeue->Position.QueueId);

   delete pDequeue;
}

static void MissingQid(int Code, SDBmessage* pMessage, SCKloop* pLoop) {
   COL_FUNCTION(MissingQid);
   UNIT_ASSERT_EQUALS(SDB_BAD_QID, Code);
   UNIT_ASSERT_EQUALS(0, pMessage);
   pLoop->shutdown();
}

static void messageDequeued(int Code, SDBmessage* pMessage, SCKloop* pLoop) {
   COL_FUNCTION(messageDequeued);
   UNIT_ASSERT_EQUALS(0, Code);
   UNIT_ASSERT_EQUALS(4, pMessage->Id.QueueId);
   pLoop->shutdown();
}

static void testDequeueFromMessages() {
   COL_FUNCTION(testDequeueFromMessages);
   COLuint64 DateTime = SDBtime();
   SDBmessageId StartPos("Component_1", DateTime, 1);
   SDBdequeueMulti* pDequeue = SDBcreateDequeueMulti(StartPos, NULL, "", "", "");
   // IX-4079_SDBid The reader positions used to be set to 0 by the constructor of SDBqueueReaderState
   // prior to the changeover to SDBid. Now they're set to invalid so manually update them to 0.
   pDequeue->Reader1.Position.QueueId = 0;
   pDequeue->Reader2.Position.QueueId = 0;

   COLthreadPool Pool;
   SCKloop Loop(&Pool);

   COL_TRC("No message to dequeue");
   UNIT_ASSERT_EQUALS(false, SDBdequeueFromMessagesMulti(pDequeue, &Loop, NULL));

   COL_TRC("Message with next QID is missing, wait for reader 2");
   pDequeue->Position.QueueId = 3;            // last dequeued message has QID 3
   pDequeue->Reader1.Position.QueueId = 7;    // reader1 fetched all messages upto QID 7
   SDBmessage* pMessage1 = new SDBmessage("Component_1", "1st msg", DateTime, 2);
   pMessage1->Id.QueueId = 5;
   pDequeue->Messages[pMessage1->Id] = pMessage1;     // next message fetched by reader 1 has QID 5

   // SDBdequeueFromMessagesMulti should return false cuz message may be in the other log dir
   UNIT_ASSERT_EQUALS(false, SDBdequeueFromMessagesMulti(pDequeue, &Loop, NULL));

   COL_TRC("Message with next QID is missing");
   pDequeue->Reader2.Position.QueueId = 7;
   COLclosure2<int, SDBmessage*>* pCallback = COLnewClosure2(&MissingQid, &Loop);
   COL_VAR(pCallback);
   SDBsetOnDequeuedMulti(pDequeue, pCallback);
   bool Result = SDBdequeueFromMessagesMulti(pDequeue, &Loop, pCallback);
   Loop.start();
   // SDBdequeueFromMessagesMulti should server bad QID error and return true
   UNIT_ASSERT_EQUALS(true, Result);

   COL_TRC("Message with next QID is available");
   pDequeue->Messages.begin().value()->Id.QueueId = 4;
   pCallback = COLnewClosure2(&messageDequeued, &Loop);
   SDBsetOnDequeuedMulti(pDequeue, pCallback);
   Result = SDBdequeueFromMessagesMulti(pDequeue, &Loop, pCallback);
   Loop.start();
   // SDBdequeueFromMessagesMulti should server bad QID error and return true
   UNIT_ASSERT_EQUALS(true, Result);

   delete pDequeue;
}

static void readError(int Code, SDBmessage* pMessage, SCKloop* pLoop, int ExpectedCode) {
   COL_FUNCTION(readError);
   UNIT_ASSERT_EQUALS(ExpectedCode, Code);
   UNIT_ASSERT_EQUALS(0, pMessage);
   pLoop->shutdown();
}

static void testDequeueFromFile() {
   COL_FUNCTION(testDequeueFromFile);
   COLuint64 DateTime = SDBtime();
   SDBmessageId StartPos("Component_1", DateTime, 1);
   COLlist<std::shared_ptr<SDBmessage>> Cache; // empty
   SDBdequeueMulti* pDequeue = SDBcreateDequeueMulti(StartPos, &Cache, "", "", "");
   pDequeue->Position.MessageId = StartPos;

   COLthreadPool Pool;
   SCKloop Loop(&Pool);

   COL_TRC("Both reader report bad start pos");
   // first reader return SDB_READER_BAD_CONFIG should wait for 2nd reader
   pDequeue->Reader1.Code = SDB_READER_BAD_CONFIG;
   SDBmessageDequeuedFromFileMulti(SDB_READER_BAD_CONFIG, &pDequeue->Reader2, pDequeue, &Loop, NULL);
   COLclosure2<int, SDBmessage*>* pCallback = COLnewClosure2(&StartPositionNotFound, &Loop);
   SDBsetOnDequeuedMulti(pDequeue, pCallback);
   // first reader return SDB_READER_BAD_CONFIG should trigger SDB_READER_BAD_CONFIG error
   SDBmessageDequeuedFromFileMulti(SDB_READER_BAD_CONFIG, &pDequeue->Reader1, pDequeue, &Loop, pCallback);
   Loop.start();
   pDequeue->Responded = false;

   COL_TRC("1st reader EOL 2nd reader bad start pos");
   pDequeue->Reader1.Code = SDB_END_OF_LOG;
   pDequeue->Reader2.Code = SDB_READER_BAD_CONFIG;
   pCallback = COLnewClosure2(&readError, &Loop, 765405431);
   SDBsetOnDequeuedMulti(pDequeue, pCallback);
   // expect combo error code SDB_END_OF_LOG * 1000000 + SDB_READER_BAD_CONFIG
   SDBmessageDequeuedFromFileMulti(SDB_READER_BAD_CONFIG, &pDequeue->Reader1, pDequeue, &Loop, pCallback);
   Loop.start();
   pDequeue->Responded = false;

   COL_TRC("both reader report EOL");
   pDequeue->Reader1.Code = SDB_END_OF_LOG;
   pDequeue->Reader2.Code = SDB_END_OF_LOG;
   pCallback = COLnewClosure2(&readError, &Loop, SDB_END_OF_LOG);
   SDBsetOnDequeuedMulti(pDequeue, pCallback);
   // expect combo error code SDB_END_OF_LOG * 1000000 + SDB_READER_BAD_CONFIG
   SDBmessageDequeuedFromFileMulti(SDB_END_OF_LOG, &pDequeue->Reader1, pDequeue, &Loop, pCallback);
   Loop.start();

   delete pDequeue;
}

void testDequeueMultiple(UNITapp* pApp){
   COL_FUNCTION(testDequeueMultiple);
   pApp->add("dequeue/multi/complete",  &testOneOne);
   pApp->add("dequeue/multi/evenodd",   &testEvenOdd);
   pApp->add("dequeue/multi/frontback", &testFrontBack);
   pApp->add("dequeue/multi/bad/start", &testBadStart);
   pApp->add("dequeue/multi/day",       &testMultipleDays);
   pApp->add("dequeue/multi/init/pos/boq",  &testInitPositionBOQ);
   pApp->add("dequeue/multi/init/pos/reg",  &testInitPositionRegular);
   pApp->add("dequeue/multi/serve/msgs",    &testDequeueFromMessages);
   pApp->add("dequeue/multi/serve/file",    &testDequeueFromFile);
}
