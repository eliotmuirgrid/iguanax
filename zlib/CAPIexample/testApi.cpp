//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testApi
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Mon  4 Apr 2022 11:30:30 EDT
//---------------------------------------------------------------------------
#include "testApi.h"

#include <CAPI/CAPIlog.h>
#include <COL/COLlog.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;


static thread_local COLstring t_MemStdOut;
static thread_local COLstring t_MemStdErr;

void logToMemory(const COLstring& Message) {
   t_MemStdOut.append(Message);
}
static void redirectStderrToMemory(const COLstring& Message) {
   t_MemStdErr.append(Message);
}


static void testLogApi1() {
   t_MemStdOut.clear();    // must clear cuz it's shared by tests running on the same thread
   COLstring Answer;
   for (int i = 0; i < 100000; i++) {
      Answer.append("testLogApi1");
      CAPIlogStdout("testLogApi1");
   }
   UNIT_ASSERT_EQUALS(Answer, t_MemStdOut);
}

static void testLogApi2() {
   t_MemStdOut.clear();    // must clear cuz it's shared by tests running on the same thread
   COLstring Answer;
   for (int i = 0; i < 100000; i++) {
      Answer.append("testLogApi2");
      CAPIlogStdout("testLogApi2");
   }
   UNIT_ASSERT_EQUALS(Answer, t_MemStdOut);
}

static void testLogApi3() {
   t_MemStdOut.clear();    // must clear cuz it's shared by tests running on the same thread
   COLstring Answer;
   for (int i = 0; i < 100000; i++) {
      Answer.append("testLogApi3");
      CAPIlogStdout("testLogApi3");
   }
   UNIT_ASSERT_EQUALS(Answer, t_MemStdOut);
}

static void testLogApi4() {
   t_MemStdOut.clear();    // must clear cuz it's shared by tests running on the same thread
   COLstring Answer;
   for (int i = 0; i < 100000; i++) {
      Answer.append("testLogApi4");
      CAPIlogStdout("testLogApi4");
   }
   UNIT_ASSERT_EQUALS(Answer, t_MemStdOut);
}

static void testLogApi5() {
   t_MemStdOut.clear();    // must clear cuz it's shared by tests running on the same thread
   COLstring Answer;
   for (int i = 0; i < 100000; i++) {
      Answer.append("testLogApi5");
      CAPIlogStdout("testLogApi5");
   }
   UNIT_ASSERT_EQUALS(Answer, t_MemStdOut);
}

static void testLogTerminal() {
   CAPIlogStdout("Standard output");
   CAPIlogStderr("Standard error");
}

void testApi(UNITapp* pApp){
   // 5 identical tests to test the multi-thread behaviour
   pApp->add("CAPI/logTest1", &testLogApi1);
   pApp->add("CAPI/logTest2", &testLogApi2);
   pApp->add("CAPI/logTest3", &testLogApi3);
   pApp->add("CAPI/logTest4", &testLogApi4);
   pApp->add("CAPI/logTest5", &testLogApi5);
   // print to terminal
   pApp->add("CAPI/logTerminal", &testLogTerminal);
}