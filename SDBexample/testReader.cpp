// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testReader
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Tuesday 28 February 2023 - 02:48PM
// ---------------------------------------------------------------------------
#include "testReader.h"

#include <math.h>

#include <UNIT/UNITapp.h>

#include <SDBT/SDBTenvelope.h>
#include <SDBT/SDBTfile.h>
#include <SDBT/SDBTdir.h>
#include <SDBT/SDBTmessage.h>
#include <SDBT/SDBTreader.h>
#include <SDBT/SDBTpositionMap.h>

#include <SDB/SDBdir.h>
#include <SDB/SDBenvelope.h>
#include <SDB/SDBfile.h>
#include <SDB/SDBfilter.h>
#include <SDB/SDBreader.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBposition.h>

#include <SCK/SCKloop.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <COL/COLauto.h>
#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void SDBTdestroyContentList(COLlist<SDBcontent*>& List) {
   while (List.size()) {
      SDBcontentDestroy(List.front());
      List.pop_front();
   }
}

static void testUnpack() {
   COL_FUNCTION(testUnpack);
   COLstring Buffer;
   // message
   SDBmessage* pExpectedMessage = SDBTcreateTestMessage("Component_0", 31415926);
   SDBTcreateEnvelopedTestMessage(&Buffer, pExpectedMessage);
   SDBcontent* pActualContent = SDBunpackContent(Buffer.data(), Buffer.size());
   UNIT_ASSERT_EQUALS(pExpectedMessage->Type, pActualContent->Type);
   UNIT_ASSERT_EQUALS(*pExpectedMessage, *(SDBmessage*)pActualContent);
   SDBcontentDestroy(pActualContent);
   Buffer.clear();

   // position
   pExpectedMessage->Type = SDB_MESSAGE;
   SDBposition ExpectedPosition = SDBposition("Consumer", pExpectedMessage->Id);
   SDBTcreateEnvelopedTestPosition(&Buffer, ExpectedPosition);
   pActualContent = SDBunpackContent(Buffer.data(), Buffer.size());
   UNIT_ASSERT_EQUALS(ExpectedPosition.Type, pActualContent->Type);
   UNIT_ASSERT_EQUALS(ExpectedPosition, *(SDBposition*)pActualContent);
   SDBcontentDestroy((SDBcontent*)pExpectedMessage);
   SDBcontentDestroy(pActualContent);
   Buffer.clear();

   // position map
   SDBpositionMap ExpectedMap = SDBTcreateTestPositionMap();
   SDBTcreateEnvelopedTestPositionMap(&Buffer, ExpectedMap);
   pActualContent = SDBunpackContent(Buffer.data(), Buffer.size());
   UNIT_ASSERT_EQUALS(ExpectedMap.Type, pActualContent->Type);
   UNIT_ASSERT_EQUALS(ExpectedMap, *(SDBpositionMap*)pActualContent);
   SDBcontentDestroy(pActualContent);
}

static void SDBTallowAllContent(const SDBcontent* pContent, bool* pAllowed) {
   *pAllowed = true;
}

static void SDBTblockAllContent(const SDBcontent* pContent, bool* pAllowed) {
   *pAllowed = false;
}

static void SDBTpositionOnly(const SDBcontent* pContent, bool* pAllowed) {
   *pAllowed = SDB_POSITION == pContent->Type;
}

static void testExtract() {
   COL_FUNCTION(testExtract);
   COLstring Buffer;
   Buffer.setCapacity(100 * (1000+100));
   // 1 position map, 1 component, 100 (25 of each message type) messages and 24 (25-1) positions
   SDBTcreateTestJournalData(&Buffer, SDBTcreateTestLogData(100, 1, time(NULL), 1));

   COLlist<SDBcontent*> Contents;
   COLauto<COLclosure2<const SDBcontent*, bool*>> pFilter;
   pFilter = COLnewClosure2(&SDBTblockAllContent);
   SDBextractMessages(Buffer.data(), Buffer.size(), pFilter, &Contents);
   UNIT_ASSERT_EQUALS(0, Contents.size());

   pFilter = COLnewClosure2(&SDBTpositionOnly);
   SDBextractMessages(Buffer.data(), Buffer.size(), pFilter, &Contents);
   UNIT_ASSERT_EQUALS(24, Contents.size());
   while (Contents.size()) {
      SDBcontentDestroy(Contents.front());
      Contents.pop_front();
   }

   pFilter = COLnewClosure2(&SDBTallowAllContent);
   SDBextractMessages(Buffer.data(), Buffer.size(), pFilter, &Contents);
   UNIT_ASSERT_EQUALS(125, Contents.size());
   while (Contents.size()) {
      SDBcontentDestroy(Contents.front());
      Contents.pop_front();
   }
}

