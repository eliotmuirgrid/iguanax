// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testDir
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Mon 13 Feb 2023 10:01:00 EST
// ---------------------------------------------------------------------------
#include "testDir.h"
#include <UNIT/UNITapp.h>

#include <SDB/SDBdir.h>
#include <SDB/SDBnumber.h>



#include <SDB/SDBfile.h>
#include <SDB/SDBmessage.h>
#include <FIL/FILfile.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <FIL/FILosDefs.h>


#include <COL/COLdateTime.h>
#include <COL/COLmath.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

//#include <algorithm>
//#include <chrono>

/*
static void testFindNextMessagePosition() {
   COL_FUNCTION(testFindNextMessagePosition);
   COLstring Buffer = "Hello World!";
   int Expected = Buffer.size();
   SDBmessage Message;
   Message.Data = "Lorem ipsum dolor sit amet, consectetur adipiscing elit";
   COL_VAR(Buffer);
   SDBwriteMessage(Message, &Buffer);
   COL_VAR(Buffer);
   UNIT_ASSERT_EQUALS(Expected, SDBgetNextMessagePosition(Buffer));
}

static void testFindNextmessageId() {
   COL_FUNCTION(testFindNextmessageId);
   COLstring Buffer = "3.14159265358979";
   int Expected = 3543564;
   SDBmessage Message;
   Message.MessageId = Expected;
   COL_VAR(Buffer);
   SDBwriteMessage(Message, &Buffer);
   COL_DBG_DUMP("Buffer = ", Buffer.data(), Buffer.size());
   COLuint64 Actual = 0;
   SDBgetNextMessageId(Buffer, 0, &Actual);
   UNIT_ASSERT_EQUALS(Expected, Actual);
}

static COLstring getMessageTemplate(int MaxSize){
   COL_FUNCTION(getMessageTemplate);
   COLstring Template;
   Template.setCapacity(MaxSize);
   Template.setSize(MaxSize);
   for (int i = 1000; i <= MaxSize; i += 1000) {
      COLstring Marker = "> " + COLintToString(i / 1000) + " KB <-|";
      memcpy(Template.get_buffer() + i - Marker.size(), Marker.c_str(), Marker.size());
   }
   // COL_DBG_DUMP("Template = ", Template.c_str(), Template.size());
   return Template;
}


// Concise way of setting up link test case. The bottom-up approach
// must be called after all messages have been generated
static void linkMessageTopDown(COLlist<SDBmessage>::iterator* pMessage, int StepsAhead){
   COL_FUNCTION(linkMessageTopDown);
   // Set up the message at i
   // linkMessage(i, 5);
   // linkMessage(i, 25);
   // linkMessage(i, 45);
   // linkMessage(i, 55);
}

// must be called while messages are being generated
static void linkMessageBottomUp(int MessageId, const COLstring& ComponentId, SDBmessage* pMessage) {
   COL_FUNCTION(linkMessageBottomUp);
   if (COLmath::randomNumber(3) == 0 && MessageId > 1) {
      COL_TRC("Set related message");
      SDBmessage& Message = *pMessage;
      Message.RelatedMessageId = COLmath::randomNumber(MessageId-1);
      Message.RelatedComponentId = ComponentId;
   }
}



static void generateMessages(int Count, const COLstring& ComponentId, COLlist<SDBmessage>* pOut) {
   COL_FUNCTION(generateMessages);
   static int MIN_MSG_SIZE =     40;   //  40 B
   static int MAX_MSG_SIZE = 300000;   // 300 KB
   static int MessageId    =      0;   // limits Count to 2^31 (~2 billon)

   COLlist<SDBmessage>& MessageList = *pOut;
   const COLstring& Template = getMessageTemplate(MAX_MSG_SIZE);
   for (int i = 0; i < Count; ++i) {
      SDBmessage Message;
      int MessageSize = std::max((COLuint32)MIN_MSG_SIZE, COLmath::randomNumber(Count));
      populateTestMessage(MessageId++, ComponentId, COLstring(Template.data(), MessageSize), &Message);
      MessageList.push_back(Message);
   }
}

static void generateLog(const COLstring& ComponentId, const COLstring& Date, int Count) {
   COL_FUNCTION(generateLog);
   COLlist<SDBmessage> MessageList;
   generateMessages(Count, ComponentId, &MessageList);

   COLstring Path = SDBlogOut(ComponentId, Date, 0);
   SDBcheckLogDir(Path);
   FILfile File(Path, FILfile::Rewrite);

   // stats
   int Sum = 0, RelatedCnt = 0;

   // one log file per day
   // TODO: multi-file per day, up to 1k messages each
   COLstring Buffer;
   Buffer.setCapacity(16777216);    // 16 MiB
   for (auto i = MessageList.cbegin(); i != MessageList.cend(); i++) {
      Sum += SDBmessageSize(*i);
      RelatedCnt += i->RelatedComponentId.is_null() ? 0 : 1;
      SDBwriteMessage(*i, &Buffer);
      if (Buffer.size() > 16477216) { // flush if buffer > 16.4 MB
         File.write(Buffer);
         Buffer.clear();
      }
   }
   COL_VAR2(Sum/Count, RelatedCnt);
   File.write(Buffer);
   File.flush();
   File.close();
}

static int findMessageContiguous(const COLstring& ComponentId, const COLstring& Date, COLuint64 TargetId, int NumMsg) {
   COL_FUNCTION(findMessageContiguous);
   auto T0 = std::chrono::steady_clock::now();

   COLstring Path = SDBlogOut(ComponentId, Date, 0);
   int FD = SDBopenLogFile(Path.c_str());
   UNIT_ASSERT(FD > 0);

   COLstring Buffer;
   Buffer.setCapacity(524288);  // TODO: kinda cheating here, max message size is 300 KB

   // In this test, all messages are in today's log file(s) so it's easy to binary search.
   // What happens when messages are spread out across multiple days? Binary search becomes
   // much more difficult. How do we know which log file the message is in? Need an index.
   int L = 0, R = FILfileSize(Path);
   COLuint64 MessageId = COL_UINT_64_MAX;
   while (TargetId != MessageId) {
      COL_VAR2(L, R);
      int M = (R-L)/2 + L;
      SDBseekLogFile(FD, M);
      Buffer.clear();
      SDBloadBuffer(FD, &Buffer, 400000); // TODO: also cheating here cuz max msg size <= 300 KB
      COLASSERT(0 == SDBgetNextMessageId(Buffer, 0, &MessageId));
      COL_VAR2(MessageId, TargetId);
      (TargetId < MessageId) ? R = M : L = M;
   }
   COL_TRC("Found target message in buffer");
   int Start = SDBgetNextMessagePosition(Buffer);
   COL_VAR(Start);
   SDBmessage Message;
   UNIT_ASSERT_EQUALS(0, SDBreadMessage(Buffer, Start, &Message));
   COL_VAR(Message);
   UNIT_ASSERT_EQUALS(TargetId, Message.MessageId);

   SDBcloseLogFile(FD);

   auto T1 = std::chrono::steady_clock::now();
   auto Td = std::chrono::duration_cast<std::chrono::microseconds>(T1 - T0).count();
   // COLcout << Td << "us" << newline;
   return Td;
}

static int findMessageSegmented(const COLstring& ComponentId, const COLstring& Date, COLuint64 TargetId, int NumMsg) {
   COL_FUNCTION(findMessageSegmented);
   return 0;
}

static void testQuery() {
   COL_FUNCTION(testQuery);
   COLstring ComponentId = "test";
   COLstring Date = COLdateTime::currentDate().format("%Y%m%d");
   COLmath::resetRandomGenerator();
   static int MSG_COUNT = 10000;
   // generate log
   generateLog(ComponentId, Date, MSG_COUNT);
   static int TEST_COUNT = 1000;
   int TimeContiguous = 0, TimeSegmented = 0;
   for (int i = 0; i < TEST_COUNT; i++) {
      // generate a random message id for seach
      COLuint64 TargetId = COLmath::randomNumber(MSG_COUNT);
      COL_VAR(TargetId);
      // search using 'one log file per day' approach
      TimeContiguous += findMessageContiguous(ComponentId, Date, TargetId, MSG_COUNT);
      // search using 'multi log files per day` approach
      // TimeSegmented  += findMessageSegmented(ComponentId, Date, TargetId, MSG_COUNT);
      // break;
   }
   COLcout << "Contiguous average search time = " << TimeContiguous / TEST_COUNT << " µs" << newline;
   COLcout << "Segmented  average search time = " << TimeSegmented  / TEST_COUNT << " µs" << newline;
}
*/
void testFind(UNITapp* pApp){
   COL_FUNCTION(testSimple);
   //pApp->add("find/testReadWrite",      &testReadWrite);
   //pApp->add("find/testNextMessagePos", &testFindNextMessagePosition);
   //pApp->add("find/testNextMessageId",  &testFindNextmessageId);
   //pApp->add("find/testQuery",          &testQuery);
}
