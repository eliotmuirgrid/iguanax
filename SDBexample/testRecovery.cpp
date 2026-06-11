// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testRecovery
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Friday 24 March 2023 - 02:37PM
// ---------------------------------------------------------------------------
#include "testRecovery.h"

#include <UNIT/UNITapp.h>

#include <SDBT/SDBTreader.h>
#include <SDBT/SDBTrecovery.h>
#include <SDBT/SDBTenvelope.h>
#include <SDBT/SDBTmessage.h>
#include <SDBT/SDBTpositionMap.h>

#include <SDB/SDBcryptoFile.h>
#include <SDB/SDBenvelope.h>
#include <SDB/SDBfile.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBposition.h>
#include <SDB/SDBpositionMap.h>
#include <SDB/SDBrecovery.h>
#include <SDB/SDBsystem.h>

#include <SCK/SCKloop.h>

#include <FIL/FILosDefs.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <COL/COLthreadPool.h>
#include <COL/COLmath.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


static int SDBTcreateTestLogFileForTruncate(const COLstring& FilePath, int TruncateAfterId, SDBmessageId* pId, int Messages = 100) {
   COL_FUNCTION(SDBTcreateTestLogFileForTruncate);
   COLarray<COLauto<SDBmessage>> MessageList;
   SDBTcreateTestMessageList(&MessageList, Messages, "Component_1", 1, 1000, time(NULL));
   int FileSize = 0;
   COLstring Buffer;
   Buffer.setCapacity(Messages*(1000+100));
   for (int i = 0; i < MessageList.size(); ++i) {
      SDBTcreateEnvelopedTestMessage(&Buffer, MessageList[i]);
      if (i == TruncateAfterId - 1) { FileSize = Buffer.size(); }
      if (i == TruncateAfterId) { *pId = MessageList[i]->Id.MessageId; }
   }
   SDBcheckLogDir(FilePath);
   FILwriteFile(FilePath, Buffer);
   return FileSize;
}

static void testFind() {
   COL_FUNCTION(testFind);
   COLstring TestFile = "content.seek.test";
   if (FILfileExists(TestFile)) { FILremove(TestFile); }
   // 100 messages (25 of each type)
   SDBmessageId NextId;
   int Expected = SDBTcreateTestLogFileForTruncate(TestFile, 42, &NextId);
   COL_VAR(FILfileSize(TestFile));
   int FD = SDBopenLogFile(TestFile);
   int Actual = SDBgetFilePosition(FD, NextId);
   SDBcloseLogFile(FD);
   UNIT_ASSERT_EQUALS(Expected, Actual);
   // Clean up
   FILremoveWithThrow(TestFile);
}

static void testFindLarge() {
   COL_FUNCTION(testFindLarge);
   COLstring TestFile = "content.seek.large.test";
   if (FILfileExists(TestFile)) { FILremove(TestFile); }
   // 1000 messages (250 of each type)
   SDBmessageId NextId;
   int Expected = SDBTcreateTestLogFileForTruncate(TestFile, 924, &NextId, 1000);
   COL_VAR(FILfileSize(TestFile));
   int FD = SDBopenLogFile(TestFile);
   // similar test that exercises the buffer loading logic
   int Actual = SDBgetFilePosition(FD, NextId);
   SDBcloseLogFile(FD);
   UNIT_ASSERT_EQUALS(Expected, Actual);
   // Clean up
   FILremoveWithThrow(TestFile);
}

