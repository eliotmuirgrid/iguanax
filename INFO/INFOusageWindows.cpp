//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INFOusageWindows
//
// Description:
//
// CPU, memory and disk usage implementations for Windows.
//
// Author: Matthew Sobkowski
// Date:   Thu 05/13/2021
//---------------------------------------------------------------------------

#include <windows.h>
#include <crtdbg.h>  // for _set_invalid_parameter_handler
#include <stdio.h>   // for _getmaxstdio, _setmaxstdio
#include <psapi.h>   // PROCESS_MEMORY_COUNTERS_EX, must be included after windows.h
#include <chrono>    // for ms sleep
#include <thread>    // for ms sleep

#include <INFO/INFOusage.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

/**
 * Helper Functions
 */

static uint64_t FileTimeToInt64(const FILETIME & ft) {
   return ( ((uint64_t)(ft.dwHighDateTime)) << 32 ) | ((uint64_t)ft.dwLowDateTime);
}

/**
 * Public Functions
 */
// https://stackoverflow.com/a/64166

INFOmemory INFOmemoryStats() {
   COL_FUNCTION(INFOmemoryStats);
   INFOmemory Memory;
   // https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
   MEMORYSTATUSEX memInfo;
   memInfo.dwLength = sizeof(MEMORYSTATUSEX);
   GlobalMemoryStatusEx(&memInfo);
   PROCESS_MEMORY_COUNTERS_EX pmc;
   GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
   Memory.Virtual  = memInfo.ullTotalPageFile;
   Memory.Physical = memInfo.ullTotalPhys;
   Memory.Resident = pmc.WorkingSetSize;
   return Memory;
}

double INFOcpuPercent() {
   COL_FUNCTION(INFOcpuPercent);
   static HANDLE Process = INVALID_HANDLE_VALUE;
   static uint64_t LastTime, LastCPUtime;
   static int NumCores;
   FILETIME FilTime, SysTime, UsrTime;
   if (INVALID_HANDLE_VALUE == Process) {
      Process = GetCurrentProcess();
      // init number of CPU cores
      SYSTEM_INFO Info;
      GetSystemInfo(&Info);
      NumCores = Info.dwNumberOfProcessors;
      // init time
      GetSystemTimeAsFileTime(&FilTime);
      LastTime = FileTimeToInt64(FilTime);
      // init time use by current process
      GetProcessTimes(Process, &FilTime, &FilTime, &SysTime, &UsrTime);
      LastCPUtime = FileTimeToInt64(SysTime) + FileTimeToInt64(UsrTime);
      // initial run, sleep for 100ms so we can calculate CPU load
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
   }
   // get current time
   GetSystemTimeAsFileTime(&FilTime);
   uint64_t Now = FileTimeToInt64(FilTime);
   // get time used by current process
   GetProcessTimes(Process, &FilTime, &FilTime, &SysTime, &UsrTime);
   uint64_t CPUtime = FileTimeToInt64(SysTime) + FileTimeToInt64(UsrTime);
   // calculate load by current process
   double Percent = (CPUtime - LastCPUtime) * 100.0 / (Now - LastTime) / NumCores;
   LastTime = Now;
   LastCPUtime = CPUtime;
   return Percent;
}

INFOdisk INFOdiskStats(const COLstring& DirPath) {
   COL_FUNCTION(INFOdiskStats);
   INFOdisk Disk;
   COLuint64 FreeForAll = 0;
   if (GetDiskFreeSpaceEx(DirPath.data(), (ULARGE_INTEGER*)&Disk.Free, (ULARGE_INTEGER*)&Disk.Size, (ULARGE_INTEGER*)&FreeForAll)) {
      Disk.Used = Disk.Size - FreeForAll;
   } else {
      COLstring Error = COLlastErrorAsString();
      COL_ERR(Error);
   }
   return Disk;
}

int INFOfileHandleLimit(){
   COL_FUNCTION(INFOfileHandleLimit);
   // Windows doesn't really have a maximum like posix does.
   // _getmaxstdio will return the number of FILE* handles allowed by the C runtime library, default of 512
   // a bit different from ulimit in that ulimit returns everything - sockets, pipes, files, etc.
   return _getmaxstdio();
}

static void IgnoreInvalidParameter(const wchar_t*, const wchar_t*, const wchar_t*, unsigned int, uintptr_t){
   // intentionally empty
}

int INFOfileHandleCount() {
   COL_FUNCTION(INFOfileHandleCount);
   int MaxFiles  = _getmaxstdio();
   int Count     = 0;
   struct _stat s;
   auto OldHandler = _set_invalid_parameter_handler(IgnoreInvalidParameter);

   for (int fd = 0; fd < MaxFiles; ++fd){
      if (_fstat(fd, &s) == 0) ++Count;
   }

   _set_invalid_parameter_handler(OldHandler);
   COL_TRC("Open CRT file descriptors: " << Count << newline);
   return Count;
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
   if (Limit > 8192) { Limit = 8192; } // Microsoft's documentation says this is the cap: https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setmaxstdio?view=msvc-170
   _setmaxstdio(Limit);
}