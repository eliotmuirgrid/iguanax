//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: CRHcrash
//
// Author: Eliot Muir
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------

#include <signal.h>

#include <COL/COLerror.h>
#include <COL/COLioCheck.h>
#include <COL/COLlog.h>
#include <CRH/CRHcrash.h>
#include <DBDVER/DBDVERgitInfo.h>
#include <DBDVER/DBDVERversion.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GITU/GITUconfigGlobal.h>
#include <INFO/INFOusage.h>
#include <INI/INIdata.h>
#include <IWEB/IWEBconfig.h>
#include <IWEB/IWEBload.h>
COL_LOG_MODULE;

COLstring CRHpidFileName (){
   COL_FUNCTION(CRHpidFileName);
   COLstring PidFile= FILexecutableDir();
   FILaddPathSeparator(PidFile);
   PidFile += "debug.pid";
   return PidFile;
}

// return false if no core dump found from previous run
bool CRHfindLastDump(COLstring* pFileName){
   COL_FUNCTION(CRHfindLastDump);
   COLstring PidFile = CRHpidFileName();
   if (FILfileExists(PidFile) != true){
      return false;
   }
   COLstring Content;
   FILreadFile(PidFile, &Content);
   COLstring CoreDumpFile = "/cores/core." + Content;
   *pFileName = CoreDumpFile;
   bool DumpPresent = FILfileExists(CoreDumpFile);
   COL_VAR2(DumpPresent,CoreDumpFile);
   return DumpPresent;
}


static void CRHgitCredentials(COLstring* pName, COLstring* pEmail){
   COL_FUNCTION(CRHgitCredentials);
   INIdata Config;
   if(!GITUconfigLoadUser("admin", &Config)){
      COL_TRC("No 'admin' user credentials to load.");
      return;
   };
   *pName  = GITUconfigUserName(Config);
   *pEmail = GITUconfigUserEmail(Config);
   COL_VAR2(*pName, *pEmail);
}

static COLstring CRHwebServerDescription(){
   COL_FUNCTION(CRHwebServerDescription);
   IWEBconfig Config;
   COLstring Error;
   IWEBload(&Config, &Error);
   return Config.serverDescription();
}

static COLstring CRHgetCurrentTimeFormatted(const char* Format = "%Y%m%d-%H%M%S"){
   COL_FUNCTION(CRHgetCurrentTimeFormatted);
   time_t t = time(NULL);
   struct tm *tm_info = localtime(&t);
   char Buffer[200];
   int Size = strftime(Buffer, sizeof(Buffer), Format, tm_info);
   return COLstring(Buffer, Size);
}

static COLstring g_AbortMessage;

void CRHsetAbortMessage(const COLstring& Message) {
   g_AbortMessage = Message;
}

static const COLstring& CRHgetAbortMessage() {
   return g_AbortMessage;
}

COLstring CRHsignalException(int Signal) {
   // return the captured message for abort if applicable
   if (SIGABRT == Signal && !CRHgetAbortMessage().is_null()) {
      return CRHgetAbortMessage();
   }
   switch (Signal) {
      case SIGSEGV:  return "a SIGSEGV error";                                break;
      case SIGABRT:  return "a SIGABRT error";                                break;
#ifndef _WIN32
      case SIGBUS:   return "a SIGBUS error";                                 break;
#endif
      default:       return "a Signal " + COLintToString(Signal) + " error";  break;
   }
}

static void CRHcollectGitInfo(COLostreamString& Stream) {
   COLstring Name  = "N/A - admin user credentials are not set up";
   COLstring Email = "N/A - admin user credentials are not set up";
   CRHgitCredentials(&Name, &Email);
   Stream << "Email: "            << Email                     << newline;
   Stream << "Name: "             << Name                      << newline;
}

static void CRHcollectServerInfo(COLostreamString& Stream) {
   // TODO: OS is useless, better to get uname and arm vs intel iguana binary
   Stream << "OS: "               << CRHgetOS()                << newline;
   Stream << "Server : "          << CRHwebServerDescription() << newline;
}

