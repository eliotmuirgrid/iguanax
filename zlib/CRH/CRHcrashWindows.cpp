//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: CRHcrashWindows
//
// Author: John Q
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------

#include "CRHcrash.h"

#include <io.h>   // for _A_NORMAL
#include <stdio.h> // printf
#include <windows.h>
#include <new.h>
#include <signal.h>
#include <dbghelp.h>
#include <tlhelp32.h>

#include <REG/REGregistryWindows.h>

#include <FIL/FILutils.h>

#include <INST/INSTstart.h>
#include <COL/COLlocker.h>
#include <COL/COLmutex.h>

#include <COL/COLdll.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


bool CRHcoresDirExistsWithRightPermissions() {
   COL_FUNCTION(CRHcoresDirExistsWithRightPermissions);
   return (FILhasPermission("/cores", 0) && FILhasPermission("/cores", _A_NORMAL)); // TESTME: _A_NORMAL means normal file with no read write restrictions
}

bool CRHenableCrashDump() {
   COL_FUNCTION(CRHenableCrashDump);
   try {
      REGregistry Register(HKEY_LOCAL_MACHINE);
      Register.openKey("SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\LocalDumps\\iguana.exe", true);
      // static COLstring DumpDir = "C:\\";
      // if (Register.readString("DumpFolder", 0) != DumpDir) {
      //    Register.writeString("DumpFolder", DumpDir);
      // }
      if (Register.readInteger("DumpCount", 0) != 7) {
         Register.writeInteger("DumpCount", 7);  // keep up to 7 dumps
      }
      if (Register.readInteger("DumpType", 0) != 2) {
         Register.writeInteger("DumpType", 2);  // full dump
      }
      return true;
   } catch(const COLerror& Error) {
      COL_ERR(Error.description());
      return false;
   }
   return true;
}

static COLstring CRHwindowsVersion() {
   WCHAR systemPath[MAX_PATH];
   UINT size = GetSystemDirectoryW(systemPath, MAX_PATH);
   if (size == 0 || size > MAX_PATH) { return "Unknown"; }

   DWORD dummy;
   wcscat_s(systemPath, MAX_PATH, L"\\kernel32.dll");
   DWORD sizeInfo = GetFileVersionInfoSizeW(systemPath, &dummy);
   if (sizeInfo == 0) { return "Unknown"; }

   BYTE* buffer = (BYTE*)malloc(sizeInfo);
   if (!buffer) { return "Unknown"; }

   if (!GetFileVersionInfoW(systemPath, 0, sizeInfo, buffer)) {
      free(buffer);
      return "Unknown";
   }

   VS_FIXEDFILEINFO* fileInfo = NULL;
   UINT len = 0;
   if (!VerQueryValueW(buffer, L"\\", (LPVOID*)&fileInfo, &len)) {
      free(buffer);
      return "Unknown";
   }

   if (!fileInfo) {
      free(buffer);
      return "Unknown";
   }

   COLstring Version = COLintToString(HIWORD(fileInfo->dwFileVersionMS)) + "."
                     + COLintToString(LOWORD(fileInfo->dwFileVersionMS)) + "."
                     + COLintToString(HIWORD(fileInfo->dwFileVersionLS)) + "."
                     + COLintToString(LOWORD(fileInfo->dwFileVersionLS));
   free(buffer);
   return Version;
}

static COLstring CRHwindowArchitecture() {
   COLstring   Architecture;
   SYSTEM_INFO sysInfo;
   GetNativeSystemInfo(&sysInfo);
   // reverted 75c20d1 previous commit because it breaks customers running on decade old systems
   switch(sysInfo.wProcessorArchitecture) {
      case PROCESSOR_ARCHITECTURE_AMD64: Architecture = "x64"; break;
      case PROCESSOR_ARCHITECTURE_ARM:   Architecture = "ARM"; break;
      case PROCESSOR_ARCHITECTURE_ARM64: Architecture = "ARM64"; break;
      case PROCESSOR_ARCHITECTURE_INTEL: Architecture = "x86"; break;
      default: return Architecture;
   }
}

