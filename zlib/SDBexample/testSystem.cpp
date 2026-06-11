// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testSystem
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Tue 7 Mar 2023 14:12:06 EST
// ---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>

#include <SDBT/SDBTmessage.h>

#include <SDB/SDBsystem.h>
#include <SDB/SDBjournalWrite.h>
#include <SDB/SDBlogWrite.h>
#include <SDB/SDBflush.h>
#include <SDB/SDBdateTime.h>
#include <SDB/SDBinit.h>
#include <SDB/SDBmessage.h>

#include <SCK/SCKloop.h>

#include <FIL/FILutils.h>
#include <FIL/FILosDefs.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLauto.h>
#include <COL/COLarray.h>
#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <COL/COLdateTime.h>
#include <COL/COLutils.h>

static void testSerializeLog() {
   COL_FUNCTION(testSerializeLog);
   COLstring TestDir = "dir.serialization.log.test";
   COLstring RemoveError;
   if (FILfileExists(TestDir)) { FILremoveFullDirNewSafe(TestDir, &RemoveError); }
   COLstring ComponentId = "Component_0";
   COLuint64 Now = SDBtime();
   COLstring Buffer, LogFile;
   COLlist<std::shared_ptr<SDBmessage>> List;

   // 2 small messages with similar timestamps, should all be written to the same log file
   List.push_back(std::shared_ptr<SDBmessage>(new SDBmessage(ComponentId, "", Now)));
   List.push_back(std::shared_ptr<SDBmessage>(new SDBmessage(ComponentId, "", Now + 1)));
   LogFile = SDBserializeLogMessages(&List, NULL, false, &Buffer, TestDir);
   UNIT_ASSERT_EQUALS(SDBlogFile(ComponentId, Now, TestDir), LogFile);
   UNIT_ASSERT_EQUALS(0, List.size());
   Buffer.clear();

   // 1 small message followed by a large message with the same timestamp, should both be written to the same log file
   COLstring Data(134217728, '\0');
   List.push_back(std::shared_ptr<SDBmessage>(new SDBmessage(ComponentId, "  ", Now, 1)));
   List.push_back(std::shared_ptr<SDBmessage>(new SDBmessage(ComponentId, Data, Now, 2)));
   LogFile = SDBserializeLogMessages(&List, NULL, false, &Buffer, TestDir);
   UNIT_ASSERT_EQUALS(0, List.size());
   Buffer.clear();

   // 1 small message followed by a large message with different timestamp, should trigger large file roll over
   List.push_back(std::shared_ptr<SDBmessage>(new SDBmessage(ComponentId, "  ", Now)));
   List.push_back(std::shared_ptr<SDBmessage>(new SDBmessage(ComponentId, Data, Now + 1000)));  // Now + 1s
   LogFile = SDBserializeLogMessages(&List, NULL, false, &Buffer, TestDir);
   UNIT_ASSERT_EQUALS(1, List.size());
   UNIT_ASSERT_EQUALS(1, List.front()->Id.MessageId.Index);
   Buffer.clear();
   List.clear();

   // 1 small message now, another 18 hours later, should trigger epoch roll over
   List.push_back(std::shared_ptr<SDBmessage>(new SDBmessage(ComponentId, "", Now)));
   List.push_back(std::shared_ptr<SDBmessage>(new SDBmessage(ComponentId, "", Now + 0xFFFF*1000)));   // Now + 0xFFFF seconds
   LogFile = SDBserializeLogMessages(&List, NULL, false, &Buffer, TestDir);
   UNIT_ASSERT_EQUALS(1, List.size());
   UNIT_ASSERT_EQUALS(1, List.front()->Id.MessageId.Index);
}