static COLstring CRHiguanaArchitecture() {
// Windows defines | Linux defines | Mac define
#if defined(_M_ARM64) || defined(_M_ARM) || defined(__aarch64__) || defined(__arm__) || defined(__arm64__)
   return "ARM";
#else
   return "x64";
#endif
}

static void CRHcollectBuildInfo(COLostreamString& Stream) {
   Stream << newline;
   Stream << "Commit Id: "        << DBDVER_COMMIT_ID          << newline;
   Stream << "Commit Timestamp: " << DBDVER_TIME_STAMP         << newline;
   Stream << "Commit Message: "   << DBDVER_COMMIT_MSG         << newline;
   Stream << "Version: "          << DBD_STRING_VERSION        << newline;
   Stream << "Architecture: "     << CRHiguanaArchitecture()   << newline;
}

static void CRHcollectSystemInfo(COLostreamString& Stream) {
   int        Handle = INFOfileHandleCount();
   double     CPU    = INFOcpuPercent();
   INFOdisk   Disk   = INFOdiskStats();
   INFOmemory Memory = INFOmemoryStats();
   static COLfloat64 GB = 1000000000.0;
   Stream << newline;
   Stream << "CPU : " << CPU << " %" << newline;
   Stream << "Open File : " << Handle << newline;
   Stream << "Disk (Free / Total) : " << Disk.Free/GB << " GB / " << Disk.Size/GB << " GB" << newline;
   Stream << "Memory (Resident | Physical | Virtual) : " << Memory.Resident/GB << " GB | " << Memory.Physical/GB << " GB | " << Memory.Virtual/GB << " GB" << newline;
}

void CRHcrashContext(COLostreamString& Stream) {
   CRHcollectGitInfo(Stream);
   CRHcollectServerInfo(Stream);
   CRHcollectBuildInfo(Stream);
   CRHcollectSystemInfo(Stream);
}

void CRHcrashHeader(COLostreamString& DumpStream, const COLstring& Exception, int ThreadId) {
   DumpStream << newline;
   DumpStream << "IguanaX terminated due to " << Exception << newline << newline;
   DumpStream << "Stack trace captured on " << CRHgetCurrentTimeFormatted("%A, %B %d, %Y @ %I:%M:%S %p") << newline;
   DumpStream << "COLbacktrace: " << (void*)&COLbacktrace << newline << newline;
   DumpStream << "Thread " << ThreadId << " CRASHED" << newline;
}

void CRHappendToServiceLog(const COLstring& Dump) {
   // COL_FUNCTION(CRHappendToServiceLog);
   COLstring SEL = FILpathAppend(FILworkingDir(), "ServiceErrorLog.txt");
   FILappendFile(SEL, Dump);
}

static void CRHsaveCrashDump(const COLstring& Dump) {
   // COL_FUNCTION(CRHsaveCrashDump);
   COLclearMainThread();
   COLstring FilePath = DIRcrashDir();
   if (!FILfileExists(FilePath)) {
      FILmakeFullDir(FilePath, FIL_USER_RWX);
   }
   FilePath = FILpathAppend(FilePath, CRHgetCurrentTimeFormatted() + ".crash");
   FILwriteFile(FilePath, Dump);
   COLcout << Dump << "Saved to " << FilePath << newline << flush;
}

void CRHfinalizeCrashDumpGeneration(COLostreamString& DumpStream) {
   // COL_FUNCTION(CRHfinalizeCrashDumpGeneration);
   const COLstring& Dump = DumpStream.str();
   CRHsaveCrashDump(Dump);
   CRHappendToServiceLog(Dump);
}

int CRHcrashDumpMode() {
   // COL_FUNCTION(CRHcrashDumpMode);
   int Mode = 0;     // collect call stack for all threads
   const char* pValue = getenv("IFW_CRASH_DUMP_MODE");
   if (pValue) {
      int Value = atoi(pValue);
      switch (Value) {
         case 1  :   Mode = 1;   break;   // current thread call stack only
         default :   Mode = 0;   break;   // all threads
      }
   }
   return Mode;
}
