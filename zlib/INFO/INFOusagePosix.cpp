//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INFOusagePosix
//
// Description:
//
// CPU, memory and disk usage implementations for Linux and macOS.
//
// Author: Shuo Hou
// Date:   06/30/2021
//---------------------------------------------------------------------------

#include <sys/stat.h>      // fstat
#include <sys/statvfs.h>   // statvfs
#include <sys/resource.h>  // getrlimit, setrlimit
#ifdef __linux__
#include <stdio.h>         // fopen, fscanf, fclose
#include <sys/times.h>     // times
#include <unistd.h>        // getpagesize, sysconf, usleep
#endif
#ifdef __APPLE__
#include <mach/mach.h>     // task_info, task_threads, thread_info, vm_deallocate
#include <sys/sysctl.h>    // sysctl
#endif

#include <INFO/INFOusage.h>

#include <COL/COLlog.h>
#include <COL/COLvar.h>
COL_LOG_MODULE;


/**
 * Public Functions
 */

#ifdef __linux__
// TODO: file io
INFOmemory INFOmemoryStats() {
   COL_FUNCTION(INFOmemoryStats);
   INFOmemory Memory;
   FILE* File = ::fopen("/proc/self/statm", "r");
   if (NULL == File) {
      COL_ERR("Failed to open '/proc/self/statm'.");
      return Memory;
   }
   if (2 == ::fscanf(File, "%lld %lld", &Memory.Virtual, &Memory.Resident)) {
      int PageSize = ::getpagesize();
      Memory.Virtual  *= PageSize;
      Memory.Resident *= PageSize;
      Memory.Physical  = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE);
   }
   ::fclose(File);
   return Memory;
}

// TODO: file io
double INFOcpuPercent() {
   COL_FUNCTION(INFOcpuPercent);
   static int NumCores = 0;
   static clock_t LastTime, LastCPUtime;
   struct tms Times;
   if (0 == NumCores) {
      // init number of CPU cores
      char buffer[128];
      FILE* File = fopen("/proc/cpuinfo", "r");
      if (NULL == File) {
         COL_ERR("Failed to open '/proc/cpuinfo'.");
         return -1;     // can't init cpu cores
      }
      while (fgets(buffer, 128, File) != NULL) {
         if (strncmp(buffer, "processor", 9) == 0) {
            NumCores++;
         }
      }
      fclose(File);
      LastTime = times(&Times);     // init time
      LastCPUtime = Times.tms_stime + Times.tms_utime;   // init time use by current process
      usleep(100000);               // initial run, sleep for 100ms so we can calculate CPU load
   }
   clock_t Now = times(&Times);     // get current times
   clock_t CPUtime = Times.tms_stime + Times.tms_utime;  // get time used by current process
   double CpuUsage = -2;            // in case clock_t overflows
   if (Now > LastTime && CPUtime >= LastCPUtime) {
      CpuUsage = (CPUtime - LastCPUtime) * 100.0 / (Now - LastTime) / NumCores;
   } // else we have clock_t overflow.
   LastTime = Now;
   LastCPUtime = CPUtime;
   return CpuUsage;
}
#endif

#ifdef __APPLE__
INFOmemory INFOmemoryStats() {
   COL_FUNCTION(INFOmemoryStats);
   INFOmemory Memory;
   struct task_basic_info Task;
   mach_msg_type_number_t TaskCount = TASK_BASIC_INFO_COUNT;
   if (KERN_SUCCESS == task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&Task, &TaskCount)) {
      Memory.Virtual  = Task.virtual_size;
      Memory.Resident = Task.resident_size;
   }
   int mib[2] = { CTL_HW, HW_MEMSIZE };
   size_t Length = sizeof(COLuint64);
   sysctl(mib, 2, &Memory.Physical, &Length, NULL, 0);
   return Memory;
}