static void testSerializePositions() {
   COL_FUNCTION(testSerializePositions);
   COLstring TestDir = "dir.serialization.position.test";
   COLstring RemoveError;
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
   if (FILfileExists(TestDir)) { FILremoveFullDirNewSafe(TestDir, &RemoveError); }
   COLstring ComponentId = "Component_0";
   COLuint64 Now = SDBtime();
   COLstring Buffer, LogFile;
   COLlist<std::shared_ptr<SDBmessage>>* pList = NULL;
   COLhashmap<COLstring, SDBid> Positions;

   // case 1 : flush position and message to log where a epoch rollover is triggered on the first message
   // create the last log file so that rollover is triggered (time(NULL) - 0xFFFF)
   COLstring LastFile = SDBlogFile(ComponentId, Now - 0xFFFF * 1000, TestDir);
   COL_VAR(LastFile);
   FILmakeFullDir(FILparentDir(LastFile), FIL_USER_RWX);
   FILwriteFile(LastFile, "");
   // serialize position map with 1 message
   pList = new COLlist<std::shared_ptr<SDBmessage>>();
   pList->push_back(std::shared_ptr<SDBmessage>(new SDBmessage(ComponentId, "", Now)));

   SDBlogTaskData Data1;
   Data1.pMessages = pList;
   Data1.pPositions = new COLhashmap<COLstring, SDBid>(Positions);
   Data1.FlushPosition = true;
   {
      SDBlogHandle Handle1;
      SDBflushLogMessages(Handle1, Data1, "", TestDir);
      // Handle 1 goes out of scope and closes the file
   }

   // message and position should be written to the rolled over log file
   LogFile = SDBlogFile(ComponentId, Now, TestDir);
   UNIT_ASSERT(FILfileSize(LogFile));
   // the last log file should remain empty
   UNIT_ASSERT_EQUALS(0, FILfileSize(LastFile));
   Buffer.clear();
   FILremove(LogFile);     // clean up for case 2

   // case 2 : flush just positions
   Positions["Compnent_1"].MessageId = SDBmessageId(ComponentId, Now, 1);
   pList = new COLlist<std::shared_ptr<SDBmessage>>();

   SDBlogTaskData Data2;
   Data2.pMessages = pList;
   Data2.pPositions = new COLhashmap<COLstring, SDBid>(Positions);
   Data2.FlushPosition = true;
   {
      SDBlogHandle Handle2;
      SDBflushLogMessages(Handle2, Data2, "", TestDir);
      // Handle 2 goes out of scope and closes the file
   }

   // positions should be written to the last log file, so it shouldn't be empty any more
   UNIT_ASSERT(FILfileSize(LastFile));

   // clean up
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
}

struct TestData {
   COLarray<SDBmessage*> Data;
   int Index = 0;
};

static void generateTestData(int ComponentCount, int MessageCount, COLmap<COLstring, TestData>* pOut) {
   COL_FUNCTION(generateTestData);
   COL_TRC("Generating logs for " << SDBdate());
   for (int i = 0; i < ComponentCount; ++i) {
      COLstring ComponentId = "Component_" + COLintToString(i);
      SDBTcreateTestMessageList(&(*pOut)[ComponentId].Data, MessageCount/2, ComponentId);
   }
   time_t Tomorrow = time(NULL)+SDB_DAY;
   COL_TRC("Generating logs for " << SDBdate(Tomorrow));
   for (int i = 0; i < ComponentCount; ++i) {
      COLstring ComponentId = "Component_" + COLintToString(i);
      SDBTcreateTestMessageList(&(*pOut)[ComponentId].Data, MessageCount/2, ComponentId, MessageCount/2+1, 1000, Tomorrow);
   }
}

static void flushComplete(SCKloop* pLoop, SDBsystem* pSystem, COLstring LogRootDir) {
   COL_FUNCTION(flushComplete);
   // prep log flush list
   COLlist<COLstring> LogList;
   for (auto i = pSystem->ComponentMap.begin(); i != pSystem->ComponentMap.end(); i++){
      LogList.push_back(i.key());
   }
   // flush logs and shut down loop after
   COLclosure0* pDone = COLnewClosure0(pLoop, &SCKloop::shutdown);
   SDBwriteLogs(pSystem, pDone, LogList);
}

static void messageEnqueued(SDBid EnqueuedId) {
   COL_FUNCTION(messageEnqueued);
   COL_VAR(EnqueuedId);
   // UNIT_ASSERT_EQUALS(1000, EnqueuedId.MessageId);
   UNIT_ASSERT(EnqueuedId.MessageId.ComponentId.find("Component_") != npos);
}