static COLstring SDBTenvelopSerialize(const SDBmessage& Message) {
   COL_FUNCTION(SDBTenvelopSerialize);
   COLstring Content;
   Content.setCapacity(SDBmessageSize(&Message));
   int Amount = SDBwriteMessage(&Message, Content.get_buffer(), Content.capacity()-1);
   Content.setSize(Amount);
   COLstring Envelope;
   SDBenvelopeWrite(&Envelope, Content);
   return Envelope;
}

static void testExtractCorrupt1() {
   COL_FUNCTION(testExtractCorrupt1);
   COLuint64 Now = SDBtime();
   SDBmessage A("qwer", "tyui", Now, 1);
   // SDBreadContent() auto-adds #message. Mod A for test
   A.Tags = "#message";
   SDBmessage B("asdf", "ghjk", Now, 2);
   // SDBreadContent() auto-adds #message. Mod B for test
   B.Tags = "#message";
   SDBmessage C("zxcv", "bnm,", Now, 3);
   // SDBreadContent() auto-adds #message. Mod C for test
   C.Tags = "#message";

   COLstring Buffer, Envelope;
   Buffer.append(SDBTenvelopSerialize(A));
   Buffer.append(SDBTenvelopSerialize(B));
   Envelope = SDBTenvelopSerialize(C);
   // corrupt message C
   Envelope[Envelope.size() - 3] -= 5;
   SDBcorruptRegion Corruption(Buffer.size(), Envelope.size());
   Buffer.append(Envelope);

   COLlist<SDBcontent*> Contents;
   COLlist<SDBcorruptRegion> Corruptions;
   COLauto<COLclosure2<const SDBcontent*, bool*>> pFilter;
   pFilter = COLnewClosure2(&SDBTallowAllContent);
   SDBextractMessages(Buffer.data(), Buffer.size(), pFilter, &Contents, &Corruptions);
   UNIT_ASSERT_EQUALS(2, Contents.size());
   UNIT_ASSERT_EQUALS(A, *(SDBmessage*)Contents.front());
   UNIT_ASSERT_EQUALS(B, *(SDBmessage*)Contents.back());
   UNIT_ASSERT_EQUALS(Corruption.Position, Corruptions.front().Position);
   UNIT_ASSERT_EQUALS(Corruption.Size, Corruptions.front().Size);
   SDBTdestroyContentList(Contents);
}

static void testExtractCorrupt2() {
   COL_FUNCTION(testExtractCorrupt2);
   COLuint64 Now = SDBtime();
   SDBmessage A("qwer", "tyui", Now, 1);
   SDBmessage B("asdf", "ghjk", Now, 2);
   SDBmessage C("zxcv", "bnm,", Now, 3);

   COLstring Buffer, Envelope;
   Buffer.append(SDBTenvelopSerialize(A));
   Envelope = SDBTenvelopSerialize(B);
   // corrupt message B
   Envelope[Envelope.size() - 3] -= 5;
   SDBcorruptRegion Corruption(Buffer.size(), Envelope.size());
   Buffer.append(Envelope);
   Buffer.append(SDBTenvelopSerialize(C));

   COLlist<SDBcontent*> Contents;
   COLlist<SDBcorruptRegion> Corruptions;
   COLauto<COLclosure2<const SDBcontent*, bool*>> pFilter;
   pFilter = COLnewClosure2(&SDBTallowAllContent);
   SDBextractMessages(Buffer.data(), Buffer.size(), pFilter, &Contents, &Corruptions);
   UNIT_ASSERT_EQUALS(2, Contents.size());
   UNIT_ASSERT_EQUALS(A, *(SDBmessage*)Contents.front());
   UNIT_ASSERT_EQUALS(C, *(SDBmessage*)Contents.back());
   UNIT_ASSERT_EQUALS(Corruption.Position, Corruptions.front().Position);
   UNIT_ASSERT_EQUALS(Corruption.Size, Corruptions.front().Size);
   SDBTdestroyContentList(Contents);
}

