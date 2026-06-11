//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: CRHcrashLinux
//
// Author: John Q
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------

#ifdef __linux__

#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/syscall.h>
#include <sys/utsname.h>

#include <CRH/CRHcrash.h>
#include <COL/COLerror.h>
#include <COL/COLlocker.h>
#include <COL/COLmutex.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLostreamString g_DumpStream;
static int g_DoNotDisturb = 0;
static int g_ThreadCount = 0;
static COLmutex g_Mutex;

static void CRHflushAndExit() {
   // COL_FUNCTION(CRHflushAndExit);
   CRHfinalizeCrashDumpGeneration(g_DumpStream);

   int Signal = g_DoNotDisturb;
   // reset signal handler
   ::signal(Signal, SIG_DFL);
   // raise signal
   ::raise(Signal);
}

static void CRHhandleCallStackSignal(int sig) {
   // COL_FUNCTION(CRHhandleCallStackSignal);
   // COL_TRC("Thread " << syscall(SYS_gettid) << " received signal to capture call stack");
   COLlocker Locker(g_Mutex);
   g_DumpStream << "Thread " << syscall(SYS_gettid) << newline;
   COLbacktrace2(g_DumpStream);
   g_ThreadCount--;
   // COL_VAR(g_ThreadCount);
   if (g_ThreadCount > 0) {
      // wait for other threads to save their call stack to g_DumpStream
      return;
   }
   // g_ThreadCount = 0, we have the call stacks from all threads
   CRHflushAndExit();
}

static void CRHbacktraceAllThreadsLinux() {
   // COL_FUNCTION(CRHbacktraceAllThreadsLinux);
   pid_t CrashThreadId = syscall(SYS_gettid);
   DIR* pDir = opendir("/proc/self/task");
   if (NULL == pDir) {
      g_DumpStream << "Unable to capture call stacks for other threads." << newline;
      CRHfinalizeCrashDumpGeneration(g_DumpStream);
      return;
   }
   g_ThreadCount = 0;
   struct dirent *Entry;
   pid_t Pid = getpid();
   while ((Entry = readdir(pDir)) != NULL) {
      if (isdigit(Entry->d_name[0])) {
         pid_t ThreadId = atoi(Entry->d_name);
         if (ThreadId == CrashThreadId) { continue; }
         // COL_TRC("Signal thread " << ThreadId << " to capture its call stack");
         // TODO: may need to save ThreadId to a list and set g_ThreadCount before
         // sending signals. i.e. what if the first thread gets the signal immediately
         // and decrement g_ThreadCount to 0 and CRHfinalizeCrashDumpGeneration().
         g_ThreadCount++;
         syscall(SYS_tgkill, Pid, ThreadId, SIGUSR1);
      }
   }
}

static void CRHgenerateCrashDumpLinux(int Signal) {
   // COL_FUNCTION(CRHgenerateCrashDumpLinux);
   CRHcrashContext(g_DumpStream);
   CRHcrashHeader(g_DumpStream, CRHsignalException(Signal), syscall(SYS_gettid));
   COLbacktrace2(g_DumpStream);           // reliable but current thread only
   if (CRHcrashDumpMode() == 1) {
      CRHflushAndExit();
   } else {
      CRHbacktraceAllThreadsLinux();      // less reliable but for other threads
   }
}

// In Ubuntu Server 24.04 for ARM:
// When a segfault happens, SIGSEGV is repeatedly delivered to this function until program exits.
// When iguana is aborted (e.g. COLASSERT(false)), SIGABRT is only delivered once.
static void CRHhandleSignalLinux(int Signal) {
   // COL_FUNCTION(CRHhandleSignalLinux);
   // COL_VAR2(syscall(SYS_gettid), Signal);
   if (g_DoNotDisturb) {
      // COL_TRC("I am busy. Go away.");
      if (SIGABRT == Signal) {
         // Emulate SIGSEGV behaviour for SIGABRT
         sleep(1);
         abort();
      }
      return;
   }
   // We will keep receiving the Signal while the crash dump is being generated. To prevent
   // CRHgenerateCrashDumpLinux() from being called repeatedly, set g_DoNotDisturb to ignore them.
   // We don't need to clear g_DoNotDisturb because Iguana is destined to terminate after this.
   g_DoNotDisturb = Signal;
   CRHgenerateCrashDumpLinux(Signal);
   // Give CRHgenerateCrashDumpLinux() some time to capture call stacks for other threads.
   sleep(1);
   // Emulate SIGSEGV behaviour for SIGABRT, i.e. keep sending SIGABRT until crash dump
   // generation is completed.
   if (SIGABRT == Signal) { abort(); }
}

void CRHsetUpCrashLog() {
   // capture the often more informative message for abort
   COLsetAbortCallback(&CRHsetAbortMessage);
   // set up signal handler for crashes
   ::signal(SIGSEGV, CRHhandleSignalLinux);
   ::signal(SIGABRT, CRHhandleSignalLinux);
   ::signal(SIGBUS,  CRHhandleSignalLinux);
   // set up custom signal for getting call stacks from other
   // threads when Iguana crashes
   ::signal(SIGUSR1, CRHhandleCallStackSignal);
}

COLstring CRHgetOS() {
   struct utsname buffer;
   if (uname(&buffer) != 0) {
      return COLlastErrorAsString();
   }
   COLostreamString Stream;
   Stream << buffer.sysname << " | " << buffer.nodename << " | " << buffer.release << " | " << buffer.version << " | " << buffer.machine;
   return Stream.str();
}

#endif
