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

#include <SDB/SDBdequeueSingle.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBmessageId.h>
#include <SDB/SDBreader.h> // for SDB_END_OF_LOG

#include <SCK/SCKloop.h>

#include <FIL/FILutils.h>

#include <COL/COLclosure.h>
#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static SDBmessageId SDBsetUPTestLogFile(const COLstring& FilePath) {
   COLarray<COLauto<SDBmessage>> MessageList;
   SDBTcreateTestMessageList(&MessageList, 100, "Component_1", 1, 1000, time(NULL));
   SDBTcreateTestLogFile(FilePath, MessageList);
   for (auto Message : MessageList) {
      if (SDB_MESSAGE != Message->Type) { continue; }
      return Message->Id.MessageId;
   }
   return SDBmessageId();
}

static void SDBonDequeuedMessage(int Code, SDBmessage* pMessage, SDBdequeueSingle* pDequeue, SCKloop* pLoop, COLlist<std::shared_ptr<SDBmessage>>* pCache) {
   COL_FUNCTION(SDBonDequeuedMessage);
   COL_VAR(Code);
   if (0 == Code) {
      COL_VAR(*pMessage);
      // delete pMessage;
      SDBremoveCommittedMessageSingle(pDequeue, pMessage->Id);
      SDBdequeueMessageSingle(pDequeue, pLoop);
   } else if (SDB_END_OF_LOG == Code) {
      pLoop->shutdown();
   } else {
      UNIT_ASSERT_MESSAGE(false, "You broke the queue reader.");
   }
}

static void testFileCache() {
   COL_FUNCTION(testFileCache);
   COLstring TestDir     = "dir.cache.file.dequeue.test";
   COLstring ComponentId = "Component_1";
   time_t    Date        = time(NULL);
   COLstring TestFile    = SDBlogFile(ComponentId, Date*1000, TestDir);
   // 100 messages (25 of each type)
   SDBmessageId Config = SDBsetUPTestLogFile(TestFile);

   // manually generate the last 4 SDB_MESSAGE messages to simulate SDBsystem cache
   COLlist<std::shared_ptr<SDBmessage>> Cache;
   Cache.push_back(std::shared_ptr<SDBmessage>(SDBTcreateTestMessage(ComponentId,  88, 1000, Date)));
   Cache.push_back(std::shared_ptr<SDBmessage>(SDBTcreateTestMessage(ComponentId,  92, 1000, Date)));
   Cache.push_back(std::shared_ptr<SDBmessage>(SDBTcreateTestMessage(ComponentId,  96, 1000, Date)));
   Cache.push_back(std::shared_ptr<SDBmessage>(SDBTcreateTestMessage(ComponentId, 100, 1000, Date)));

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   SDBdequeueSingle* pDequeue = SDBcreateDequeueSingle(Config, &Cache, "", TestDir);
   // SDBdequeueSingle should dequeue the first 21 SDB_MESSAGE messages from log file,
   // and the last 4 from cache (even if they are also in the log file)
   COLclosure2<int, SDBmessage*>* pCallback = COLnewClosure2(SDBonDequeuedMessage, pDequeue, &Loop, &Cache);
   SDBsetOnDequeuedSingle(pDequeue, pCallback);
   SDBdequeueMessageSingle(pDequeue, &Loop);
   Loop.start();

   delete pDequeue;
   // Clean up
   COLstring RemoveError;
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
}

static void testFile() {
   COL_FUNCTION(testFile);
   COLstring TestDir     = "dir.file.dequeue.test";
   COLstring ComponentId = "Component_1";
   time_t    Date        = time(NULL);
   COLstring TestFile    = SDBlogFile(ComponentId, Date*1000, TestDir);
   // 100 messages (25 of each type)
   SDBmessageId Config = SDBsetUPTestLogFile(TestFile);

   COLlist<std::shared_ptr<SDBmessage>> Cache; // empty

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   SDBdequeueSingle* pDequeue = SDBcreateDequeueSingle(Config, &Cache, "", TestDir);
   // SDBdequeueSingle should dequeue all 25 SDB_MESSAGE messages from log file
   COLclosure2<int, SDBmessage*>* pCallback = COLnewClosure2(SDBonDequeuedMessage, pDequeue, &Loop, &Cache);
   SDBsetOnDequeuedSingle(pDequeue, pCallback);
   SDBdequeueMessageSingle(pDequeue, &Loop);
   Loop.start();

   delete pDequeue;
   // Clean up
   COLstring RemoveError;
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
}