static void testFindEncrypted() {
   COL_FUNCTION(testFindEncrypted);
   COLstring EncKey = "1234567890ABCDEF!@#$%^&*()ABCDEF";
   // |                                   log file                                         |
   // |  encryption packet 1  |  encryption packet 2  | corruption |  encryption packet 4  |
   // | message 1 | message 2 | message 3 | message 4 | corruption |       message 6       |
   time_t Now = time(NULL);
   COLstring ComponentId = "Component_1";
   COLauto<SDBmessage> pMsg1 = SDBTcreateTestMessage(ComponentId, 1, 100, Now);
   COLauto<SDBmessage> pMsg2 = SDBTcreateTestMessage(ComponentId, 2, 100, Now);
   COLauto<SDBmessage> pMsg3 = SDBTcreateTestMessage(ComponentId, 3, 100, Now);
   COLauto<SDBmessage> pMsg4 = SDBTcreateTestMessage(ComponentId, 4, 100, Now);
   // pMsg5 is corrected
   COLauto<SDBmessage> pMsg6 = SDBTcreateTestMessage(ComponentId, 6, 100, Now);

   COLstring PlainMsg1, PlainMsg2, PlainMsg3, PlainMsg4, Corruption, PlainMsg6;
   SDBTcreateEnvelopedTestMessage(&PlainMsg1, pMsg1);
   SDBTcreateEnvelopedTestMessage(&PlainMsg2, pMsg2);
   SDBTcreateEnvelopedTestMessage(&PlainMsg3, pMsg3);
   SDBTcreateEnvelopedTestMessage(&PlainMsg4, pMsg4);
   Corruption = "Lorem ipsum odor amet, consectetuer adipiscing elit.";
   SDBTcreateEnvelopedTestMessage(&PlainMsg6, pMsg6);

   COLstring EncPkt1, EncPkt2, EncPkt4;
   SDBencryptAndSerialize(PlainMsg1 + PlainMsg2, EncKey, &EncPkt1);
   SDBencryptAndSerialize(PlainMsg3 + PlainMsg4, EncKey, &EncPkt2);
   SDBencryptAndSerialize(PlainMsg6,             EncKey, &EncPkt4);

   COLstring EncryptedLogContent(EncPkt1 + EncPkt2 + Corruption + EncPkt4);

   int Consumed = 0;
   COLstring Reencrypt;
   UNIT_ASSERT(SDBfindMessageIdEncrypted(EncryptedLogContent, EncKey, pMsg1->Id.MessageId, Consumed, &Reencrypt));
   UNIT_ASSERT(Reencrypt.is_null());   // pMsg1 is at beginning of enc packet 1
   UNIT_ASSERT_EQUALS(0, Consumed);    // enc packet 1 is the first enc packet

   UNIT_ASSERT(SDBfindMessageIdEncrypted(EncryptedLogContent, EncKey, pMsg2->Id.MessageId, Consumed, &Reencrypt));
   UNIT_ASSERT_EQUALS(PlainMsg1, Reencrypt);    // pMsg2 is the 2nd msg in enc packet 1, need to re-encrypt msg 1 to preserve it
   UNIT_ASSERT_EQUALS(0, Consumed);             // enc packet 1 is the first enc packet
   Reencrypt.clear();

   UNIT_ASSERT(SDBfindMessageIdEncrypted(EncryptedLogContent, EncKey, pMsg3->Id.MessageId, Consumed, &Reencrypt));
   UNIT_ASSERT(Reencrypt.is_null());   // pMsg2 is at beginning of enc packet 2
   UNIT_ASSERT_EQUALS(EncPkt1.size(), Consumed);   // the 1st enc packet should have been consumed
   Reencrypt.clear();

   UNIT_ASSERT(SDBfindMessageIdEncrypted(EncryptedLogContent, EncKey, pMsg4->Id.MessageId, Consumed, &Reencrypt));
   UNIT_ASSERT_EQUALS(PlainMsg3, Reencrypt);    // pMsg4 is the 2nd msg in enc packet 2, need to re-encrypt msg 3 to preserve it
   UNIT_ASSERT_EQUALS(EncPkt1.size(), Consumed);   // the 1st enc packet should have been consumed
   Reencrypt.clear();

   UNIT_ASSERT(SDBfindMessageIdEncrypted(EncryptedLogContent, EncKey, pMsg6->Id.MessageId, Consumed, &Reencrypt));
   UNIT_ASSERT(Reencrypt.is_null());    // pMsg6 is the 1st msg in enc packet 4
   UNIT_ASSERT_EQUALS(EncPkt1.size() + EncPkt2.size() + Corruption.size(), Consumed);   // the 1st enc packet should have been consumed
}