static void testSimple(){
   COL_FUNCTION(testSimple);
   COLstring TestDir = "dir.simple.test";
   COLstring RemoveError;
   if (FILfileExists(TestDir)) { FILremoveFullDirNewSafe(TestDir, &RemoveError); }

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   COLmap<COLstring, TestData> ComponentData;
   generateTestData(10, 1000, &ComponentData);

   CFGmap Map;
   SDBsystem System(&Loop, Map);
   // register components
   for (auto i = ComponentData.cbegin(); i != ComponentData.cend(); i++) {
      SDBregisterComponent(&System, i.key());
      SDBsetMessageQueued(&System, i.key(), COLnewClosure1(&messageEnqueued));
   }
   // push 1000 messages
   for (int i = 0; i < 1000; ++i) {
      // for each component in round-robin fashion
      for (auto j = ComponentData.begin(); j != ComponentData.end(); j++) {
         SDBlogMessage(&System, std::shared_ptr<SDBmessage>(j.value().Data[j.value().Index++]));
      }
   }
   SDBstartJournalThread(&Loop, &System.JournalTaskQueue, NULL, "", TestDir);
   SDBstartLogThread(&System, NULL, "", TestDir);
   // flush journal
   SDBwriteJournal(&System, COLnewClosure0(&flushComplete, &Loop, &System, TestDir));
   Loop.start();
   // clean up
   SDBstopJournalThread(&System.JournalTaskQueue, NULL);
   SDBstopLogThread(&System, NULL);
   COLsleep(100);    // wait for Log and Journal thread to stop completely before task queues get deleted
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
}

static void testLogMessage() {
   COL_FUNCTION(testLogMessage);
   CFGmap Map;
   SCKloop* pLoop = NULL;
   SDBsystem System(pLoop, Map);
   SDBregisterComponent(&System, "Component_1");
   COLuint64 Time = SDBtime();
   // log 3 messages with the same timestamp and 1 message with a different timestamp
   // first 3 should have Index 1, 2, and 3, while the 4th has Index 1
   std::shared_ptr<SDBmessage> Msg1(new SDBmessage("Component_1", "", Time));
   std::shared_ptr<SDBmessage> Msg2(new SDBmessage("Component_1", "", Time));
   std::shared_ptr<SDBmessage> Msg3(new SDBmessage("Component_1", "", Time));
   std::shared_ptr<SDBmessage> Msg4(new SDBmessage("Component_1", "", Time+1));
   SDBmessageId Id1 = SDBlogMessage(&System, Msg1).MessageId;
   SDBmessageId Id2 = SDBlogMessage(&System, Msg2).MessageId;
   SDBmessageId Id3 = SDBlogMessage(&System, Msg3).MessageId;
   SDBmessageId Id4 = SDBlogMessage(&System, Msg4).MessageId;
   UNIT_ASSERT_EQUALS(1, Id1.Index);
   UNIT_ASSERT_EQUALS(2, Id2.Index);
   UNIT_ASSERT_EQUALS(3, Id3.Index);
   UNIT_ASSERT_EQUALS(1, Id4.Index);
}

static void SDBTdone(SCKloop* pLoop) {
   COL_FUNCTION(SDBTdone);
   pLoop->shutdown();
}

static void SDBTstop(SCKloop* pLoop, SDBsystem* pSystem) {
   COL_FUNCTION(SDBTstop);
   SDBstopSystem(pSystem, COLnewClosure0(&SDBTdone, pLoop));
}

// memcheck server will yell if pComponentList is leaked in SDBlogRecovered() during first startup
static void testFirstStartupMemLeak() {
   COL_FUNCTION(testFirstStartupMemLeak);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   CFGmap Configs;
   SDBsystem System(&Loop, Configs);
   auto pPositionMap = new COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>();
   SDBlogRecovered(1, pPositionMap, &System, COLnewClosure0(&SDBTstop, &Loop, &System));
   Loop.start();
}

// memcheck server will yell if pPositionMap is leaked in SDBlogRecovered() if we need to get positions from log files
static void testLogRecoverMemLeak() {
   COL_FUNCTION(testLogRecoverMemLeak);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   CFGmap Configs;
   Configs.ConfigMap["Component_A"];
   Configs.ConfigMap["Component_B"];
   SDBsystem System(&Loop, Configs);
   auto pPositionMap = new COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>();
   SDBlogRecovered(1, pPositionMap, &System, COLnewClosure0(&SDBTstop, &Loop, &System));
   Loop.start();
}