static void testCache() {
   COL_FUNCTION(testCache);
   COLstring TestDir     = "dir.cache.dequeue.test";
   COLstring ComponentId = "Component_1";
   time_t    Date        = time(NULL);
   COLstring TestFile    = SDBlogFile(ComponentId, Date*1000, TestDir);
   // 100 messages (25 of each type)
   SDBmessageId Config = SDBsetUPTestLogFile(TestFile);

   // manually generate the 4 more SDB_MESSAGE messages to simulate SDBsystem cache
   COLlist<std::shared_ptr<SDBmessage>> Cache;
   Cache.push_back(std::shared_ptr<SDBmessage>(SDBTcreateTestMessage(ComponentId, 104, 1000, Date)));
   Cache.push_back(std::shared_ptr<SDBmessage>(SDBTcreateTestMessage(ComponentId, 108, 1000, Date)));
   Cache.push_back(std::shared_ptr<SDBmessage>(SDBTcreateTestMessage(ComponentId, 112, 1000, Date)));
   Cache.push_back(std::shared_ptr<SDBmessage>(SDBTcreateTestMessage(ComponentId, 116, 1000, Date)));

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   SDBdequeueSingle* pDequeue = SDBcreateDequeueSingle(Config, &Cache, "", TestDir);
   // SDBdequeueSingle should dequeue all 25 SDB_MESSAGE messages from log file, and 4 from cache
   COLclosure2<int, SDBmessage*>* pCallback = COLnewClosure2(SDBonDequeuedMessage, pDequeue, &Loop, &Cache);
   SDBsetOnDequeuedSingle(pDequeue, pCallback);
   SDBdequeueMessageSingle(pDequeue, &Loop);
   Loop.start();

   delete pDequeue;
   // Clean up
   COLstring RemoveError;
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
}

static void SDBonDequeueError(int Code, SDBmessage* pMessage, SDBdequeueSingle* pDequeue, SCKloop* pLoop, COLlist<std::shared_ptr<SDBmessage>>* pCache) {
   COL_FUNCTION(SDBonDequeueError);
   UNIT_ASSERT_EQUALS(SDB_READER_BAD_CONFIG, Code);
   pLoop->shutdown();
}

static void testMissingLogFile() {
   COL_FUNCTION(testMissingLogFile);
   COLstring TestDir     = "dir.log.no.dequeue.test";
   COLstring ComponentId = "Component_1";
   time_t    Date        = time(NULL);
   SDBmessageId Config(ComponentId, Date*1000, 1);

   // simulating the start position Config points to a log file that doesn't exist
   // so no need to generate test log file
   COLlist<std::shared_ptr<SDBmessage>> Cache; // empty

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   SDBdequeueSingle* pDequeue = SDBcreateDequeueSingle(Config, &Cache, "", TestDir);
   // SDBdequeueSingle should report SDB_READER_BAD_CONFIG
   COLclosure2<int, SDBmessage*>* pCallback = COLnewClosure2(SDBonDequeueError, pDequeue, &Loop, &Cache);
   SDBsetOnDequeuedSingle(pDequeue, pCallback);
   SDBdequeueMessageSingle(pDequeue, &Loop);
   Loop.start();

   delete pDequeue;
   // Clean up
   COLstring RemoveError;
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
}