static void testExtractCorrupt3() {
   COL_FUNCTION(testExtractCorrupt3);
   COLuint64 Now = SDBtime();
   SDBmessage A("qwer", "tyui", Now, 1);
   SDBmessage B("asdf", "ghjk", Now, 2);
   SDBmessage C("zxcv", "bnm,", Now, 3);

   COLstring Buffer, Envelope;
   Envelope = SDBTenvelopSerialize(A);
   // corrupt message A
   Envelope[Envelope.size() - 3] -= 5;
   SDBcorruptRegion CorruptionA(Buffer.size(), Envelope.size());
   Buffer.append(Envelope);
   Buffer.append(SDBTenvelopSerialize(B));
   Envelope = SDBTenvelopSerialize(C);
   // corrupt message C
   Envelope[Envelope.size() - 3] -= 5;
   SDBcorruptRegion CorruptionC(Buffer.size(), Envelope.size());
   Buffer.append(Envelope);

   COLlist<SDBcontent*> Contents;
   COLlist<SDBcorruptRegion> Corruptions;
   COLauto<COLclosure2<const SDBcontent*, bool*>> pFilter;
   pFilter = COLnewClosure2(&SDBTallowAllContent);
   SDBextractMessages(Buffer.data(), Buffer.size(), pFilter, &Contents, &Corruptions);
   UNIT_ASSERT_EQUALS(1, Contents.size());
   UNIT_ASSERT_EQUALS(B, *(SDBmessage*)Contents.front());
   UNIT_ASSERT_EQUALS(2, Corruptions.size());
   UNIT_ASSERT_EQUALS(CorruptionA.Position, Corruptions.front().Position);
   UNIT_ASSERT_EQUALS(CorruptionA.Size,     Corruptions.front().Size);
   UNIT_ASSERT_EQUALS(CorruptionC.Position, Corruptions.back().Position);
   UNIT_ASSERT_EQUALS(CorruptionC.Size,     Corruptions.back().Size);
   SDBTdestroyContentList(Contents);
}

static void testExtractCorrupt4() {
   COL_FUNCTION(testExtractCorrupt4);
   COLuint64 Now = SDBtime();
   SDBmessage A("qwer", "tyui", Now, 1);
   SDBmessage B("asdf", "ghjk", Now, 2);
   SDBmessage C("zxcv", "bnm,", Now, 3);

   COLstring Buffer, Envelope;
   Buffer.append(SDBTenvelopSerialize(A));
   Envelope = SDBTenvelopSerialize(B);
   // corrupt marker B
   Envelope[1] = '(';
   SDBcorruptRegion Corruption(Buffer.size(), Envelope.size());
   Buffer.append(Envelope);
   Buffer.append(SDBTenvelopSerialize(C));

   COLlist<SDBcontent*> Contents;
   COLlist<SDBcorruptRegion> Corruptions;
   COLauto<COLclosure2<const SDBcontent*, bool*>> pFilter;
   pFilter = COLnewClosure2(&SDBTallowAllContent);
   SDBextractMessages(Buffer.data(), Buffer.size(), pFilter, &Contents, &Corruptions);
   UNIT_ASSERT_EQUALS(2, Contents.size());
   UNIT_ASSERT_EQUALS(A, *(SDBmessage*)Contents.front());
   UNIT_ASSERT_EQUALS(C, *(SDBmessage*)Contents.back());
   UNIT_ASSERT_EQUALS(Corruption.Position, Corruptions.front().Position);
   UNIT_ASSERT_EQUALS(Corruption.Size, Corruptions.front().Size);
   SDBTdestroyContentList(Contents);
}

