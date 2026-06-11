// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PROexecuteWindows
//
// Description:
//
// Implementation
//
// Author: Matthew Sobkowski
// Date:   Monday 19 January 2023
// ---------------------------------------------------------------------------
#include <COL/COLerror.h>
#include <COL/COLlog.h>
#include <COL/COLsink.h>
#include <COL/COLutils.h>
#include <PRO/PROexecute.h>
#include <windows.h>
#include <tlhelp32.h>

COL_LOG_MODULE;

bool PROexecuteOS(const COLstring& Dir, const COLstring& FullCommand, COLsink* pOutSink, COLsink* pErrSink,
                  int OutputTimeout, int* pExitCode, int Try, bool ExitOnOutput);

struct PROpipeHandles {
   HANDLE readHandle;
   HANDLE writeHandle;
};

struct PROreaderArgs {
   HANDLE   ReadHandle;
   COLsink* pSink;
   HANDLE   hOutputEvent;
   bool     ExitOnOutput;
};

COLostream& operator<<(COLostream& Stream, SECURITY_ATTRIBUTES Attrs) {
   Stream << "SECURITY: Handles inherited=" << Attrs.bInheritHandle << " Descriptor=" << Attrs.lpSecurityDescriptor
          << " len=" << Attrs.nLength;
   return Stream;
}

COLostream& operator<<(COLostream& Stream, STARTUPINFO Attrs) {
   Stream << "STARTUPINFO: Size=" << Attrs.cb << " Allow IO redirection=" << Attrs.dwFlags;
   return Stream;
}

COLostream& operator<<(COLostream& Stream, PROpipeHandles pHandle) {
   Stream << "ReadHandle=" << pHandle.readHandle << "\t WriteHandle=" << pHandle.writeHandle;
   return Stream;
}

static void PROcleanPipe(PROpipeHandles* pPipe) {
   COL_FUNCTION(PROcleanPipe);
   if(pPipe->readHandle  && pPipe->readHandle  != INVALID_HANDLE_VALUE) { ::CloseHandle(pPipe->readHandle);  pPipe->readHandle  = nullptr; }
   if(pPipe->writeHandle && pPipe->writeHandle != INVALID_HANDLE_VALUE) { ::CloseHandle(pPipe->writeHandle); pPipe->writeHandle = nullptr; }
}

static void PROcleanAll(PROCESS_INFORMATION* pProcessInfo, PROpipeHandles* pPipe1, PROpipeHandles* pPipe2,
                        HANDLE JobHandle, int ExitCode) {
   COL_FUNCTION(PROcleanAll);
   COL_VAR4(pProcessInfo, pPipe1, pPipe2, ExitCode);
   if(pProcessInfo && pProcessInfo->hProcess) {
      ::TerminateProcess(pProcessInfo->hProcess, ExitCode);
      ::WaitForSingleObject(pProcessInfo->hProcess, 5000);
      ::CloseHandle(pProcessInfo->hProcess);
   }
   if(pProcessInfo->hThread)  { ::CloseHandle(pProcessInfo->hThread);  }
   if(pPipe1)     { PROcleanPipe(pPipe1); }
   if(pPipe2)     { PROcleanPipe(pPipe2); }
   if(JobHandle)  { ::CloseHandle(JobHandle); }
}

static bool PROhandledError(const COLstring& Dir, const COLstring& Command, COLsink* pStdErr, int* pExitCode,
                            const DWORD& LastErr) {
   COL_FUNCTION(PROhandledError);
   switch(LastErr) {
      case ERROR_BAD_EXE_FORMAT:
         COL_ERR(Command + " is not an executable file");
         pStdErr->writeString(Command + " is not an executable file");
         *pExitCode = ERROR_BAD_EXE_FORMAT;
         return true;
      case ERROR_DIRECTORY:
      case ERROR_PATH_NOT_FOUND:
      case ERROR_INVALID_DRIVE:
         COL_ERR(Dir + " not found.");
         pStdErr->writeString(Dir + " not found.");
         *pExitCode = PRO_BAD_DIR;
         return true;
      case ERROR_FILE_NOT_FOUND:
         COL_ERR("The system cannot find the file specified.");
         pStdErr->writeString("The system cannot find the file specified.");
         *pExitCode = ERROR_FILE_NOT_FOUND;
         return true;
      default:
         COL_ERR("LastErr: " << LastErr);
         pStdErr->writeString(COLerrorString(LastErr));
         *pExitCode = PRO_BAD_EXIT;
         return false;
   }
}