static void testBadStartPosition() {
   COL_FUNCTION(testBadStartPosition);
   COLstring TestDir     = "dir.start.bad.dequeue.test";
   COLstring ComponentId = "Component_1";
   time_t    Date        = time(NULL);
   COLstring TestFile    = SDBlogFile(ComponentId, Date*1000, TestDir);

   // simulate the start position pointing to a message that's not SDB_MESSAGE
   COLarray<COLauto<SDBmessage>> MessageList;
   SDBTcreateTestMessageList(&MessageList, 100, ComponentId, 1, 1000, Date);
   SDBTcreateTestLogFile(TestFile, MessageList);
   // Use a non-SDB_MESSAGE as the start position (which is required to be a SDB_MESSAGE)
   // Note the 1st message crated by SDBTcreateTestMessage is a SDB_INFO
   const SDBmessageId& Config = MessageList[0]->Id.MessageId;

   COLlist<std::shared_ptr<SDBmessage>> Cache; // empty

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   SDBdequeueSingle* pDequeue = SDBcreateDequeueSingle(Config, &Cache, "", TestDir);
   // SDBdequeueSingle should report SDB_READER_BAD_CONFIG
   COLclosure2<int, SDBmessage*>* pCallback = COLnewClosure2(SDBonDequeueError, pDequeue, &Loop, &Cache);
   SDBsetOnDequeuedSingle(pDequeue, pCallback);
   SDBdequeueMessageSingle(pDequeue, &Loop);
   Loop.start();

   delete pDequeue;
   // Clean up
   COLstring RemoveError;
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
}
// static void SDBonAgainMessage(int Code, SDBmessage* pMessage, SDBdequeueSingle* pDequeue, SCKloop* pLoop, COLlist<std::shared_ptr<SDBmessage>>* pCache, COLclosure2<int, SDBmessage*>** ppCallback) {
//    COL_FUNCTION(SDBonAgainMessage);
//    static int Count = 0;
//    static COLint64 PrevId = 0;
//    COL_VAR(Code);
//    if (0 == Code) {
//       COL_VAR(*pMessage);
//       // SDBdequeueMessageSingle() should return the same message unless we SDBremoveCommittedMessageSingle()
//       if (Count++ % 2) {
//          UNIT_ASSERT_EQUALS(PrevId, pMessage->MessageId);
//          SDBremoveCommittedMessageSingle(pDequeue, pMessage->MessageId);
//       } else {
//          UNIT_ASSERT(PrevId != pMessage->MessageId);
//          PrevId = pMessage->MessageId;
//       }
//       SDBdequeueMessageSingle(pDequeue, pLoop, *ppCallback);
//    } else if (SDB_END_OF_LOG == Code) {
//       // Cache should be empty when all SDB_MESSAGE messages had been dequeued
//       UNIT_ASSERT_EQUALS(0, pCache->size());
//       pLoop->shutdown();
//    } else {
//       UNIT_ASSERT_MESSAGE(false, "You broke the queue reader.");
//    }
// }

// static void testAgain() {
//    COL_FUNCTION(testAgain);
//    COLstring TestDir     = "dir.again.dequeue.test";
//    COLstring ComponentId = "Component_0";
//    time_t    Date        = time(NULL);
//    COLstring TestFile    = SDBlogFile(ComponentId, Date, TestDir);
//    // 100 messages (25 of each type)
//    SDBTcreateTestLogFile(TestFile);

//    // manually generate the last 4 SDB_MESSAGE messages to simulate SDBsystem cache
//    COLlist<std::shared_ptr<SDBmessage>> Cache;
//    Cache.push_back(SDBTcreateTestMessage(ComponentId,  88, 1000, Date));
//    Cache.push_back(SDBTcreateTestMessage(ComponentId,  92, 1000, Date));
//    Cache.push_back(SDBTcreateTestMessage(ComponentId,  96, 1000, Date));
//    Cache.push_back(SDBTcreateTestMessage(ComponentId, 100, 1000, Date));

//    COLthreadPool Pool;
//    Pool.start();
//    SCKloop Loop(&Pool);

//    SDBdequeueSingle* pDequeue = SDBcreateDequeueSingle(ComponentId, Date, 0, &Cache, TestDir);
//    // SDBdequeueSingle should dequeue the first 21 SDB_MESSAGE messages from log file,
//    // and the last 4 from cache (even if they are also in the log file)
//    COLclosure2<int, SDBmessage*>* pCallback;
//    pCallback = COLnewClosure2(SDBonAgainMessage, pDequeue, &Loop, &Cache, &pCallback);
//    SDBdequeueMessageSingle(pDequeue, &Loop, pCallback);
//    Loop.start();

//    delete pDequeue;
//    delete pCallback;
//    // Clean up
//    FILremoveFullDirNew(TestDir);
// }

void testDequeue(UNITapp* pApp){
   COL_FUNCTION(testDequeue);
   pApp->add("dequeue/file/cache", &testFileCache);
   pApp->add("dequeue/file",       &testFile);
   pApp->add("dequeue/cache",      &testCache);
   pApp->add("dequeue/log/absent", &testMissingLogFile);
   pApp->add("dequeue/bad/start",  &testBadStartPosition);
   // pApp->add("dequeue/again",      &testAgain);
}
