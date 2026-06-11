//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: CRHcrashMac
//
// Author: John Q
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------

#ifdef __APPLE__

#include <cxxabi.h>
#include <dlfcn.h>
#include <errno.h>
#include <sys/sysctl.h>
#include <sys/utsname.h>
#include <signal.h>
#include <mach/mach.h>
#include <mach/mach_init.h>
#include <mach/mach_vm.h>
#include <mach/thread_act.h>
#include <mach-o/dyld.h>

#include <CRH/CRHcrash.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>

#include <TargetConditionals.h>

COL_LOG_MODULE;


static void print_stack_frame(void* Address, int i, COLostreamString& bt) {
   Dl_info info;
   if (dladdr(Address, &info) && info.dli_sname) {
      int status = 0;
      char* demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
      const char* func_name = (status == 0 && demangled) ? demangled : info.dli_sname;
      bt << i << "\t" << Address << "\t" << func_name << " + " << (char*)Address - (char*)info.dli_saddr << newline;
      free(demangled);
   } else {
      bt << i << "\t" << Address << "\t[Unknown Symbol]" << newline;
   }
}

#if TARGET_CPU_X86_64   // Intel

#include <mach/i386/thread_status.h>

void print_stack_from_registers(uint64_t rip, uint64_t rbp, COLostreamString& bt) {
   int frame = 0;
   while (rbp != 0 && frame < 250) {
      print_stack_frame((void*)rip, frame, bt);

      // Read next frame
      uint64_t *rbp_ptr = (uint64_t *)rbp;
      rip = rbp_ptr[1];
      rbp = rbp_ptr[0];
      frame++;
   }
}

#elif TARGET_CPU_ARM64  // Apple Silicon

#include <mach/arm/thread_status.h>

void print_stack_from_registers(uint64_t pc, uint64_t fp, COLostreamString& bt) {
   int frame = 0;
   while (fp && frame < 250) {
      print_stack_frame((void*)pc, frame, bt);

      // Walk frame pointer manually
      uint64_t *fp_ptr = (uint64_t *)fp;
      uint64_t next_fp = fp_ptr[0];
      uint64_t next_pc = fp_ptr[1];

      // Safety check: avoid infinite loops or bad memory
      if (next_fp <= fp || next_fp == 0) break;

      fp = next_fp;
      pc = next_pc;
      frame++;
   }
   bt << decimal << newline;
}

#endif

static void CRHbacktraceAllThreadsMac(COLostreamString& bt) {
   // COL_FUNCTION(CRHbacktraceAllThreadsMac);
   mach_port_t CurrentThread = mach_thread_self();
   // COL_VAR(CurrentThread);
   thread_act_array_t ThreadList;
   mach_msg_type_number_t ThreadCount = 0;
   if (task_threads(mach_task_self(), &ThreadList, &ThreadCount) != KERN_SUCCESS) {
      bt << "Unable to retrieve Iguana's thread list" << newline;
      return;
   }
   // COL_VAR(ThreadCount);
   for (mach_msg_type_number_t i = 0; i < ThreadCount; i++) {
      thread_t Thread = ThreadList[i];
      // COL_TRC("Thread " << ThreadList[i]);
      if (CurrentThread == Thread) { continue; }

      // suspend other threads when capturing their call stacks
      thread_suspend(Thread);

#if TARGET_CPU_X86_64   // Intel
      x86_thread_state64_t State;
      thread_state_flavor_t Flavour = x86_THREAD_STATE64;
      mach_msg_type_number_t StateCount = x86_THREAD_STATE64_COUNT;
#elif TARGET_CPU_ARM64  // Apple Silicon
      arm_thread_state64_t State;
      thread_state_flavor_t Flavour = ARM_THREAD_STATE64;
      mach_msg_type_number_t StateCount = ARM_THREAD_STATE64_COUNT;
#endif
      if (thread_get_state(Thread, Flavour, (thread_state_t)&State, &StateCount) == KERN_SUCCESS) {
         bt << "Thread " << Thread << newline;
#if TARGET_CPU_X86_64   // Intel
         uint64_t pc = State.__rip;
         uint64_t fp = State.__rbp;
#elif TARGET_CPU_ARM64  // Apple Silicon
         uint64_t pc = State.__pc;
         uint64_t fp = State.__fp;
#endif
         print_stack_from_registers(pc, fp, bt);
      }

      // resume other threads after capturing their call stacks
      thread_resume(Thread);
   }

   vm_deallocate(mach_task_self(), (vm_address_t)ThreadList, ThreadCount * sizeof(thread_t));
}

// returns the value 0 for a native process, 1 for a translated process, and -1 when an error occurs.
// https://developer.apple.com/documentation/apple-silicon/about-the-rosetta-translation-environment
static int CRHprocessIsTranslated() {
   int ret = 0;
   size_t size = sizeof(ret);
   if (sysctlbyname("sysctl.proc_translated", &ret, &size, NULL, 0) == -1) {
      if (errno == ENOENT) { return 0; }
      return -1;
   }
   return ret;
}

static void CRHgenerateCrashDumpMac(int Signal) {
   // COL_FUNCTION(CRHgenerateCrashDumpMac);
   static COLostreamString DumpStream;
   CRHcrashContext(DumpStream);
   CRHcrashHeader(DumpStream, CRHsignalException(Signal), mach_thread_self());
   COLbacktrace2(DumpStream);                   // reliable but current thread only
   if (!CRHprocessIsTranslated() && (CRHcrashDumpMode() != 1)) {
      CRHbacktraceAllThreadsMac(DumpStream);    // less reliable but for all threads
   }
   CRHfinalizeCrashDumpGeneration(DumpStream);
}

void CRHhandleSignalMac(int Signal) {
   CRHgenerateCrashDumpMac(Signal);
   // reset segfault handler
   ::signal(Signal, SIG_DFL);
   // raise segfault
   ::raise(Signal);
}

void CRHsetUpCrashLog() {
   // capture the often more informative message for abort
   COLsetAbortCallback(&CRHsetAbortMessage);
   // set up signal handler for segfault
   ::signal(SIGSEGV, CRHhandleSignalMac);
   ::signal(SIGABRT, CRHhandleSignalMac);
   ::signal(SIGBUS,  CRHhandleSignalMac);
}

static COLstring CRHmacArchitecture() {
   COLstring Architecture("Failed to get CPU architecture");
   int arm64 = 0;
   size_t size = sizeof(arm64);
   // Apple had stopped selling 64-bit Intel CPU Macs in Jun. 2023
   // If arm64 is supported, we have an ARM mac, otherwise 64-bit Intel mac
   if (sysctlbyname("hw.optional.arm64", &arm64, &size, NULL, 0) == 0) {
      if (arm64 == 1) { Architecture = "ARM64";  }
      else            { Architecture = "X86_64"; }
   }
   return Architecture;
}

COLstring CRHgetOS() {
   struct utsname buffer;
   if (uname(&buffer) != 0) {
      return COLlastErrorAsString();
   }
   COLostreamString Stream;
   Stream << buffer.sysname << " | " << buffer.nodename << " | " << buffer.release << " | " << buffer.version << " | " << CRHmacArchitecture();
   return Stream.str();
}

#endif