static bool PROretry(const COLstring& Dir, const COLstring& Command, COLsink* pStdOut, COLsink* pStdErr, int Timeout,
                     int* pExitCode, int Try, const DWORD& LastErr) {
   COL_FUNCTION(PROretry);
   if(PROhandledError(Dir, Command, pStdErr, pExitCode, LastErr)) { return false; }
   COLstring Msg = "Invocation of command " + Command + " failed (" + COLerrorString(LastErr) + "). ";
   if(Try == 20) {
      COL_ERR(Msg << "Run out of retrys.");
      *pExitCode = PRO_RETRIES_EXCEEDED;
      return false;
   }
   Try++;
   COL_ERR(Msg << "Retry " << Try << " of 20");
   COLsleep(200);
   return PROexecuteOS(Dir, Command, pStdOut, pStdErr, Timeout, pExitCode, Try, false);
}

static bool PROverifyPipe(PROpipeHandles* pHandle) {
   COL_FUNCTION(PROverifyPipe);
   if(pHandle->readHandle == INVALID_HANDLE_VALUE || pHandle->readHandle == nullptr) {
      COL_ERR("Read handle failed to initialize");
      PROcleanPipe(pHandle);
      return false;
   }
   if(pHandle->writeHandle == INVALID_HANDLE_VALUE || pHandle->writeHandle == nullptr) {
      COL_ERR("Write handle failed to initialize");
      PROcleanPipe(pHandle);
      return false;
   }
   return true;
}

static DWORD WINAPI PROreaderThread(LPVOID pArg) {
   COL_FUNCTION(PROreaderThread);
   PROreaderArgs* pArgs     = (PROreaderArgs*)pArg;
   char           Buffer[4096];
   DWORD          dwBytesRead = 0;
   bool           Signaled    = false;
   while(::ReadFile(pArgs->ReadHandle, Buffer, sizeof(Buffer), &dwBytesRead, nullptr)) {
      if(dwBytesRead > 0) {
         pArgs->pSink->write(Buffer, dwBytesRead);
         if(pArgs->ExitOnOutput && !Signaled) {
            ::SetEvent(pArgs->hOutputEvent);
            Signaled = true;
         }
      }
   }
   COL_TRC("Reader thread exiting");
   return 0;
}

static void PROjoinReaders(HANDLE hThread1, HANDLE hThread2) {
   COL_FUNCTION(PROjoinReaders);
   HANDLE Threads[2] = { hThread1, hThread2 };
   DWORD  Result     = ::WaitForMultipleObjects(2, Threads, TRUE, 5000);
   if(Result == WAIT_TIMEOUT) { COL_TRC("Reader threads did not finish in time"); }
}
 
static void PROterminateOne(DWORD ProcessId, int ExitCode) {
   COL_FUNCTION(PROterminateOne);
   HANDLE ProcessHandle = ::OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, FALSE, ProcessId);
   if(!ProcessHandle) {
      COL_TRC("OpenProcess failed for PID " << ProcessId);
      return;
   }
   ::TerminateProcess(ProcessHandle, ExitCode);
   ::WaitForSingleObject(ProcessHandle, 5000);
   ::CloseHandle(ProcessHandle);
}
 
static void PROterminateChildren(DWORD ParentId, int ExitCode) {
   COL_FUNCTION(PROterminateChildren);
   HANDLE Snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
   if(Snapshot == INVALID_HANDLE_VALUE) {
      COL_TRC("Failed to create process snapshot for PID " << ParentId);
      return;
   }
   PROCESSENTRY32 Entry = {};
   Entry.dwSize = sizeof(Entry);
   if(::Process32First(Snapshot, &Entry)) {
      do {
         if(Entry.th32ParentProcessID == ParentId) {
            PROterminateChildren(Entry.th32ProcessID, ExitCode);
            COL_TRC("Terminating child process " << Entry.th32ProcessID << " for parent " << ParentId);
            PROterminateOne(Entry.th32ProcessID, ExitCode);
         }
      } while(::Process32Next(Snapshot, &Entry));
   }
   ::CloseHandle(Snapshot);
}
 