static COLstring CRHwindowsComputerName() {
   COLstring Name;
   Name.setCapacity(MAX_COMPUTERNAME_LENGTH);
   DWORD Size = Name.capacity();
   if (GetComputerName(Name.get_buffer(), &Size)) {
      Name.setSize(Size);
   } else {
      Name = "Unknown";
   }
   return Name;
}

COLstring CRHgetOS() {
   COLostreamString Stream;
   Stream << "Windows | " << CRHwindowsComputerName() << " | " << CRHwindowsVersion() << " | " << CRHwindowArchitecture();
   return Stream.str();
}


static void CRHprintSymbolFromAddress(HANDLE hProcess, DWORD64 address, int Index, COLostreamString& bt) {
   char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)] = {0};
   SYMBOL_INFO* symbol = (SYMBOL_INFO*)symbolBuffer;
   symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
   symbol->MaxNameLen = MAX_SYM_NAME;

   DWORD64 displacement = 0;
   if (!SymFromAddr(hProcess, address, &displacement, symbol)) {
      // long Err = ::GetLastError();
      // COL_TRC("Error resolving symbol " << Index << ": " << COLerrorString(Err));
      bt << Index << "\t0x" << address << "\t0x" << displacement << "\t\t[Unknown Symbol]\n";
   } else {
      bt << Index << "\t0x" << address << "\t0x" << displacement << "\t\t" << symbol->Name << "\n";
   }
}

static void CRHprintThreadCallStack(HANDLE hThread, COLostreamString& bt) {
   CONTEXT context;
   ZeroMemory(&context, sizeof(CONTEXT));
   context.ContextFlags = CONTEXT_FULL;
   if (!GetThreadContext(hThread, &context)) {
      long Err = ::GetLastError();
      bt << "\tFailed to get thread context: " << COLerrorString(Err);
      return;
   }

   DWORD imageType;
   STACKFRAME64 stack;
   ZeroMemory(&stack, sizeof(STACKFRAME64));
#ifdef _M_IX86
   imageType = IMAGE_FILE_MACHINE_I386;
   stack.AddrPC.Offset    = context.Eip;
   stack.AddrFrame.Offset = context.Ebp;
   stack.AddrStack.Offset = context.Esp;
#elif _M_X64
   imageType = IMAGE_FILE_MACHINE_AMD64;
   stack.AddrPC.Offset    = context.Rip;
   stack.AddrFrame.Offset = context.Rbp;
   stack.AddrStack.Offset = context.Rsp;
#elif _M_ARM64
   imageType = IMAGE_FILE_MACHINE_ARM64;
   stack.AddrPC.Offset    = context.Pc;
   stack.AddrFrame.Offset = context.Fp;
   stack.AddrStack.Offset = context.Sp;
#else
#error "Unsupported architecture"
#endif
   stack.AddrPC.Mode    = AddrModeFlat;
   stack.AddrFrame.Mode = AddrModeFlat;
   stack.AddrStack.Mode = AddrModeFlat;

   int Index = 0;
   HANDLE hProcess = GetCurrentProcess();
   while (StackWalk64(imageType, hProcess, hThread, &stack, &context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL)) {
      if (stack.AddrPC.Offset == 0) { break; }
      CRHprintSymbolFromAddress(hProcess, stack.AddrPC.Offset, Index++, bt);
   }
   bt << newline;
}