// static void testFindEdge() {
//    COL_FUNCTION(testFindEdge);
//    COLstring TestFile = "content.seek.edge.test";
//    // 100 messages (25 of each type)
//    SDBmessageId NextId;
//    int Expected = SDBTcreateTestLogFileForTruncate(TestFile, 100, &NextId);
//    COL_VAR(FILfileSize(TestFile));
//    int FD = SDBopenLogFile(TestFile);
//    // seek to the beginning of message 101, should be at EOF since we only have 100 messages in log file
//    int Actual = SDBgetFilePosition(FD, NextId);
//    SDBcloseLogFile(FD);
//    UNIT_ASSERT_EQUALS(Expected, Actual);
//    // Clean up
//    FILremoveWithThrow(TestFile);
// }

// static void SDBTonRecoveredFromNoCrash(int Code, COLhashmap<COLstring, SDBposition>* pPositionMap, SCKloop* pLoop, COLmap<COLstring, int> ExpectedLogSizes, COLstring LogDir) {
//    COL_FUNCTION(SDBTonRecoveredFromNoCrash);
//    UNIT_ASSERT_EQUALS(0, Code);
//    UNIT_ASSERT_EQUALS(4, pPositionMap->size());
//    for (auto i = pPositionMap->cbegin(); i != pPositionMap->cend(); i++) {
//       UNIT_ASSERT_EQUALS(1, i.value().MessageId);
//    }
//    COLstring LogFile = SDBlog(ExpectedLogSizes.begin().key(), time(NULL), true, LogDir);
//    UNIT_ASSERT_EQUALS(false, FILfileExists(FILparentDir(LogFile)));
//    delete pPositionMap;
//    pLoop->shutdown();
// }

// static void testMapOnly() {
//    COL_FUNCTION(testMapOnly);
//    COLstring TestDir = "dir.crash.no.journal.test";

//    COLthreadPool Pool;
//    Pool.start();
//    SCKloop Loop(&Pool);

//    // 1 position map with queue position 1
//    COLmap<COLstring, int> ExpectedLogSizes = SDBTcreateTestJournalFileForNoCrash(TestDir);

//    COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBmessageId>>*>* pCallback = COLnewClosure2(&SDBTonRecoveredFromNoCrash, &Loop, ExpectedLogSizes, TestDir);
//    SDBrecoverJournal(SDBjournalFile(TestDir), &Loop, pCallback, TestDir);

//    Loop.start();
//    // clean up
//    FILremoveFullDirNew(TestDir);
// }

static void SDBTonRecoveredFromEmptyFile(int Code, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap, SCKloop* pLoop, COLstring LogFile, int ExpectedSize) {
   COL_FUNCTION(SDBTonRecoveredFromEmptyFile);
   UNIT_ASSERT_EQUALS(0,  Code);
   UNIT_ASSERT_EQUALS(1,  pPositionMap->size());
   UNIT_ASSERT_EQUALS(96, pPositionMap->cbegin().value().cbegin().value().MessageId.Index);
   UNIT_ASSERT(FILfileExists(LogFile));
   delete pPositionMap;
   pLoop->shutdown();
}