double INFOcpuPercent() {
   COL_FUNCTION(INFOcpuPercent);
   thread_t *Threads = NULL;
   mach_msg_type_number_t ThreadsCount = 0;
   if (KERN_SUCCESS != task_threads(mach_task_self(), &Threads, &ThreadsCount)) {
      COL_ERR("Failed to fetch thread list");
      return -1;
   }
   double CpuUsage = 0;
   struct thread_basic_info Thread;
   mach_msg_type_number_t ThreadCount = THREAD_BASIC_INFO_COUNT;
   for (int ThreadIndex = 0; ThreadIndex < ThreadsCount; ThreadIndex++) {
      if (KERN_SUCCESS != thread_info(Threads[ThreadIndex], THREAD_BASIC_INFO, (thread_info_t)&Thread, &ThreadCount)) { continue; }
      CpuUsage += Thread.cpu_usage;
   }
   CpuUsage = CpuUsage * 100 / TH_USAGE_SCALE;
   COL_VAR(CpuUsage);
   vm_deallocate(mach_task_self(), (vm_address_t)Threads, ThreadsCount * sizeof(thread_t));
   return CpuUsage;
}
#endif

INFOdisk INFOdiskStats(const COLstring& DirPath) {
   COL_FUNCTION(INFOdiskStats);
   INFOdisk Disk;
   struct statvfs Stats;
   if (statvfs(DirPath.c_str(), &Stats)) {
      COLstring Error = COLlastErrorAsString();
      COL_ERR(Error);
   } else {
      Disk.Free = (COLuint64)Stats.f_bavail * Stats.f_frsize;
      Disk.Size = (COLuint64)Stats.f_blocks * Stats.f_frsize;
      Disk.Used = Disk.Size - (COLuint64)Stats.f_bfree * Stats.f_frsize;
   }
   return Disk;
}

int INFOfileHandleLimit(){
   COL_FUNCTION(INFOfileHandleLimit);
   struct rlimit rlimitStruct;
   int rc = ::getrlimit(RLIMIT_NOFILE, &rlimitStruct);
   COL_VAR(rlimitStruct.rlim_cur);
   int FileDescriptorLimit = rc == 0 ? rlimitStruct.rlim_cur : 2048;
   return FileDescriptorLimit;
}

int INFOfileHandleCount() {
   COL_FUNCTION(INFOfileHandleCount);
   int FileDescriptorLimit = INFOfileHandleLimit();
   int FdCount = 0;
   struct stat statStruct;
   for (int i = 0; i < FileDescriptorLimit; ++i) {
      if (::fstat(i, &statStruct) == 0) {
         ++FdCount;
      }
   }
   return FdCount;
}


void INFOmemoryUsage(COLvar *pInput) {
   COL_FUNCTION(INFOmemoryUsage);
   COLvar& Result = *pInput;
   static COLfloat64 MB = 1000000;
   INFOmemory Memory = INFOmemoryStats();
   Result["VirtualMB"]  = Memory.Virtual  / MB;
   Result["ResidentMB"] = Memory.Resident / MB;
   Result["PhysicalMB"] = Memory.Physical / MB;
}

void INFOcpuUsage(COLvar *pInput) {
   COL_FUNCTION(INFOcpuUsage);
   COLvar& Result = *pInput;
   Result["CPU"] = INFOcpuPercent();
}

void INFOdiskUsage(COLvar *pInput) {
   COL_FUNCTION(INFOdiskUsage);
   COLvar& Result = *pInput;
   static COLfloat64 GB = 1000000000.0;
   INFOdisk Disk = INFOdiskStats();
   Result["DiskSizeGB"] = Disk.Size / GB;
   Result["DiskUsedGB"] = Disk.Free / GB;
   Result["DiskFreeGB"] = Disk.Used / GB;
}

void INFOfileHandle(COLvar *pInput) {
   COL_FUNCTION(INFOfileHandle);
   COLvar& Result = *pInput;
   Result["OpenFD"] = INFOfileHandleCount();
}

void INFOsetFdLimit(int Limit) {
   COL_FUNCTION(INFOsetFdLimit);
   struct rlimit rlimitStruct;
   int rc = ::getrlimit(RLIMIT_NOFILE, &rlimitStruct);
   if (Limit > rlimitStruct.rlim_max) { Limit = rlimitStruct.rlim_max; }
   rlimitStruct.rlim_cur = Limit;
   rc = ::setrlimit(RLIMIT_NOFILE, &rlimitStruct);
   COL_VAR(rc);
}