static void CRHbacktraceAllThreadsWindows(COLostreamString& bt) {
   DWORD Pid = GetCurrentProcessId();
   DWORD ThisThreadId = GetCurrentThreadId();
   HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
   if (Snapshot == INVALID_HANDLE_VALUE) {
      printf("Failed to create snapshot\n");
      return;
   }
   THREADENTRY32 Thread;
   Thread.dwSize = sizeof(THREADENTRY32);
   if (Thread32First(Snapshot, &Thread)) {
      do {
         if (Thread.th32OwnerProcessID != Pid) { continue; }
         // COL_VAR(Thread.th32ThreadID);
         bt << "Thread " << Thread.th32ThreadID;
         if (Thread.th32ThreadID == ThisThreadId) { bt << " CRASHED"; }
         bt << newline;
         HANDLE hThread = OpenThread(THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION, FALSE, Thread.th32ThreadID);
         if (NULL == hThread) {
            long Err = ::GetLastError();
            // COL_TRC("Failed to open thread " << Thread.th32ThreadID << " : " << COLerrorString(Err));
            bt << "\tFailed to open thread: " << COLerrorString(Err);
            continue;
         }
         if (Thread.th32ThreadID == ThisThreadId) { SuspendThread(hThread); }
         CRHprintThreadCallStack(hThread, bt);
         if (Thread.th32ThreadID == ThisThreadId) { ResumeThread(hThread); }
         CloseHandle(hThread);
      } while (Thread32Next(Snapshot, &Thread));
   }
   CloseHandle(Snapshot);
}

static void CRHgenerateCrashDumpWindows(const COLstring& Exception) {
   // COL_FUNCTION(CRHgenerateCrashDumpWindows);
   static COLostreamString DumpStream;
   CRHcrashContext(DumpStream);
   CRHcrashHeader(DumpStream, Exception, GetCurrentThreadId());

   static COLmutex Mutex;     // not sure if this is strictly necessary
   {
      COLlocker Locker(Mutex);
      COLstring Error;
      if (!COLdebugInit(&Error)) {
         // failed to load debug symbols
         DumpStream << Error << newline;
      } else {
         COLbacktrace2(DumpStream);                      // reliable but current thread only
         if (CRHcrashDumpMode() != 1) {
            CRHbacktraceAllThreadsWindows(DumpStream);   // less reliable but for all threads
         }
         if (!COLdebugShutdown(&Error)) {
            // failed to unload debug symbols
            DumpStream << Error << newline;
         }
      }
   }

   CRHfinalizeCrashDumpGeneration(DumpStream);
}

static COLstring CRHexceptionName(COLuint32 Code) {
   switch (Code) {
      case EXCEPTION_ACCESS_VIOLATION:          return "Access violation";
      case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:     return "Array bounds exceeded";
      case EXCEPTION_BREAKPOINT:                return "Breakpoint";
      case EXCEPTION_DATATYPE_MISALIGNMENT:     return "Datatype misalignment";
      case EXCEPTION_FLT_DENORMAL_OPERAND:      return "Denormal floating point operand";
      case EXCEPTION_FLT_DIVIDE_BY_ZERO:        return "Floating point division by zero";
      case EXCEPTION_FLT_INEXACT_RESULT:        return "Inexact floating point result";
      case EXCEPTION_FLT_INVALID_OPERATION:     return "Floating point exception";
      case EXCEPTION_FLT_OVERFLOW:              return "Floating point overflow";
      case EXCEPTION_FLT_STACK_CHECK:           return "Stack overflow/underflow during floating point operation";
      case EXCEPTION_FLT_UNDERFLOW:             return "Floating point exponent underflow";
      case EXCEPTION_GUARD_PAGE:                return "Attempted to access a guarded page";
      case EXCEPTION_ILLEGAL_INSTRUCTION:       return "Illegal instruction";
      case EXCEPTION_IN_PAGE_ERROR:             return "Attempted to access a page which could not be loaded";
      case EXCEPTION_INT_DIVIDE_BY_ZERO:        return "Integer division by zero";
      case EXCEPTION_INT_OVERFLOW:              return "Integer overflow";
      case EXCEPTION_INVALID_HANDLE:            return "Invalid handle";
      case EXCEPTION_NONCONTINUABLE_EXCEPTION:  return "Attempted to continue after a non-continuable exception";
      case EXCEPTION_PRIV_INSTRUCTION:          return "Attempted to execute a privileged instruction";
      case EXCEPTION_STACK_OVERFLOW:            return "Stack overflow";
      case ERROR_PROCESS_ABORTED:               return "Process aborted";
      case 0xE06D7363:                          return "C++ exception";    // https://devblogs.microsoft.com/oldnewthing/20100730-00/?p=13273
      default:                                  return "Unknown exception";
   }
}

