//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testExecute
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Mon  4 Apr 2022 11:30:30 EDT
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>

#include <errno.h>

#include <PRO/PROexecute.h>
#include <PRO/PROwhich.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <SCK/SCKloop.h>

#include <COL/COLclosure6.h>
#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring testOS() {
#ifdef _WIN32
   return "test.exe ";
#else
   return "./test ";
#endif
}

static void multiDone(int* pCount, SCKloop* pLoop, bool Success, bool* pTestSuccess, int Id) {
   COL_FUNCTION(multiDone);
   (*pCount)--;
   if(!Success) { *pTestSuccess = false; }
   if(*pCount == 0) {
      COL_TRC("Test complete.");
      pLoop->shutdown();
   }
}

static void testMulti(int Id, int* pCount, SCKloop* pLoop, bool* pTestSuccess) {
   COL_FUNCTION(testMulti);
   COLstring Error;
   COLstring Out, Err;
   const int ExitCode = PROexecuteString("", testOS() + "--test \"app/testSimple\" " + COLintToString(Id), &Out, &Err);
   COL_VAR3(ExitCode, Out, Err);
   const bool Success = Out.find("COMPLETE SUCCESS") != npos;
   if(!Success) { COL_ERR(Id << " failed."); }
   COL_TRC("Done " << Id);
   pLoop->post(COLnewClosure0(&multiDone, pCount, pLoop, Success, pTestSuccess, Id));
}

static void testExecuteThreaded(){
   COL_FUNCTION(testExecuteThreaded);
   COLthreadPool ThreadPool(1,400);
   SCKloop Loop(&ThreadPool);
   bool TestSuccess = true;
   ThreadPool.start();
   int Count = 5000;  // Change to 10000 for stress testing.
   for (int i=0; i < Count;i++){
      ThreadPool.scheduleTask(COLnewClosure0(testMulti, i, &Count, &Loop, &TestSuccess));
   }
   Loop.start();
   UNIT_ASSERT_EQUALS(true, TestSuccess);
}

static void testSingleLarge(){
   COL_FUNCTION(testSingleLarge);
   COLstring Out, Err;
   int ExitCode = PROexecuteString("", testOS() + "test 1000000", &Out, &Err, 10);
   COL_VAR3(ExitCode, Out, Err);
   COL_VAR2(Out.size(), Err.size());
   UNIT_ASSERT_EQUALS(1000000, Out.size());
   UNIT_ASSERT_EQUALS(1000000, Err.size());
}

static void testSimple() {
   COL_FUNCTION(testSimple);
   COLstring Out, Err;
   int ExitCode = PROexecuteString("", "git", &Out, &Err);
   COL_VAR(Out);
   UNIT_ASSERT(Out.size());
}

static void testDoesNotExist() {
   COL_FUNCTION(testDoesNotExist);
   COLstring Out, Err;
   int ExitCode = PROexecuteString("", "DoNotExist", &Out, &Err);
   COL_VAR2(Out, ExitCode);
    // UNIT_ASSERT(ExitCode == ENOEXEC);  // exec implementations are not consistent with respect to error codes here
}

static void testBadFile() {
   COL_FUNCTION(testBadFile);
   COLstring Out, Err;
   int ExitCode = PROexecuteString("", "main.cpp", &Out, &Err);
   COL_VAR3(Out, Err, ExitCode);
   UNIT_ASSERT(Err == "main.cpp is not an executable file");
   // TODO - go through and make deterministic error messages for the errors we test for.
   UNIT_ASSERT(ExitCode !=0);
   //UNIT_ASSERT(ExitCode == ENOEXEC); // exec implementations are not consistent with respect to error codes here
}

static void testDirWithSpaces() {
   COL_FUNCTION(testDirWithSpaces);
   COLstring DirWithSpaces = FILpathAppend("Pro with spaces", "with spaces");
   FILmakeFullDir(DirWithSpaces, (FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX));
   UNIT_ASSERT(FILfileExists(DirWithSpaces));
   COLstring Out, Err;
   COLstring Command;
#ifdef _WIN32
   Command = "dir";
#else
   Command = "ls";
#endif
   int ExitCode = PROexecuteString(DirWithSpaces, Command, &Out, &Err);
   if(FILfileExists(DirWithSpaces)) {
      COLstring RemoveError;
      FILremoveFullDirNewSafe(DirWithSpaces, &RemoveError);
   }
}

static void testBadDir(){
   COL_FUNCTION(testBadDir);
   COLstring Out, Err;
   int ExitCode = PROexecuteString("Bad Directory", testOS() + "timeout 5000", &Out, &Err);
   COL_VAR3(ExitCode, Out, Err);
   UNIT_ASSERT_EQUALS("Bad Directory not found.", Err);
   UNIT_ASSERT(ExitCode == PRO_BAD_DIR);
}

// Timeout should be from when the last output was received from the child
static void testTimeout(){
   COL_FUNCTION(testTimeout);
   COLstring Out, Err;
   int ExitCode = PROexecuteString("", testOS() + "timeout 500", &Out, &Err,5);
   COL_VAR3(ExitCode, Out, Err);
   UNIT_ASSERT_EQUALS(0, ExitCode);
   UNIT_ASSERT(Err.is_null());
   UNIT_ASSERT(Out.size() > 0);
}

static void testBadTimeout(){
   COL_FUNCTION(testTimeout);
   COLstring Out, Err;
   int ExitCode = PROexecuteString("", testOS() + "timeout 5000", &Out, &Err, 2);
   COL_VAR3(ExitCode, Out, Err);
   UNIT_ASSERT(ExitCode != 0);
   UNIT_ASSERT_EQUALS(ExitCode, PRO_TIMED_OUT);  //TODO enable this
   UNIT_ASSERT(Err.is_null());
}

void testExecute(UNITapp* pApp) {
   // pApp->add("app/execute/threads",   &testExecuteThreaded);
   // pApp->add("app/testLarge",         &testSingleLarge);
   pApp->add("app/testSimple",        &testSimple);
   pApp->add("app/testDoesNotExist",  &testDoesNotExist);
   pApp->add("app/testDirWithSpaces", &testDirWithSpaces);
   pApp->add("app/testBadFile",       &testBadFile);
   pApp->add("app/testBadDir",        &testBadDir);
   pApp->add("app/timeoutNo",         &testTimeout);
   pApp->add("app/timeoutYes",        &testBadTimeout);
}