static void SDBTfetchedMessage(int Code, SDBmessage* pMessage, SDBreader* pReader, SCKloop* pLoop, int* pCount) {
   COL_FUNCTION(SDBTfetchedMessage);
   if (0 == Code) {
      UNIT_ASSERT_EQUALS((*pCount)++, pMessage->Id.MessageId.Index);
      delete pMessage;
      SDBreaderFetchMessage(pReader, pLoop, COLnewClosure2(&SDBTfetchedMessage, pReader, pLoop, pCount));
   } else if (SDB_END_OF_LOG == Code) {
      pLoop->shutdown();
   } else {
      UNIT_ASSERT_MESSAGE(false, "You broke the queue reader.");
   }
}

static void testSimple() {
   COL_FUNCTION(testSimple);
   COLstring File = "fetch.reader.test";

   SDBreader Reader;
   Reader.FilePath = File;
   SDBTcreateTestLogFile(File, time(NULL), 100);

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   int Count = 1;
   SDBreaderFetchMessage(&Reader, &Loop, COLnewClosure2(&SDBTfetchedMessage, &Reader, &Loop, &Count));
   Loop.start();

   // Clean up
   FILremoveWithThrow(File);
}

static void SDBTfetchedLargeMessage(int Code, SDBmessage* pMessage, SDBreader* pReader, SCKloop* pLoop, int MsgSize) {
   COL_FUNCTION(SDBTfetchedLargeMessage);
   if (0 == Code) {
      UNIT_ASSERT(pMessage->Data.size() > MsgSize);
      delete pMessage;
      SDBreaderFetchMessage(pReader, pLoop, COLnewClosure2(&SDBTfetchedLargeMessage, pReader, pLoop, MsgSize));
   } else if (SDB_END_OF_LOG == Code) {
      pLoop->shutdown();
   } else {
      UNIT_ASSERT_MESSAGE(false, "You broke the queue reader.");
   }
}

static void testLarge() {
   COL_FUNCTION(testLarge);
   COLstring File = "large.fetch.reader.test";

   SDBreader Reader;
   Reader.FilePath = File;
   // create 1 message that's larger than SDB_READER_BUF_LOAD_SIZE
   int MessageSize = 2 * SDB_READER_BUF_LOAD_SIZE + 1048576;
   SDBTcreateTestLogFile(File, time(NULL), 1, 1, "Component_1", MessageSize);

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   SDBreaderFetchMessage(&Reader, &Loop, COLnewClosure2(&SDBTfetchedLargeMessage, &Reader, &Loop, MessageSize));
   Loop.start();

   // Clean up
   FILremoveWithThrow(File);
}

static void SDBTgetNextFile(bool* pSuccess, COLstring& NextFile, COLstring NextFileName) {
   COL_FUNCTION(SDBTgetNextFile);
   static bool HasNext = true;
   if (HasNext) {
      NextFile = NextFileName;
      HasNext = false;
   }
   *pSuccess = HasNext;
}

static void testNextFile() {
   COL_FUNCTION(testNextFile);
   COLstring File1 = "1.file.fetch.reader.test";
   COLstring File2 = "2.file.fetch.reader.test";
   SDBTcreateTestLogFile(File1, time(NULL), 30,  1);
   SDBTcreateTestLogFile(File2, time(NULL), 30, 31);

   SDBreader Reader;
   Reader.FilePath = File1;
   Reader.pNextFileCall = COLnewClosure2(&SDBTgetNextFile, File2);

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   int Count = 1;
   SDBreaderFetchMessage(&Reader, &Loop, COLnewClosure2(&SDBTfetchedMessage, &Reader, &Loop, &Count));
   Loop.start();

   // Clean up
   FILremoveWithThrow(File1);
   FILremoveWithThrow(File2);
}