static void testEmptyFile() {
   COL_FUNCTION(testEmptyFile);
   COLstring TestDir = "dir.file.log.empty.recover.test";
   COLstring RemoveError;
   if (FILfileExists(TestDir)) { FILremoveFullDirNewSafe(TestDir, &RemoveError); }
   COLstring ComponentId = "Component_1";
   time_t Now = time(NULL);

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   COLmap<COLstring, COLarray<COLauto<SDBmessage>>> LogData;
   SDBTcreateTestMessageList(&LogData[ComponentId], 100, ComponentId, 1, 1000, Now);
   int ExpectedSize = SDBTcalculateLogFileSize(LogData.cbegin().value());

   COLstring LogFile = SDBlogFile(ComponentId, Now*1000, TestDir);
   COL_VAR(LogFile);
   SDBcheckLogDir(LogFile);
   FILwriteFile(LogFile, "");

   COLstring JournalFile = SDBjournalFile(TestDir);
   SDBcheckLogDir(JournalFile);
   SDBTcreateTestJournalFile(JournalFile, LogData);

   COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pCallback = COLnewClosure2(&SDBTonRecoveredFromEmptyFile, &Loop, LogFile, ExpectedSize);
   SDBrecoverJournal(SDBjournalFile(TestDir), "", &Loop, pCallback, TestDir);

   Loop.start();
   // clean up
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
}

static void SDBTonRecoveredFromNoFile(int Code, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap, SCKloop* pLoop, COLstring LogFile, int ExpectedSize) {
   COL_FUNCTION(SDBTonRecoveredFromNoFile);
   UNIT_ASSERT_EQUALS(0,  Code);
   UNIT_ASSERT_EQUALS(1,  pPositionMap->size());
   UNIT_ASSERT_EQUALS(96, pPositionMap->cbegin().value().cbegin().value().MessageId.Index);
   UNIT_ASSERT(FILfileExists(LogFile));
   delete pPositionMap;
   pLoop->shutdown();
}

static void testNoFile() {
   COL_FUNCTION(testNoFile);
   COLstring TestDir = "dir.file.log.no.recover.test";
   COLstring RemoveError;
   if (FILfileExists(TestDir)) { FILremoveFullDirNewSafe(TestDir, &RemoveError); }
   COLstring ComponentId = "Component_1";
   time_t Now = time(NULL);

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   COLmap<COLstring, COLarray<COLauto<SDBmessage>>> LogData;
   SDBTcreateTestMessageList(&LogData[ComponentId], 100, ComponentId, 1, 1000, Now);
   int ExpectedSize = SDBTcalculateLogFileSize(LogData.cbegin().value());

   COLstring LogFile = SDBlogFile(ComponentId, Now*1000, TestDir);
   COL_VAR(LogFile);

   COLstring JournalFile = SDBjournalFile(TestDir);
   SDBcheckLogDir(JournalFile);
   SDBTcreateTestJournalFile(JournalFile, LogData);

   COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pCallback = COLnewClosure2(&SDBTonRecoveredFromNoFile, &Loop, LogFile, ExpectedSize);
   SDBrecoverJournal(SDBjournalFile(TestDir), "", &Loop, pCallback, TestDir);

   Loop.start();
   // clean up
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
}

static void SDBTonRecoveredFromPartialFile(int Code, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap, SCKloop* pLoop, COLstring LogFile, int ExpectedSize) {
   COL_FUNCTION(SDBTonRecoveredFromPartialFile);
   UNIT_ASSERT_EQUALS(0,  Code);
   UNIT_ASSERT_EQUALS(1,  pPositionMap->size());
   UNIT_ASSERT_EQUALS(96, pPositionMap->cbegin().value().cbegin().value().MessageId.Index);
   UNIT_ASSERT(FILfileExists(LogFile));
   delete pPositionMap;
   pLoop->shutdown();
}