static void SDBTenqueueMessage(SDBsystem* pSystem) {
   COL_FUNCTION(SDBTenqueueMessage);
   SDBmessage* pMessage = new SDBmessage();
   pMessage->Data = "kasdhfadsfasdfasl;jkldfjks;ladfjkslkajdslkfjiofghrvg902y389h9y&*G78GT(B JD978G^&r^F)";
   pMessage->Type = SDB_MESSAGE;
   pMessage->Id.MessageId = SDBmessageId("Enqueuer");
   SDBlogMessage(pSystem, std::shared_ptr<SDBmessage>(pMessage));
   SDBflushSystemNow(pSystem);
}

static void SDBTmessageEnqueued(SDBid EnqueuedId, int Count, SDBsystem* pSystem, SCKloop* pLoop) {
   COL_FUNCTION(SDBTmessageEnqueued);
   static int Counter = 1;
   COL_TRC("Enqueued message " << Counter);
   if (++Counter == Count) {
      SDBstopSystem(pSystem, COLnewClosure0(pLoop, &SCKloop::shutdown));
   } else {
      // COLcout << Counter << newline;
      COL_TRC("Enqueue message " << Counter);
      SDBTenqueueMessage(pSystem);
   }
}

static void testEnqueue(){
   COL_FUNCTION(testEnqueue);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   CFGmap Map;
   SDBsystem System(&Loop, Map);
   SDBregisterComponent(&System, "Enqueuer");
   int Count = 1000;
   SDBsetMessageQueued(&System, "Enqueuer", COLnewClosure1(&SDBTmessageEnqueued, Count, &System, &Loop));
   SDBstartSystem(&System, COLnewClosure0(&SDBTenqueueMessage, &System));
   Loop.start();
}

static void SDBTlog1(SCKloop* pLoop);
static void SDBTwrite1(SCKloop* pLoop) {
   COL_FUNCTION(SDBTwrite1);
   FILappendFile("model1.log", "kasdhfadsfasdfasl;jkldfjks;ladfjkslkajdslkfjiofghrvg902y389h9y&*G78GT(B JD978G^&r^F)");
   pLoop->post(COLnewClosure0(&SDBTlog1, pLoop));
}

static void SDBTlog1(SCKloop* pLoop) {
   COL_FUNCTION(SDBTlog1);
   static int Counter = 1;
   COL_TRC("Enqueued message " << Counter);
   if (++Counter == 1000) {
      pLoop->shutdown();
   } else {
      COL_TRC("Enqueue message " << Counter);
      pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBTwrite1, pLoop));
   }
}

static void testModel1() {
   COL_FUNCTION(testModel1);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   Loop.addTimer(COLnewClosure0(&SDBTlog1, &Loop));
   Loop.start();

   // clean up
   FILremove("model1.log");
}

int FD = 0;
static void SDBTlog2(SCKloop* pLoop);
static void SDBTwrite2(SCKloop* pLoop) {
   COL_FUNCTION(SDBTwrite2);
   if (!FD) {
      FD = FILopen("model3.log", O_RDWR | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
   }
   static COLstring s = "kasdhfadsfasdfasl;jkldfjks;ladfjkslkajdslkfjiofghrvg902y389h9y&*G78GT(B JD978G^&r^F)";
   write(FD, s.c_str(), s.size());
   pLoop->post(COLnewClosure0(&SDBTlog2, pLoop));
}

static void SDBTlog2(SCKloop* pLoop) {
   COL_FUNCTION(SDBTlog2);
   static int Counter = 1;
   COL_TRC("Enqueued message " << Counter);
   if (++Counter == 10000) {
      pLoop->shutdown();
      close(FD);
   } else {
      COL_TRC("Enqueue message " << Counter);
      pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBTwrite2, pLoop));
   }
}

static void testModel2() {
   COL_FUNCTION(testModel2);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   Loop.addTimer(COLnewClosure0(&SDBTlog2, &Loop));
   Loop.start();

   // clean up
   FILremove("model2.log");
}

static void SDBTlog3(COLthreadQueue<COLstring>* pQueue, SCKloop* pLoop) {
   COL_FUNCTION(SDBTlog3);
   static int Counter = 1;
   COL_TRC("Enqueued message " << Counter);
   if (++Counter == 10000) {
      pQueue->put("TERMINATE");
      pLoop->shutdown();
   } else {
      COL_TRC("Enqueue message " << Counter);
      static COLstring s = "kasdhfadsfasdfasl;jkldfjks;ladfjkslkajdslkfjiofghrvg902y389h9y&*G78GT(B JD978G^&r^F)";
      pQueue->put(s);
   }
}