// ELIOT - you cannot throw exceptions from on timer events.
static void SDBTonPowerMessage(int Code, SDBmessage* pMessage, SDBreader* pReader, SCKloop* pLoop, int* pCount, bool* pError, int* pReqNum) {
   COL_FUNCTION(SDBTonPowerMessage);
   static int ResponseCount = 1;
   COL_TRC("Fetch Response " << ResponseCount++);
   (*pCount)--;
   static int LastMsgId = 0;
   if (0 == Code) {
      COL_VAR2(*pCount, pMessage->Id.MessageId);
      if (pMessage->Id.MessageId.Index - LastMsgId == 1) {
         LastMsgId = pMessage->Id.MessageId.Index;
      } else {
         COL_ERR("Expecting message " << LastMsgId + 1);
         COLcout << "Expecting message " << LastMsgId + 1 << ", got " << pMessage->Id.MessageId << newline;
         *pError = true;
         pLoop->shutdown();
      }
      delete pMessage;
      (*pCount)++;
      COL_TRC("Fetch Request " << ++(*pReqNum));
      SDBreaderFetchMessage(pReader, pLoop, COLnewClosure2(&SDBTonPowerMessage, pReader, pLoop, pCount, pError, pReqNum));
      // amplify fetch requests
      (*pCount)++;
      COL_TRC("Fetch Request " << ++(*pReqNum));
      SDBreaderFetchMessage(pReader, pLoop, COLnewClosure2(&SDBTonPowerMessage, pReader, pLoop, pCount, pError, pReqNum));
   } else if (SDB_END_OF_LOG == Code) {
      COL_VAR(*pCount);
      if (0 == *pCount && LastMsgId == 3000) { pLoop->shutdown(); }
   } else {
      COL_ERR("You broke the queue reader.");
      COLcout << "Error code: " << Code << newline;
      *pError = true;
      pLoop->shutdown();
   }
}

static void testLogPower() {
   COL_FUNCTION(testLogPower);
   COLstring TestDir     = "dir.reader.power.log.test";
   COLstring RemoveError;
   if (FILfileExists(TestDir)) { FILremoveFullDirNewSafe(TestDir, &RemoveError); }
   COLstring ComponentId = "Component_0";
   time_t    StartDate   = time(NULL) - SDB_DAY * 30;
   // 30 days, 100 messages (25 of each type) each day
   SDBTcreateTestLogFiles(StartDate, 30, TestDir, ComponentId);

   SDBreader* pReader = SDBcreateLogReader(ComponentId, StartDate*1000, "", NULL, TestDir);

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   int Count = 1;
   int ReqNum = 0;
   bool Error = false;
   COL_TRC("Fetch Request " << ++ReqNum);
   SDBreaderFetchMessage(pReader, &Loop, COLnewClosure2(&SDBTonPowerMessage, pReader, &Loop, &Count, &Error, &ReqNum));
   Loop.start();

   SDBdeleteReader(pReader);
   // Clean up
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
   UNIT_ASSERT_EQUALS(false, Error);
}

static void SDBTonLogMessage(int Code, SDBmessage* pMessage, SDBreader* pReader, SCKloop* pLoop) {
   COL_FUNCTION(SDBTonLogMessage);
   static int InfoCount    = 0;
   static int ErrorCount   = 0;
   static int MessageCount = 0;
   static int WarningCount = 0;
   if (0 == Code) {
      switch (pMessage->Type) {
         case SDB_MESSAGE:    MessageCount++;   break;
         case SDB_INFO:       InfoCount++;      break;
         case SDB_WARNING:    WarningCount++;   break;
         case SDB_ERROR:      ErrorCount++;     break;
         default: UNIT_ASSERT_MESSAGE(false, "You broke the log reader.");
      }
      delete pMessage;
      SDBreaderFetchMessage(pReader, pLoop, COLnewClosure2(&SDBTonLogMessage, pReader, pLoop));
   } else if (SDB_END_OF_LOG == Code) {
      pLoop->shutdown();
      UNIT_ASSERT_EQUALS(0, InfoCount    % 25);
      UNIT_ASSERT_EQUALS(0, ErrorCount   % 25);
      UNIT_ASSERT_EQUALS(0, MessageCount % 25);
      UNIT_ASSERT_EQUALS(0, WarningCount % 25);
   } else {
      UNIT_ASSERT_MESSAGE(false, "You broke the queue reader.");
   }
}