static void testPartialFile() {
   COL_FUNCTION(testPartialFile);
   COLstring TestDir = "dir.file.log.partial.recover.test";
   COLstring RemoveError;
   if (FILfileExists(TestDir)) { FILremoveFullDirNewSafe(TestDir, &RemoveError); }
   COLstring ComponentId = "Component_1";
   time_t Now = time(NULL);

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   COLmap<COLstring, COLarray<COLauto<SDBmessage>>> LogData;
   SDBTcreateTestMessageList(&LogData[ComponentId], 100, ComponentId, 1, 1000, Now);
   int ExpectedSize = SDBTcalculateLogFileSize(LogData.cbegin().value());

   COLstring LogFile = SDBlogFile(ComponentId, Now*1000, TestDir);
   COL_VAR(LogFile);
   SDBcheckLogDir(LogFile);
   COLstring Buffer;
   SDBTcreateTestLogData(&Buffer, LogData.cbegin().value());
   UNIT_ASSERT_EQUALS(ExpectedSize, Buffer.size());
   // write anywhere between the first 25% and 75% of the logs to file
   COLmath::resetRandomGenerator();
   int Size = COLmath::randomNumber(Buffer.size()/2) + Buffer.size()/4;
   FILwriteFile(LogFile, Buffer.substr(0, Size));
   // remove the first 20 messages before creating the journal file
   for (int i = 0; i < 20; ++i) {
      LogData.begin().value().remove(0);
   }
   COLstring JournalFile = SDBjournalFile(TestDir);
   SDBcheckLogDir(JournalFile);
   SDBTcreateTestJournalFile(JournalFile, LogData);

   COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pCallback = COLnewClosure2(&SDBTonRecoveredFromPartialFile, &Loop, LogFile, ExpectedSize);
   COLASSERT(FILfileExists(LogFile));
   SDBrecoverJournal(SDBjournalFile(TestDir), "", &Loop, pCallback, TestDir);

   Loop.start();
   // clean up
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
}

static void SDBTonRecoveredFromRolloverFile(int Code, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap, SCKloop* pLoop, COLstring LogFile, int ExpectedSize) {
   COL_FUNCTION(SDBTonRecoveredFromRolloverFile);
   UNIT_ASSERT_EQUALS(0,  Code);
   UNIT_ASSERT_EQUALS(1,  pPositionMap->size());
   UNIT_ASSERT_EQUALS(96, pPositionMap->cbegin().value().cbegin().value().MessageId.Index);
   UNIT_ASSERT(FILfileExists(LogFile));
   delete pPositionMap;
   pLoop->shutdown();
}

static void testRolloverFile() {
   COL_FUNCTION(testRolloverFile);
   COLstring TestDir = "dir.file.log.rollover.recover.test";
   COLstring RemoveError;
   if (FILfileExists(TestDir)) { FILremoveFullDirNewSafe(TestDir, &RemoveError); }
   COLstring ComponentId = "Component_1";
   time_t Now = time(NULL);

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   COLmap<COLstring, COLarray<COLauto<SDBmessage>>> LogData;
   SDBTcreateTestMessageList(&LogData[ComponentId], 100, ComponentId, 2, 1000, Now);
   int ExpectedSize = SDBTcalculateLogFileSize(LogData.cbegin().value());

   // create a log file that is large enough to trigger a rollover
   COLstring LargeFile = SDBlogFile(ComponentId, (Now-1)*1000, TestDir);
   SDBTcreateTestLogFileLarge(LargeFile, Now-1, 1);

   COLstring JournalFile = SDBjournalFile(TestDir);
   SDBcheckLogDir(JournalFile);
   SDBTcreateTestJournalFile(JournalFile, LogData);

   COLstring LogFile = SDBlogFile(ComponentId, Now*1000, TestDir);
   COL_VAR(LogFile);
   COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pCallback = COLnewClosure2(&SDBTonRecoveredFromRolloverFile, &Loop, LogFile, ExpectedSize);
   SDBrecoverJournal(SDBjournalFile(TestDir), "", &Loop, pCallback, TestDir);

   Loop.start();
   // clean up
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
}