static void SDBTwriter(COLthreadQueue<COLstring>* pQueue, SCKloop* pLoop) {
   COL_FUNCTION(SDBTwriter);
   int FD = 0;
   while (true) {
      COLstring Msg = pQueue->take();
      if ("TERMINATE" == Msg) {
         break;
      }
      if (!FD) { FD = FILopen("model3.log", O_RDWR | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE); }
      ::write(FD, Msg.c_str(), Msg.size());
      pLoop->post(COLnewClosure0(&SDBTlog3, pQueue, pLoop));
   }
   close(FD);
}

static void testModel3() {
   COL_FUNCTION(testModel3);
   COLthreadQueue<COLstring> JobQueue;
   COLthreadPool Pool;
   SCKloop Loop(&Pool);
   Pool.start();
   Pool.scheduleTask(COLnewClosure0(&SDBTwriter, &JobQueue, &Loop));
   Loop.addTimer(COLnewClosure0(&SDBTlog3, &JobQueue, &Loop));
   Loop.start();

   // clean up
   FILremove("model3.log");
}

static void SDBTwriteAndFlushLogs(COLlist<COLstring>* pComponentList, SDBsystem* pSystem, SCKloop* pLoop) {
   COL_FUNCTION(SDBTwriteAndFlushLogs);
   static int Count = 0;
   if (Count++ > 3) { pLoop->shutdown(); return; }
   for (auto i = pComponentList->begin(); i != pComponentList->end(); i++) {
      const COLstring& ComponentId = *i;
      for (int i = 0; i < 100; ++i) {
         SDBlogMessage(pSystem, std::shared_ptr<SDBmessage>(new SDBmessage(ComponentId, "Hello")));
      }
   }
   // don't care about journal, testing logs
   pSystem->JournalBuffer.clear();
   COLclosure0* pCallback = COLnewClosure0(&SDBTwriteAndFlushLogs, pComponentList, pSystem, pLoop);
   SDBwriteLogs(pSystem, pCallback, *pComponentList);
}

static void testLog(){
   COL_FUNCTION(testLog);
   COLstring TestDir = "dir.log.test";
   COLstring RemoveError;
   if (FILfileExists(TestDir)) { FILremoveFullDirNewSafe(TestDir, &RemoveError); }

   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   CFGmap Map;
   SDBsystem System(&Loop, Map);

   // register components
   COLlist<COLstring> ComponentList;
   for (int i = 0; i < 30; ++i) {
      COLstring ComponentId = "Component_" + COLintToString(i);
      SDBregisterComponent(&System, ComponentId);
      ComponentList.push_back(ComponentId);
   }

   SDBstartLogThread(&System, NULL, "", TestDir);
   Loop.addTimer(COLnewClosure0(&SDBTwriteAndFlushLogs, &ComponentList, &System, &Loop));
   Loop.start();

   // clean up
   SDBstopLogThread(&System, NULL);
   COLsleep(100);    // wait for Log thread to stop completely before task queue get deleted
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
}

void testSystem(UNITapp* pApp){
   COL_FUNCTION(testSystem);
   pApp->add("system/serialize/log", &testSerializeLog);
   pApp->add("system/serialize/position", &testSerializePositions);
   pApp->add("system/simple", &testSimple);
   pApp->add("system/logmessage", &testLogMessage);
   pApp->add("system/memory/first/startup", &testFirstStartupMemLeak);
   pApp->add("system/memory/log/recover", &testLogRecoverMemLeak);
   // Tests for Windows performance issues. Run individually. See https://interfaceware.atlassian.net/browse/IX-2995
   // pApp->add("system/enqueue", &testEnqueue);
   // pApp->add("system/model1", &testModel1);  // performance test. on-demand worker thread open, write, close
   // pApp->add("system/model2", &testModel2);  // performance test. on-demand worker thread write to files kept open
   // pApp->add("system/model3", &testModel3);  // performance test. persistent worker thread that processes write tasks from COLthreadQueue
   pApp->add("system/log", &testLog);
}