static LONG WINAPI CRHunhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionPointers) {
   COLstring Exception = CRHexceptionName(pExceptionPointers->ExceptionRecord->ExceptionCode);
   COL_VAR(Exception);
   CRHgenerateCrashDumpWindows("SEH " + Exception);
   return EXCEPTION_CONTINUE_SEARCH;
}
static void CRHcrtPureCallHandler() {
   CRHgenerateCrashDumpWindows("CRT Pure virtual function call");
   exit(1);
}
static int CRHcrtNewHandler(size_t) {
   CRHgenerateCrashDumpWindows("CRT Fail to allocate memory");
   return 0;
}


static LPTOP_LEVEL_EXCEPTION_FILTER WINAPI CRHdummyUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter) {
  return NULL;
}

static void CRHprotectUnhandledExceptionFilter() {
   // Replaces the real SetUnhandledExceptionFilter with pNewFunc.  This allows us to
   // stop 3rd party libraries from overwriting our desired unhandled exception filter
   // (e.g. Oracle OCI).
   //
   // Code adapted from Jochen Kalmach's Blog post "Unhandled exceptions in VC8 and
   // above ... for x86 and x64"
   // http://blog.kalmbach-software.de/2008/04/02/unhandled-exceptions-in-vc8-and-above-for-x86-and-x64

   COLdll Kernel32;
   Kernel32.loadLibrary("kernel32.dll");
   void *pOrgEntry = Kernel32.getProcAddress("SetUnhandledExceptionFilter");
   if (!pOrgEntry) { return; }

   SIZE_T jmpSize = 13;

   DWORD dwOldProtect = 0;
   BOOL bProt = VirtualProtect(pOrgEntry, jmpSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
   BYTE newJump[20];
   void* pNewFunc = &CRHdummyUnhandledExceptionFilter;
   newJump[0] = 0x49;  // MOV R11, ...
   newJump[1] = 0xBB;  // ...
   memcpy(&newJump[2], &pNewFunc, sizeof(pNewFunc));
   newJump[10] = 0x41;  // JMP R11, ...
   newJump[11] = 0xFF;  // ...
   newJump[12] = 0xE3;  // ...

   SIZE_T bytesWritten;
   BOOL bRet = WriteProcessMemory(GetCurrentProcess(), pOrgEntry, newJump, jmpSize, &bytesWritten);

   if (bProt != FALSE) {
      DWORD dwBuf;
      VirtualProtect(pOrgEntry, jmpSize, dwOldProtect, &dwBuf);
   }
}

static void CRHhandleSignalWindows(int Signal) {
   CRHgenerateCrashDumpWindows(CRHsignalException(Signal));
   // Prints "Press enter to finish and exit or wait 30 seconds." to stdout.
   // Not sure if this is necessary since this is only for abort.
   INSTpauseIfNotWindowsService();
   // reset segfault handler
   ::signal(Signal, SIG_DFL);
   // raise segfault
   ::raise(Signal);
}

void CRHsetUpCrashLog() {
   _set_purecall_handler(CRHcrtPureCallHandler);
   _set_new_handler(CRHcrtNewHandler);
   // capture the often more informative message for abort
   COLsetAbortCallback(&CRHsetAbortMessage);
   ::signal(SIGABRT, CRHhandleSignalWindows);
   ::SetUnhandledExceptionFilter(CRHunhandledExceptionFilter);
   CRHprotectUnhandledExceptionFilter();
}