static void PROterminateTree(PROCESS_INFORMATION& ProcInfo, int ExitCode, bool JobAssigned, HANDLE JobHandle) {
   COL_FUNCTION(PROterminateTree);
   if(JobAssigned && JobHandle) {
      COL_TRC("Terminating job object for PID " << ProcInfo.dwProcessId);
      ::TerminateJobObject(JobHandle, ExitCode);
   } else {
      COL_TRC("Job assignment failed earlier, killing descendants for PID " << ProcInfo.dwProcessId);
      PROterminateChildren(ProcInfo.dwProcessId, ExitCode);
   }
   if(ProcInfo.hProcess) {
      ::TerminateProcess(ProcInfo.hProcess, ExitCode);
      ::WaitForSingleObject(ProcInfo.hProcess, 5000);
   }
}

static bool PROwait(PROCESS_INFORMATION& piProcInfo, PROpipeHandles* phStdOut, PROpipeHandles* phStdErr, int Timeout,
                    int* pExitCode, COLsink* pStdOutSink, COLsink* pStdErrSink, bool ExitOnOutput, bool JobAssigned,
                    HANDLE JobHandle) {
   COL_FUNCTION(PROwait);
   COL_TRC("ExitOnOutput=" << ExitOnOutput << " Timeout=" << Timeout);

   HANDLE hOutputEvent = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);
   if(!hOutputEvent) {
      DWORD dwError = ::GetLastError();
      COL_ERR("CreateEvent failed: " << COLerrorString(dwError));
      *pExitCode = PRO_BAD_EXIT;
      return false;
   }

   PROreaderArgs StdOutArgs = { phStdOut->readHandle, pStdOutSink, hOutputEvent, ExitOnOutput };
   PROreaderArgs StdErrArgs = { phStdErr->readHandle, pStdErrSink, hOutputEvent, ExitOnOutput };

   HANDLE hStdOutThread = ::CreateThread(nullptr, 0, PROreaderThread, &StdOutArgs, 0, nullptr);
   HANDLE hStdErrThread = ::CreateThread(nullptr, 0, PROreaderThread, &StdErrArgs, 0, nullptr);

   if(!hStdOutThread || !hStdErrThread) {
      DWORD dwError = ::GetLastError();
      COL_ERR("Failed to create reader threads: " << COLerrorString(dwError));
      PROterminateTree(piProcInfo, 0, JobAssigned, JobHandle);
      *pExitCode = PRO_BAD_EXIT;
      if(hStdOutThread) { ::WaitForSingleObject(hStdOutThread, 5000); ::CloseHandle(hStdOutThread); }
      if(hStdErrThread) { ::WaitForSingleObject(hStdErrThread, 5000); ::CloseHandle(hStdErrThread); }
      ::CloseHandle(hOutputEvent);
      return false;
   }

   HANDLE WaitHandles[2] = { piProcInfo.hProcess, hOutputEvent };
   DWORD  nHandles       = ExitOnOutput ? 2 : 1;
   DWORD  dwTimeoutMs    = (Timeout < 0) ? INFINITE : (DWORD)((ULONGLONG)Timeout * 1000);

   DWORD dwWaitResult = ::WaitForMultipleObjects(nHandles, WaitHandles, FALSE, dwTimeoutMs);
   COL_TRC("WaitResult=" << dwWaitResult);

   bool Success = false;
   if(dwWaitResult == WAIT_OBJECT_0) {
      DWORD dwExit = 0;
      ::GetExitCodeProcess(piProcInfo.hProcess, &dwExit);
      COL_DBG("ProcessExitCode=" << dwExit);
      *pExitCode = dwExit;
      Success    = true;
   } else if(dwWaitResult == WAIT_OBJECT_0 + 1) {
      COL_TRC("ExitOnOutput triggered, terminating process");
      PROterminateTree(piProcInfo, 0, JobAssigned, JobHandle);
      *pExitCode = 0;
      Success    = true;
   } else if(dwWaitResult == WAIT_TIMEOUT) {
      COL_TRC("Process timed out");
      PROterminateTree(piProcInfo, PRO_TIMED_OUT, JobAssigned, JobHandle);
      *pExitCode = PRO_TIMED_OUT;
      Success    = false;
   } else {
      DWORD dwError = ::GetLastError();
      COL_ERR("WaitForMultipleObjects failed with error " << dwError);
      PROterminateTree(piProcInfo, PRO_BAD_EXIT, JobAssigned, JobHandle);
      *pExitCode = PRO_BAD_EXIT;
      Success    = false;
   }

   PROjoinReaders(hStdOutThread, hStdErrThread);
   ::CloseHandle(hStdOutThread);
   ::CloseHandle(hStdErrThread);
   ::CloseHandle(hOutputEvent);
   return Success;
}