static void testLog() {
   COL_FUNCTION(testLog);
   COLstring TestDir     = "dir.reader.log.test";
   COLstring RemoveError;
   if (FILfileExists(TestDir)) { FILremoveFullDirNewSafe(TestDir, &RemoveError); }
   COLstring ComponentId = "Component_0";
   time_t    StartDate   = time(NULL) - SDB_DAY * 30;
   // 30 days, 100 messages (25 of each type) each day
   SDBTcreateTestLogFiles(StartDate, 30, TestDir, ComponentId);

   SDBreader* pReader = SDBcreateLogReader(ComponentId, StartDate*1000, "", NULL, TestDir);

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   SDBreaderFetchMessage(pReader, &Loop, COLnewClosure2(&SDBTonLogMessage, pReader, &Loop));
   Loop.start();

   SDBdeleteReader(pReader);
   // Clean up
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
}

static void SDBTonQueueMessage(int Code, SDBmessage* pMessage, SDBreader* pReader, SCKloop* pLoop, int ExpectedId) {
   COL_FUNCTION(SDBTonQueueMessage);
   if (0 == Code) {
      UNIT_ASSERT_EQUALS(SDB_MESSAGE,        pMessage->Type);
      UNIT_ASSERT_EQUALS((ExpectedId/4+1)*4, pMessage->Id.MessageId.Index);
      delete pMessage;
      SDBreaderFetchMessage(pReader, pLoop, COLnewClosure2(&SDBTonQueueMessage, pReader, pLoop, ExpectedId+4));
   } else if (SDB_END_OF_LOG == Code) {
      pLoop->shutdown();
   } else {
      UNIT_ASSERT_MESSAGE(false, "You broke the queue reader.");
   }
}

static void testQueue() {
   COL_FUNCTION(testQueue);
   time_t Date = time(NULL);
   COLstring TestDir     = "dir.reader.queue.test";
   COLstring ComponentId = "Component_1";
   COLstring TestFile    = SDBlogFile(ComponentId, Date*1000, TestDir);

   COLarray<COLauto<SDBmessage>> MessageList;
   // 100 messages (25 of each type)
   SDBTcreateTestMessageList(&MessageList, 100, ComponentId, 1, 1000, Date);
   SDBTcreateTestLogFile(TestFile, MessageList);

   int StartId = 42;
   const SDBmessageId& Config = MessageList[StartId].get()->Id.MessageId;
   SDBreader* pReader = SDBcreateQueueReader(Config, "", false, TestDir);

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   SDBreaderFetchMessage(pReader, &Loop, COLnewClosure2(&SDBTonQueueMessage, pReader, &Loop, StartId));
   Loop.start();

   SDBdeleteReader(pReader);
   // Clean up
   COLstring RemoveError;
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
}

static void SDBTonManualMessage(int Code, SDBmessage* pMessage, SDBreader* pReader, SCKloop* pLoop, int ExpectedId) {
   COL_FUNCTION(SDBTonManualMessage);
   static int Count = 0;
   static COLint64 PrevId = 0;
   if (0 == Code) {
      // SDBreaderFetchMessage() should return the same message unless we SDBreaderRemoveMessageFromCache()
      UNIT_ASSERT_EQUALS(SDB_MESSAGE, pMessage->Type);
      if (Count++ % 2) {
         UNIT_ASSERT_EQUALS(PrevId, pMessage->Id.MessageId.Index);
         SDBreaderRemoveMessageFromCache(pReader, pMessage->Id);
      } else {
         UNIT_ASSERT(PrevId != pMessage->Id.MessageId.Index);
         PrevId = pMessage->Id.MessageId.Index;
      }
      SDBreaderFetchMessage(pReader, pLoop, COLnewClosure2(&SDBTonManualMessage, pReader, pLoop, ExpectedId+4));
   } else if (SDB_END_OF_LOG == Code) {
      pLoop->shutdown();
   } else {
      UNIT_ASSERT_MESSAGE(false, "You broke the queue reader.");
   }
}

