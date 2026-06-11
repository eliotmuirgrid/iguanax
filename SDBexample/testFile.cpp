// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testFile
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Mon 13 Feb 2023 10:01:00 EST
// ---------------------------------------------------------------------------
#include "testFile.h"
#include <UNIT/UNITapp.h>

#include <SDB/SDBfile.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBnumber.h>
#include <SDB/SDBmessage.h>

#include <SDBT/SDBTfile.h>
#include <SDBT/SDBTparse.h>
#include <SDBT/SDBTsearch.h>

#include <SCK/SCKloop.h>

#include <FIL/FILutils.h>

#include <COL/COLdateTime.h>
#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testOpen(){
   COL_FUNCTION(testOpen);
   COLstring TestFile = "open.tests";
   if (FILfileExists(TestFile)) { FILremove(TestFile); }

   COLstring Data = "0123456789";
   FILwriteFile(TestFile, Data);

   // open file in truncate mode
   int FD = SDBreplaceLogFile(TestFile);
   SDBwriteLogFile(FD, TestFile.data(), TestFile.size());
   // "0123456789" -> "open.tests"
   SDBcloseLogFile(FD);
   COLstring Actual;
   FILreadFile(TestFile, &Actual);
   UNIT_ASSERT_EQUALS(TestFile, Actual);

   // open file in append mode
   FD = SDBopenLogFile(TestFile);
   SDBwriteLogFile(FD, Data.data(), Data.size());
   // "open.tests" -> "open.tests0123456789"
   SDBcloseLogFile(FD);
   FILreadFile(TestFile, &Actual);
   UNIT_ASSERT_EQUALS(TestFile+Data, Actual);

   // open file in read/write mode
   FD = SDBopenLogFile(TestFile, false);
   SDBseekLogFile(FD, 10);
   SDBwriteLogFile(FD, TestFile.data(), TestFile.size());
   // "open.tests0123456789" -> "open.testsopen.tests"
   SDBcloseLogFile(FD);
   FILreadFile(TestFile, &Actual);
   UNIT_ASSERT_EQUALS(TestFile+TestFile, Actual);

   FILremoveWithThrow(TestFile);
}

static void testSeek(){
   COL_FUNCTION(testSeek);
   COLstring TestFile = "seek.tests";
   if (FILfileExists(TestFile)) { FILremove(TestFile); }

   FILwriteFile(TestFile, "0123456789");
   // open in append mode
   int FD = SDBopenLogFile(TestFile);
   UNIT_ASSERT_EQUALS(10, SDBseekLogFileEOF(FD));
   UNIT_ASSERT_EQUALS(10, SDBseekLogFile(FD, 0, true));
   UNIT_ASSERT_EQUALS(6, SDBseekLogFile(FD, -4, true));
   UNIT_ASSERT_EQUALS(9, SDBseekLogFile(FD, 3, true));
   UNIT_ASSERT_EQUALS(0, SDBseekLogFile(FD, 0));
   UNIT_ASSERT_EQUALS(9, SDBseekLogFile(FD, -1));
   SDBcloseLogFile(FD);
   FILremoveWithThrow(TestFile);
}

static void readCallback(COLstring Chunk, SCKloop* pLoop){
   //COL_FUNCTION(readCallback);
   //COL_VAR(Chunk.size());
   if (Chunk.size() == 0){
      COL_TRC("End of file.");
      pLoop->shutdown();
   }
}

static void testSimple(){
   COL_FUNCTION(testSimple);
   SDBTmakeTestMessageSet("test.log", 1, 1000);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   int FD = SDBopenLogFile("test.log");
   COL_TRC("Starting to stream file.");
   SDBstreamFile(&Loop, FD, COLnewClosure1(&readCallback, &Loop));
   Loop.start();
   COL_TRC("Done streaming file.");
   // Clean up
   SDBcloseLogFile(FD);
   FILremoveWithThrow("test.log");
}

static void parseCallBack(COLstring Chunk, SCKloop* pLoop, COLstring* pBuffer, int* pCount){
   //COL_FUNCTION(parseCallBack);
   if (Chunk.size() == 0){
      COL_TRC("End of file.");
      pLoop->shutdown();
   }
   *pBuffer += Chunk;
   COLlist<SDBcontent*> List;
   SDBTparse(pBuffer, pCount);
}

static void testParsing(){
   COL_FUNCTION(testParsing);
   SDBTmakeTestMessageSet("test2.log", 1, 1000);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   int Count = 0;
   time_t Start = time(NULL);
   int FD = SDBopenLogFile("test2.log");
   COL_TRC("Starting to stream file.");
   COLstring Buffer;
   SDBstreamFile(&Loop, FD, COLnewClosure1(&parseCallBack, &Loop, &Buffer, &Count));
   Loop.start();
   COL_TRC("Done streaming file.");
   COL_VAR(Count);
   time_t End = time(NULL);
   COL_VAR(End - Start);
   double MessagesPerSecond = Count / (End - Start);
   COL_VAR(MessagesPerSecond);
   // Clean up
   SDBcloseLogFile(FD);
   FILremoveWithThrow("test2.log");
}

// Need to be able to search
// Criteria - with a string
//          - using an ID of a message (for context of what happened to a message
// Can use HTTP streaming with chunked transfer


static void testSearchId(){
   COL_FUNCTION(testSearchId);
   SDBTmakeTestMessageFile("test3.log", 1, 1000);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);

   int FD = SDBopenLogFile("test3.log");
   COL_TRC("Starting to stream file.");

   SDBTseachMessageId(543, FD, &Loop);

   Loop.start();
   COL_TRC("Done streaming file.");

   // Clean up
   SDBcloseLogFile(FD);
   FILremoveWithThrow("test3.log");
}

void testFile(UNITapp* pApp){
   COL_FUNCTION(testSimple);
   pApp->add("file/open", &testOpen);
   pApp->add("file/seek", &testSeek);
   pApp->add("file/simple", &testSimple);
   pApp->add("file/parsing", &testParsing);
   // pApp->add("file/search", &testSearchId);
}