static PROpipeHandles PROcreateAnonymousPipe() {
   COL_FUNCTION(PROcreateAnonymousPipe);
   PROpipeHandles      PipeHandles = { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE };
   SECURITY_ATTRIBUTES SecAttr     = {};
   SecAttr.nLength                 = sizeof(SECURITY_ATTRIBUTES);
   SecAttr.bInheritHandle          = TRUE;

   if(!::CreatePipe(&PipeHandles.readHandle, &PipeHandles.writeHandle, &SecAttr, 0)) {
      const DWORD dwLastErr = GetLastError();
      COL_ERR("Pipe creation failed -- " << COLerrorString(dwLastErr));
      return PipeHandles;
   }

   ::SetHandleInformation(PipeHandles.readHandle, HANDLE_FLAG_INHERIT, 0);
   return PipeHandles;
}

bool PROexecuteOS(const COLstring& Dir, const COLstring& Command, COLsink* pStdOut, COLsink* pStdErr, int Timeout,
                  int* pExitCode, int Try, bool ExitOnOutput) {
   COL_FUNCTION(PROexecuteOS);
   COL_VAR3(Dir, Command, Try);
   *pExitCode = PRO_BAD_EXIT;

   HANDLE JobHandle = ::CreateJobObjectA(NULL, NULL);
   if(JobHandle) {
      JOBOBJECT_EXTENDED_LIMIT_INFORMATION JobLimit = {};
      JobLimit.BasicLimitInformation.LimitFlags     = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
      if(!::SetInformationJobObject(JobHandle, JobObjectExtendedLimitInformation, &JobLimit, sizeof(JobLimit))) {
         COL_TRC("Failed to set job object limit flags");
      }
   } else {
      COL_TRC("Failed to create job object");
   }

   PROpipeHandles StdOutPipe = PROcreateAnonymousPipe();
   PROpipeHandles StdErrPipe = PROcreateAnonymousPipe();
   COL_VAR2(StdOutPipe, StdErrPipe);

   if(!PROverifyPipe(&StdOutPipe) || !PROverifyPipe(&StdErrPipe)) {
      COL_ERR("Error creating pipes -- retrying");
      PROcleanPipe(&StdOutPipe);
      PROcleanPipe(&StdErrPipe);
      if(JobHandle) { ::CloseHandle(JobHandle); }
      return PROretry(Dir, Command, pStdOut, pStdErr, Timeout, pExitCode, Try, -1);
   }

   PROCESS_INFORMATION pi = {};
   STARTUPINFO         si = {};
   si.cb                  = sizeof(si);
   si.dwFlags             = STARTF_USESTDHANDLES;
   si.hStdOutput          = StdOutPipe.writeHandle;
   si.hStdError           = StdErrPipe.writeHandle;

   LPSTR pDir = Dir.is_null() ? nullptr : (LPSTR)Dir.c_str();

   COL_DBG("Attempting to create process...");
   if(!CreateProcessA(NULL, (LPSTR)Command.c_str(), NULL, NULL, TRUE, CREATE_SUSPENDED | CREATE_NEW_PROCESS_GROUP, NULL,
                      pDir, &si, &pi)) {
      DWORD dwLastErr = ::GetLastError();
      COL_TRC("CreateProcess failed with error " << dwLastErr << "\t" << COLerrorString(dwLastErr));
      PROcleanAll(&pi, &StdOutPipe, &StdErrPipe, JobHandle, dwLastErr);
      return PROretry(Dir, Command, pStdOut, pStdErr, Timeout, pExitCode, Try, dwLastErr);
   }
   bool JobAssigned = false;

   if(JobHandle) {
      if(!::AssignProcessToJobObject(JobHandle, pi.hProcess)) {
         DWORD JobErr = ::GetLastError();
         COL_TRC("Failed to assign process to job object: " << JobErr << " " << COLerrorString(JobErr));
      } else {
         JobAssigned = true;
      }
   }

   ::ResumeThread(pi.hThread);

   ::CloseHandle(StdOutPipe.writeHandle);
   StdOutPipe.writeHandle = nullptr;
   ::CloseHandle(StdErrPipe.writeHandle);
   StdErrPipe.writeHandle = nullptr;

   bool Success = PROwait(pi, &StdOutPipe, &StdErrPipe, Timeout, pExitCode, pStdOut, pStdErr, ExitOnOutput, JobAssigned, JobHandle);

   PROcleanAll(&pi, &StdOutPipe, &StdErrPipe, JobHandle, *pExitCode);

   COL_VAR2(Success, *pExitCode);
   return Success;
}