static void testManual() {
   COL_FUNCTION(testManual);
   COLuint64 Now = SDBtime();
   COLstring TestDir     = "dir.reader.manual.test";
   COLstring ComponentId = "Component_1";
   COLstring TestFile    = SDBlogFile(ComponentId, Now, TestDir);
   // 100 messages (25 of each type)
   SDBTcreateTestLogFile(TestFile);

   int StartId = 42;
   SDBmessageId Config(ComponentId, Now, StartId);
   SDBreader* pReader = SDBcreateQueueReader(Config, "", true, TestDir);

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   SDBreaderFetchMessage(pReader, &Loop, COLnewClosure2(&SDBTonManualMessage, pReader, &Loop, StartId));
   Loop.start();

   SDBdeleteReader(pReader);
   // Clean up
   COLstring RemoveError;
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
}

static void SDBTonJournalData(int Code, SDBcontent* pContent, SDBreader* pReader, SCKloop* pLoop) {
   COL_FUNCTION(SDBTonJournalData);
   static int PosMapCount = 0;
   static int MessageCount = 0;
   static int PositionCount = 0;
   if (Code == 0) {
      switch (pContent->Type) {
         case SDB_MARKER:     UNIT_ASSERT_MESSAGE(false, "Did you change ");
         case SDB_POSMAP:     PosMapCount++;    break;
         case SDB_POSITION:   PositionCount++;  break;
         case SDB_EOT:        UNIT_ASSERT_MESSAGE(false, "End-of-type is not a valid type");
         default:             MessageCount++;   break;
      }
      SDBcontentDestroy(pContent);
      SDBreaderFetch(pReader, pLoop, COLnewClosure2(&SDBTonJournalData, pReader, pLoop));
   } else if (SDB_END_OF_LOG == Code) {
      UNIT_ASSERT_EQUALS(1,     PosMapCount);
      UNIT_ASSERT_EQUALS(4*100, MessageCount);
      UNIT_ASSERT_EQUALS(4*24,  PositionCount);
      pLoop->shutdown();
   } else {
      UNIT_ASSERT_MESSAGE(false, "You broke the journal reader.");
   }
}

static void testJournal() {
   COL_FUNCTION(testJournal);
   COLstring TestFile = "reader.journal.test";
   if (FILfileExists(TestFile)) { FILremove(TestFile); }

   // 1 position map, 4 components, 100 (25 of each message type) messages and 24 (25-1) positions each
   SDBTcreateTestJournalFile(TestFile);

   SDBreader* pReader = SDBcreateJournalReader(TestFile, "");

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   SDBreaderFetch(pReader, &Loop, COLnewClosure2(&SDBTonJournalData, pReader, &Loop));
   Loop.start();

   SDBdeleteReader(pReader);
   // Clean up
   FILremoveWithThrow(TestFile);
}

static void SDBTstopLoop(SCKloop* pLoop) {
   COL_FUNCTION(SDBTstopLoop);
   pLoop->shutdown();
}

static void testDelete() {
   COL_FUNCTION(testDelete);
   COLstring TestFile = "reader.delete.test";
   if (FILfileExists(TestFile)) { FILremove(TestFile); }

   // 1 position map, 4 components, 100 (25 of each message type) messages and 24 (25-1) positions each
   SDBTcreateTestJournalFile(TestFile);

   SDBreader* pReader = SDBcreateJournalReader(TestFile, "");

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   COLauto<COLclosure2<int, SDBcontent*>> pCallback = COLnewClosure2(&SDBTonJournalData, pReader, &Loop);
   SDBreaderFetch(pReader, &Loop, pCallback);
   SDBdeleteReader(pReader, COLnewClosure0(&SDBTstopLoop, &Loop));
   Loop.start();

   // Clean up
   FILremoveWithThrow(TestFile);
}