static void testReadId() {
   COL_FUNCTION(testReadId);
   SDBmessage Msg("message_test", "data");
   const SDBmessageId& Expected = Msg.Id.MessageId;
   COLstring Buffer;
   Buffer.setCapacity(SDBmessageSize(&Msg));
   int Written = SDBwriteMessage(&Msg, Buffer.get_buffer(), Buffer.capacity());
   Buffer.setSize(Written);

   // SDBsystem depends on this function to initialize SDBcomponent.NextMessageId
   // if SDBmessage serialization changed, make sure SDBsystem init still functions properly
   SDBmessageId Actual = SDBreadMessageId(Buffer.data(), Buffer.size());
   UNIT_ASSERT_EQUALS(Expected, Actual);
}

static void SDBTonRecoveredFromCorruptedJournal(int Code, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap, SCKloop* pLoop, SDBpositionMap ExpectedMap) {
   COL_FUNCTION(SDBTonRecoveredFromCorruptedJournal);
   UNIT_ASSERT_EQUALS(0,  Code);
   SDBpositionMap ActualMap(*pPositionMap);
   COL_VAR2(ExpectedMap, ActualMap);
   UNIT_ASSERT_EQUALS(ExpectedMap, ActualMap);
   delete pPositionMap;
   pLoop->shutdown();
}

static void testCorruptJournal() {
   COL_FUNCTION(testCorruptJournal);
   COLstring TestDir = "dir.file.journal.corrupt.recover.test";
   COLstring RemoveError;
   if (FILfileExists(TestDir)) { FILremoveFullDirNewSafe(TestDir, &RemoveError); }
   COLstring ComponentId = "Component_1";
   time_t Now = time(NULL);

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   COLstring JournalFile = SDBjournalFile(TestDir);
   SDBcheckLogDir(JournalFile);
   COLmap<COLstring, COLarray<COLauto<SDBmessage>>> LogData;
   SDBTcreateTestMessageList(&LogData[ComponentId], 100, ComponentId, 2, 1000, Now);
   SDBTcreateTestJournalFile(JournalFile, LogData);
   // corrupt journal file by truncating it
   SDBtruncateFile(JournalFile, 1024);

   // SDBTcreateTestJournalFile() uses SDBTcreateTestPositionMap() to create a position map
   SDBpositionMap ExpectedMap = SDBTcreateTestPositionMap(LogData.size(), 1, Now);
   COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pCallback = COLnewClosure2(&SDBTonRecoveredFromCorruptedJournal, &Loop, ExpectedMap);
   SDBrecoverJournal(SDBjournalFile(TestDir), "", &Loop, pCallback, TestDir);

   Loop.start();
   // clean up
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
}

// static void SDBTonRecoveredFromLogs(int Code, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap, SCKloop* pLoop, SDBpositionMap ExpectedMap) {
//    COL_FUNCTION(SDBTonRecoveredFromLogs);
//    UNIT_ASSERT_EQUALS(0,  Code);
//    SDBpositionMap ActualMap(*pPositionMap);
//    COL_VAR2(ExpectedMap, ActualMap);
//    UNIT_ASSERT_EQUALS(ExpectedMap, ActualMap);
//    delete pPositionMap;
//    pLoop->shutdown();
// }

// static void testNoJournal() {
//    COL_FUNCTION(testNoJournal);
//    COLstring TestDir = "dir.file.journal.no.recover.test";
//    COLstring RemoveError;
//    if (FILfileExists(TestDir)) { FILremoveFullDirNewSafe(TestDir, &RemoveError); }
//    COLstring ComponentId = "Component_1";
//    COLstring SourceOne = "Source1";
//    COLstring SourceTwo = "Source2";
//    time_t Now = time(NULL);

//    COLthreadPool Pool;
//    Pool.start();
//    SCKloop Loop(&Pool);

//    SDBid SDBidOne, SDBidTwo;
//    SDBidOne.MessageId = SDBmessageId(SourceOne);
//    SDBidTwo.MessageId = SDBmessageId(SourceTwo);

