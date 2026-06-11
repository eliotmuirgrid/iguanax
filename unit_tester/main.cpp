//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: main.cpp
//
// Description:
//
// Tool to orchestrate builds, tests, and uploads of Iguana
//
// Author: Nicolas Lehman
// Date:   Thu 14/11/2019
//---------------------------------------------------------------------------
#include <chrono>

#include <CMD/CMDlineParser.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLmap.h>
#include <COL/COLthreadPool.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <PRO/PROexecute.h>
#include <SCK/SCKloop.h>
#ifdef _WIN32
#include <SCK/SCKutils.h>
#endif
COL_LOG_MODULE;


// NOTE: Never modify TestList from a worker thread!
static COLmap<COLstring, bool> TestList;
static COLmap<COLstring, bool> LibList;
static int ExitCode = 0;
static const int BUILD_TIMEOUT = 60;   // seconds

static void getBuildList() {
   COL_FUNCTION(getBuildList);
   COLstring DirName;
   FILdirEnumerator Enumerator("../*", true);
   while (Enumerator.getFile(DirName)) {
      if (!Enumerator.isDirectory()) {
         continue;
      }
      size_t Position = DirName.find("example");
      if (Position != npos && Position > 1) {    // test dir
         TestList[DirName] = true;
         continue;
      }
      COLstring Copy = DirName;
      if (DirName.toUpperCase() == Copy) {    // lib dir
         LibList[DirName] = true;
         continue;
      }
   }
}

static void buildFinished(SCKloop* pLoop, COLstring Name, int Result, long long Time, bool IsTest) {
   COL_FUNCTION(buildFinished);
   COLmap<COLstring, bool>* pList = IsTest ? &TestList : &LibList;
   pList->erase(Name);
   // no-style stdout
   // COLcout << "Build task " << pList->size() << "\t'" << Name << "'\tcompleted with code " << Result << "\tin " << Time << " ms" << newline;
   // table-style stdout
   COLcout << "[" << pList->size() << "]\t" << Name << "\t" << Result << "\t" << Time << " ms" << newline;
   if (Result != 0) {
      ExitCode++;
   }
   if (pList->size() == 0) {
      COL_TRC("Build completed!");
      pLoop->shutdown();
   }
}

static void runTestWorker(const COLstring& TestDir, const COLstring& TestName, const COLstring& LogDir, COLstring* pOutput, int* pExitCode) {
   COL_FUNCTION(runTestWorker);
#ifdef _WIN32
   COLstring Command = "cmd.exe /c test.exe";
#else
   COLstring Command = "sh -c ./test";
#endif
	COLstring Err;
   *pExitCode = PROexecuteString(TestDir, Command, pOutput, &Err, BUILD_TIMEOUT);
   if (*pExitCode) {
      FILwriteFile(FILpathAppend(LogDir, TestName + ".test.txt"), *pOutput + "\n" + Err);
   }
}

static void buildWorker(SCKloop* pLoop, COLstring Name, COLstring LogDir, bool IsTest) {
   COL_FUNCTION(buildWorker);
   auto Start = std::chrono::steady_clock::now();
   COLstring TestDir = FILpathAppend("..", Name);
   COLstring Out, Err;
   int ExitCode = PROexecuteString(TestDir, "make", &Out, &Err, BUILD_TIMEOUT);
   if (0 == ExitCode) {
      if (IsTest) {
         runTestWorker(TestDir, Name, LogDir, &Out, &ExitCode);
      }
   } else {
      FILwriteFile(FILpathAppend(LogDir, Name + ".build.txt"), Out + "\n" + Err);
   }
   auto Stop = std::chrono::steady_clock::now();
   long long Time = std::chrono::duration_cast<std::chrono::milliseconds>(Stop - Start).count();
   pLoop->post(COLnewClosure0(&buildFinished, pLoop, Name, ExitCode, Time, IsTest));
   return;
}

int main (int argc, const char** argv) {
#ifdef _WIN32
   SCKinitWinsock();
#endif
   FILinitExecutableName(argv[0]);
   CMDlineParser LineParser;
   LineParser.addFlagWithArgument("log_dir", "Directory for logs");
   LineParser.addFlagWithArgument("jobs", "The number of jobs to run simultaneously (default 8)");
   LineParser.parseArgs(argc,argv);
   if (LineParser.parsingErrorsPresent(COLcout)) {
      LineParser.showUsage(COLcout);
      return EXIT_FAILURE;
   }

   int Worker = 8;
   if (LineParser.isFlagPresent("jobs")) {
      Worker = ::atoi(LineParser.flagArgument("jobs").c_str());
      if (Worker <= 0) {
         COLcerr << "The '--jobs' option requires a positive integral argument." << newline;
         return EXIT_FAILURE;
      }
   }

   COLstring LogDir = "logs";
   if (LineParser.isFlagPresent("log_dir")) {
      LogDir = FILpathResolveEnvironment(LineParser.flagArgument("log_dir"));
   }
   if (FILfileExists(LogDir)) {
      COLstring Error;
      if (!FILremoveFullDirNewSafe(LogDir, &Error)){
         COLcerr << "Failed to remove " << LogDir << ", error: " << Error << newline;
         return EXIT_FAILURE;
      }
   }
   FILmakeDir(LogDir, FIL_USER_RWX);
   COLcout << "Log directory: " << LogDir << newline;

   COLthreadPool Pool(1, Worker);
   SCKloop Loop(&Pool);
   Pool.start();

   // generate library and test lists
   getBuildList();

   // Step 1: build libraries
   COLcout << "======= Build Libraries =======" << newline;
   COLcout << "Task\tName\tExit\tTime" << newline;
   COLmap<COLstring, bool>::const_iterator i;
   for (i = LibList.cbegin(); i != LibList.cend(); i++){
      Loop.threadPool()->scheduleTask(COLnewClosure0(&buildWorker, &Loop, i.key(), LogDir, false));
   }
   Loop.start();

   // Step 2: build and run unit tests
   COLcout << newline << "======= Build and Run Unit Tests =======" << newline;
   COLcout << "Task\tName\t\tExit\tTime" << newline;
   for (i = TestList.cbegin(); i != TestList.cend(); i++) {
      Loop.threadPool()->scheduleTask(COLnewClosure0(&buildWorker, &Loop, i.key(), LogDir, true));
   }
   Loop.start();

   return ExitCode;  // set by buildFinished()
}