static void SDBTsuccess(int Code, SDBmessage* pMessage, SCKloop* pLoop) {
   COL_FUNCTION(SDBTsuccess);
   UNIT_ASSERT_EQUALS(SDB_END_OF_LOG, Code);
   pLoop->shutdown();
}

// https://interfaceware.atlassian.net/browse/IX-2046?focusedCommentId=251887
static void testIX2046() {
   COL_FUNCTION(testIX2046);
   COLstring ComponentId = "Component_1";
   COLstring TestDir = "dir.IX-2046.log.test";
   COLstring RemoveError;
   if (FILfileExists(TestDir)) { FILremoveFullDirNewSafe(TestDir, &RemoveError); }
   SDBmessage Message(ComponentId, "This is message number 1000000");

   // simulate a corrupted log file with a partially written log message at EOF
   // note that the reader is trying to start reading from a message that doesn't (only partially) exist in the log
   COLstring FileBuffer;
   SDBTcreateEnvelopedTestMessage(&FileBuffer, &Message);
   COLstring TestFile = SDBlogFile(ComponentId, Message.Id.MessageId.DateTime, TestDir);
   SDBcheckLogDir(TestFile);
   FILwriteFile(TestFile, FileBuffer.substr(0, 4));

   SDBreader* pReader = SDBcreateQueueReader(Message.Id.MessageId, "", true, TestDir);

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   // the bug results in an infinite loop. using a timer to break the loop and end the test
   bool Success = true;
   SDBreaderFetchMessage(pReader, &Loop, COLnewClosure2(&SDBTsuccess, &Loop));
   Loop.start();
   UNIT_ASSERT(Success);
   // we should have a corruption region
   UNIT_ASSERT(pReader->Corruptions.size());
   // the region should include EOF
   const SDBcorruptRegion& Region = pReader->Corruptions.back();
   UNIT_ASSERT_EQUALS(FILfileSize(TestFile), Region.Position + Region.Size);

   SDBdeleteReader(pReader);
   // Clean up
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
}

// unit test for SDBreader.Position calculation bug
static void testIX2812() {
   COL_FUNCTION(testIX2812);
   COLauto<COLstring> pBuffer = new COLstring();
   int MsgSize = 10000000;
   SDBTcreateEnvelopedTestMessage(pBuffer, "Component_1", 1, MsgSize);
   int ExpectedSize = pBuffer->size();
   // set up reader state for the test
   SDBreader Reader;
   Reader.pFilter = COLnewClosure2(&SDBTblockAllContent);
   // for 10MB message, Reader.Position is incremented when the 1st 8MiB was loaded.
   // the 2nd load only loads the remaining 1611392 bytes, which is the amount that
   //  should be added to Reader.Position
   Reader.Position = MsgSize - (MsgSize % SDB_READER_BUF_LOAD_SIZE);
   SDBbufferLoaded(0, pBuffer, &Reader, NULL);
   UNIT_ASSERT_EQUALS(ExpectedSize, Reader.Position);
}

void testReader(UNITapp* pApp){
   COL_FUNCTION(testSimple);
   pApp->add("reader/unpack",     &testUnpack);
   pApp->add("reader/extract",    &testExtract);
   pApp->add("reader/extract/corrupt/1", &testExtractCorrupt1);
   pApp->add("reader/extract/corrupt/2", &testExtractCorrupt2);
   pApp->add("reader/extract/corrupt/3", &testExtractCorrupt3);
   pApp->add("reader/extract/corrupt/4", &testExtractCorrupt4);
   pApp->add("reader/simple",     &testSimple);
   pApp->add("reader/large",      &testLarge);
   pApp->add("reader/next/file",  &testNextFile);
   pApp->add("reader/log/power",  &testLogPower);
   pApp->add("reader/log",        &testLog);
   pApp->add("reader/queue",      &testQueue);
   pApp->add("reader/manual",     &testManual);
   pApp->add("reader/journal",    &testJournal);
   pApp->add("reader/delete",     &testDelete);
   pApp->add("reader/IX-2046",    &testIX2046);
   pApp->add("reader/IX-2812",    &testIX2812);
}