//    // create log files with positions
//    SDBpositionMap PosMapOne, PosMapTwo;
//    PosMapOne.PositionMap[SourceOne][ComponentId] = SDBidOne;
//    PosMapTwo.PositionMap[SourceTwo][ComponentId] = SDBidTwo;
//    COLstring BufOne, BufTwo;
//    SDBTcreateEnvelopedTestPositionMap(&BufOne, PosMapOne);
//    SDBTcreateEnvelopedTestPositionMap(&BufTwo, PosMapTwo);
//    COLstring LogOne = SDBlogFile(SourceOne, Now * 1000, TestDir);
//    COLstring LogTwo = SDBlogFile(SourceTwo, Now * 1000, TestDir);
//    SDBcheckLogDir(LogOne);
//    SDBcheckLogDir(LogTwo);
//    FILwriteFile(LogOne, BufOne);
//    FILwriteFile(LogTwo, BufTwo);

//    COLlist<COLstring>* pComponents = new COLlist<COLstring>();
//    pComponents->push_back(SourceOne);
//    pComponents->push_back(SourceTwo);
//    // we expect a pos map that is the union of map 1 and 2
//    SDBpositionMap ExpectedMap(PosMapOne);
//    ExpectedMap.PositionMap[SourceTwo] = PosMapTwo.PositionMap[SourceTwo];
//    COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pCallback = COLnewClosure2(&SDBTonRecoveredFromLogs, &Loop, ExpectedMap);
//    SDBrecoverPositionsFromLogs(pComponents, &Loop, "", pCallback, TestDir);

//    Loop.start();
//    // clean up
//    FILremoveFullDirNewSafe(TestDir, &RemoveError);
// }

// static void SDBTonNothing(int Code, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap, SCKloop* pLoop) {
//    COL_FUNCTION(SDBTonNothing);
//    UNIT_ASSERT_EQUALS(0,  Code);
//    delete pPositionMap;
//    pLoop->shutdown();
// }

// // memcheck server will yell if pComponents passed to SDBrecoverPositionsFromLogs() is leaked
// static void testNoJournalLog() {
//    COL_FUNCTION(testNoJournalLog);
//    COLstring TestDir = "dir.file.log.journal.no.recover.test";
//    COLstring RemoveError;
//    if (FILfileExists(TestDir)) { FILremoveFullDirNewSafe(TestDir, &RemoveError); }
//    COLthreadPool Pool;
//    Pool.start();
//    SCKloop Loop(&Pool);

//    COLlist<COLstring>* pComponents = new COLlist<COLstring>();
//    pComponents->push_back("Component_1");
//    pComponents->push_back("Component_2");

//    COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pCallback = COLnewClosure2(&SDBTonNothing, &Loop);
//    SDBrecoverPositionsFromLogs(pComponents, &Loop, "", pCallback, TestDir);

//    Loop.start();
// }


void testRecovery(UNITapp* pApp) {
   COL_FUNCTION(testRecovery);
   pApp->add("recover/find",           &testFind);
   pApp->add("recover/find/large",     &testFindLarge);
   pApp->add("recover/find/encrypted", &testFindEncrypted);
   // pApp->add("recover/find/edge",      &testFindEdge);
   // pApp->add("recover/normal",         &testMapOnly);
   pApp->add("recover/empty/file",     &testEmptyFile);
   pApp->add("recover/no/file",        &testNoFile);
   pApp->add("recover/partial/file",   &testPartialFile);
   pApp->add("recover/rollover/file",  &testRolloverFile);
   pApp->add("recover/read/id",        &testReadId);
   pApp->add("recover/corrupt/journal",&testCorruptJournal);
   // TODO replace tests with component init tests
   // pApp->add("recover/no/journal",     &testNoJournal);
   // pApp->add("recover/no/journal/log/mem", &testNoJournalLog);
}
