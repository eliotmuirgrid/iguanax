//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PROutils
//
// Description:
//
// Implementation
//
// Author: Rishi Marwah
// Date:   Tue 09/01/1998 
//---------------------------------------------------------------------------

#ifdef _WIN32
#   include <COL/COLdll.h>
#   include <COL/COLwindows.h>
#   include <process.h>
#else
#   include <sys/types.h>
#   include <signal.h>
#   include <unistd.h>
#   include <limits.h>
#endif

#include <errno.h>
#include <stdlib.h>

#include <sys/stat.h>

#include "PROutils.h"
#include "PROptrArray.h"

#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLmath.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


#include "PROutils.h"

bool PROisProcessRunning (int ProcessID) {
#ifndef _WIN32
   return ::kill((pid_t)ProcessID,0)==0; //null signal
#else
   typedef HANDLE (__stdcall *COLopenProcess)(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId);
   typedef BOOL (__stdcall *COLcloseHandle)(HANDLE hObject);
  
   // TODO - just load once?
   COLdll Dll;
   Dll.loadLibrary("kernel32.dll");
   COLopenProcess pOpenFunction = (COLopenProcess)Dll.getProcAddress("OpenProcess");
   COLcloseHandle pCloseFunction = (COLcloseHandle)Dll.getProcAddress("CloseHandle");
   if (pOpenFunction==NULL || pCloseFunction == NULL) {
      COL_ERROR_STREAM_PLAIN ("Error: You must be running Windows 2000 or later.",COLerror::PreCondition);
   }
   HANDLE hProcess =  (*pOpenFunction)(PROCESS_VM_READ,false,ProcessID);
   if (hProcess!=NULL) {
      (*pCloseFunction)(hProcess);
      return true;
   }
   return false;
#endif
}

void PROtraceInit(const COLstring& Env){
   char* pEnv = getenv(Env.c_str());
   if (!pEnv) { return;
   }
   COLstring LogCommandLine = pEnv;
   if (!LogCommandLine.is_null()) {
      COLarray<COLstring> Arguments;
      PROparseCommandLine("dummy " + LogCommandLine, &Arguments);

      PROptrArray<char> Argv;
      PRObuildPosixArgvArray(Arguments, &Argv);

      COL_LOG_INIT(Argv.size(), (const char**)Argv.array());
      COL_FUNCTION(PROtraceInit);
      COL_VAR(LogCommandLine);
   }
}

static COLauto<COLclosure0> s_pOnClose;

#ifdef _WIN32
static BOOL WINAPI PROcontrolledProcessCtrlCHandler(DWORD dwCtrlType){
   COL_FUNCTION(PROcontrolledProcessCtrlCHandler);
   COL_VAR(dwCtrlType);
   if (s_pOnClose.get() && (dwCtrlType == CTRL_BREAK_EVENT || dwCtrlType == CTRL_C_EVENT)){
      COL_TRC("It was a CTRL_BREAK_EVENT or CTRL_C_EVENT");
      try{
         s_pOnClose->run();
      }
      catch (...){
         COL_ERR("We got an unhandled exception.");
         //swallow exceptions since its a c call
      }
      return true;
   }
   return false;
}

void PROsetOnKill(COLclosure0* pOnCloseEvent){
   COL_FUNCTION(PROsetOnKill);
   s_pOnClose = pOnCloseEvent;
   COL_TRC("Set console ctrl handler");
   BOOL Enable = pOnCloseEvent ? true : false;
   if (!SetConsoleCtrlHandler(PROcontrolledProcessCtrlCHandler, Enable)){ // handler function
      COL_THROW_WIN32_ERROR("Could not set ctrl handler");
   }
}

#else
static void PROcontrolledProcessSigTermHandler(int Signal){
   COL_FUNCTION(PROcontrolledProcessSigTermHandler);
   COL_VAR(Signal);
   s_pOnClose->run();
}

void PROsetOnKill(COLclosure0* pOnCloseEvent){
   COL_FUNCTION(PROsetOnKill);
   s_pOnClose = pOnCloseEvent;
   COL_TRC("Setting Handler For SIGTERM");
   void (*pInterrupt) (int) = pOnCloseEvent ? &PROcontrolledProcessSigTermHandler : NULL;
   if (::signal(SIGTERM, pInterrupt) == SIG_ERR) { COL_THROW_STRERROR_PLAIN("Cannot set sigterm handler"); }
   COL_TRC("Setting Handler For SIGINT");
   if (::signal(SIGINT, pInterrupt) == SIG_ERR) { COL_THROW_STRERROR_PLAIN("Cannot set sigint handler"); }
}

#endif // _WIN32

#ifdef COL_ENABLE_TRACING
// Works with both COLarray and PROptrArray.
#define PRO_VAR_VECTOR(V){\
   for (int VectorIndex = 0; VectorIndex < (V).size(); ++VectorIndex ){\
      COL_TRC("("#V")[" << VectorIndex << "] = " << (V)[VectorIndex]);\
   }\
}
#else
#define PRO_VAR_VECTOR(V)
#endif

static bool PROisWhitespace(char Char);
static void PROparseCommandLineWhitespace(const char** ppCommandLinePos);
static void PROparsePosixCommandLineArgument(const char** ppCommandLinePos, COLstring* pArgumentOut);
COLstring PROescapePosixCommandLineArgument(const COLstring& Argument);
static const char* PROfindPosixFileNamePosition(const COLstring& Command);
static void PROparseWindowsCommandLineArgument(const char** ppCommandLinePos, COLstring* pArgumentOut);
static COLstring PROescapeWindowsCommandLineArgument(const COLstring& Argument);

void PROparseCommandLine(const COLstring& CommandLine, COLarray<COLstring>* pArgumentsOut){
#ifdef _WIN32
   PROparseWindowsCommandLine(CommandLine, pArgumentsOut);
#else
   PROparsePosixCommandLine(CommandLine, pArgumentsOut);
#endif
}

COLstring PRObuildCommandLine(const COLarray<COLstring>& Arguments){
#ifdef _WIN32
   return PRObuildWindowsCommandLine(Arguments);
#else
   return PRObuildPosixCommandLine(Arguments);
#endif
}

static bool PROisWhitespace(char Char){
   switch (Char){
   case ' ':
   case '\t':
   case '\r':
   case '\n':
      return true;
   default:
      return false;
   }
}

static void PROparseCommandLineWhitespace(const char** ppCommandLinePos){
   while (PROisWhitespace(**ppCommandLinePos)){
      ++(*ppCommandLinePos);
   }
}

static void PROparseCommandLineChar(const char** ppCommandLinePos, COLstring* pArgumentOut){
   char Char = **ppCommandLinePos;
   if (Char >= 0){
      (*pArgumentOut) += Char;
      ++(*ppCommandLinePos);
   } else{
      unsigned char* pCommandLinePos = (unsigned char*)*ppCommandLinePos;
      const int MultiByteLength = COLmath::multiByteLength(pCommandLinePos);
      for (int i = 0; i < MultiByteLength; ++i){
         (*pArgumentOut) += **ppCommandLinePos;
         ++(*ppCommandLinePos);
      }
   }
}

static void PROparsePosixCommandLineArgument(const char** ppCommandLinePos, COLstring* pArgumentOut){
   COLASSERT(ppCommandLinePos);
   COLASSERT(*ppCommandLinePos);
   COLASSERT(**ppCommandLinePos);
   COLASSERT(!PROisWhitespace(**ppCommandLinePos));
   COLASSERT(pArgumentOut);

   pArgumentOut->clear();
   bool InQuotes = false;
   for (char Char = **ppCommandLinePos; Char != 0; Char = **ppCommandLinePos){
      switch (Char){
      case ' ':
      case '\t':
      case '\r':
      case '\n':
         if (InQuotes){
            (*pArgumentOut) += Char;
            ++(*ppCommandLinePos);
         } else {
            return;
         }
         break;
         
      case '"':
         InQuotes = !InQuotes;
         ++(*ppCommandLinePos);
         break;
      case '\\':
         Char = *(++(*ppCommandLinePos));
         if (InQuotes) {
            switch (Char) {
            case '\\':
            case '"':
               break;
            default:
               (*pArgumentOut) += '\\';
               break;
            }
         }

         if (Char) {
            PROparseCommandLineChar(ppCommandLinePos, pArgumentOut);
         } else {
            return;
         }
         break;
      default:
         PROparseCommandLineChar(ppCommandLinePos, pArgumentOut);
         break;
      }
   }
}

void PROparsePosixCommandLine(const COLstring& CommandLine, COLarray<COLstring>* pArgumentsOut) {
   COL_FUNCTION(PROparsePosixCommandLine);
   COL_VAR(CommandLine);
   COLASSERT(pArgumentsOut);
   pArgumentsOut->clear();
   const char* pCommandLinePos = CommandLine.c_str();
   PROparseCommandLineWhitespace(&pCommandLinePos);
   while (*pCommandLinePos) {
      pArgumentsOut->push_back();
      PROparsePosixCommandLineArgument(&pCommandLinePos, &pArgumentsOut->back());
      PROparseCommandLineWhitespace(&pCommandLinePos);
   }
   PRO_VAR_VECTOR(*pArgumentsOut);
}

COLstring PROescapePosixCommandLineArgument(const COLstring& Argument){
   COLstring EscapedArgument;
   EscapedArgument.setCapacity(Argument.size());
   bool NeedsQuotes = false;
   const char* pChar = Argument.c_str();
   if (pChar) {
      for (char Char = *pChar; Char; Char = *(++pChar)) {
         switch (Char) {
         case '"':
         case '\\':
            EscapedArgument += '\\';
            break;
         case ' ':
         case '\t':
         case '\r':
         case '\n':
            // We could just escape whitespace, but quoting is prettier.
            NeedsQuotes = true;
            break;
         default:
            break;
         }

         EscapedArgument += Char;
      }
   }

   if (NeedsQuotes || EscapedArgument.is_null()){ // empty arg becomes a pair of double quotes
      EscapedArgument.prepend("\"");
      EscapedArgument.append("\"");
   }
   return EscapedArgument;
}

COLstring PRObuildPosixCommandLine(const COLarray<COLstring>& Arguments){
   COL_FUNCTION(PRObuildPosixCommandLine);
   PRO_VAR_VECTOR(Arguments);
   COLstring CommandLineOut;
   const int CountOfArgument = Arguments.size();
   for (int i = 0; i < CountOfArgument; ++i) {
      if (i > 0) {
         CommandLineOut += ' ';
      }
      CommandLineOut += PROescapePosixCommandLineArgument(Arguments[i]);
   }
   COL_VAR(CommandLineOut);
   return CommandLineOut;
}

static const char* PROfindPosixFileNamePosition(const COLstring& Command) {
   size_t LastSlashIndex = Command.find_last_of("\\/");
   if (LastSlashIndex != npos) {
      return Command.c_str() + LastSlashIndex + 1;
   } else {
      return Command.c_str();
   }
}

void PRObuildPosixArgvArray(const COLarray<COLstring>& Arguments, PROptrArray<char>* pArgvArrayOut){
   COL_FUNCTION(PRObuildArgvArray);
   PRO_VAR_VECTOR(Arguments);
   COLASSERT(pArgvArrayOut);
   const int CountOfArgument = Arguments.size();
   pArgvArrayOut->resize(CountOfArgument);

   if (CountOfArgument > 0){
      // Special treatement for the command string.
      (*pArgvArrayOut)[0] = const_cast<char*>(PROfindPosixFileNamePosition(Arguments[0]));
      for (int i = 1; i < CountOfArgument; ++i) {
         (*pArgvArrayOut)[i] = const_cast<char*>(Arguments[i].c_str());
      }
   }
   PRO_VAR_VECTOR(*pArgvArrayOut);
}

static void PROparseWindowsCommandLineArgument(const char** ppCommandLinePos, COLstring* pArgumentOut){
   COLASSERT(ppCommandLinePos);
   COLASSERT(*ppCommandLinePos);
   COLASSERT(**ppCommandLinePos);
   COLASSERT(!PROisWhitespace(**ppCommandLinePos));
   COLASSERT(pArgumentOut);
   pArgumentOut->clear();
   bool InQuotes = false;
   int CountOfBackSlash = 0;
   for (char Char = **ppCommandLinePos; Char != 0; Char = **ppCommandLinePos){
      switch (Char){
      case ' ':
      case '\t':
         for (; CountOfBackSlash > 0; --CountOfBackSlash) {
            (*pArgumentOut) += '\\'; // Literal backslashes
         }
         if (InQuotes){
            (*pArgumentOut) += Char;
            ++(*ppCommandLinePos);
         } else {
            return;
         }
         break;
      case '"':
         for (; CountOfBackSlash > 1; CountOfBackSlash -= 2){
            (*pArgumentOut) += '\\'; // One backslash for every pair
         }

         if (CountOfBackSlash == 0){
            InQuotes = !InQuotes; // Even -> double quote is delimiter
         } else {
            (*pArgumentOut) += '"'; // Odd -> double quote is literal
            CountOfBackSlash = 0;
         }
         ++(*ppCommandLinePos);
         break;
      case '\\':
         // Append later, once we find if trailing char is double quotes
         ++CountOfBackSlash; 
         ++(*ppCommandLinePos);
         break;
      default:
         for (; CountOfBackSlash > 0; --CountOfBackSlash) {
            (*pArgumentOut) += '\\'; // Literal backslashes
         }
         PROparseCommandLineChar(ppCommandLinePos, pArgumentOut);
         break;
      }
   }

   for (; CountOfBackSlash > 0; --CountOfBackSlash) {
      (*pArgumentOut) += '\\'; // Literal backslashes
   }
}

void PROparseWindowsCommandLine(const COLstring& CommandLine, COLarray<COLstring>* pArgumentsOut) {
   COL_FUNCTION(PROparseWindowsCommandLine);
   COL_VAR(CommandLine);
   COLASSERT(pArgumentsOut);
   pArgumentsOut->clear();
   const char* pCommandLinePos = CommandLine.c_str();
   PROparseCommandLineWhitespace(&pCommandLinePos);
   while (*pCommandLinePos) {
      pArgumentsOut->push_back();
      PROparseWindowsCommandLineArgument(&pCommandLinePos, &pArgumentsOut->back());
      PROparseCommandLineWhitespace(&pCommandLinePos);
   }
   PRO_VAR_VECTOR(*pArgumentsOut);
}

static COLstring PROescapeWindowsCommandLineArgument(const COLstring& Argument) {
   COLstring EscapedArgument;
   EscapedArgument.setCapacity(Argument.size());
   bool NeedsQuotes = false;
   int CountOfBackSlash = 0;
   const char* pChar = Argument.c_str();
   if (pChar) {
      for (char Char = *pChar; Char; Char = *(++pChar)) {
         switch (Char) {
         case '"':
            for (; CountOfBackSlash > 0; --CountOfBackSlash) {
               EscapedArgument += '\\'; // escape each backslash
            }
            EscapedArgument += '\\'; // escape the double quote
            break;
         case '\\':
            ++CountOfBackSlash;
            break;
         case ' ':
         case '\t':
         case '\r':
         case '\n':
            NeedsQuotes = true;
            break;
         default:
            break;
         }
         if (Char != '\\') {
            CountOfBackSlash = 0;
         }
         EscapedArgument += Char;
      }
   }

   if (NeedsQuotes || EscapedArgument.is_null()) {
      for (; CountOfBackSlash > 0; --CountOfBackSlash) {
         EscapedArgument += '\\'; // escape each backslash (but not the double quote appended below)
      }
      EscapedArgument.prepend("\"");
      EscapedArgument.append("\"");
   }
   return EscapedArgument;
}

COLstring PRObuildWindowsCommandLine(const COLarray<COLstring>& Arguments) {
   COL_FUNCTION(PRobuildWindowsCommandLine);
   PRO_VAR_VECTOR(Arguments);
   COLstring CommandLineOut;
   const int CountOfArgument = Arguments.size();
   for (int i = 0; i < CountOfArgument; ++i) {
      if (i > 0) {
         CommandLineOut += ' ';
      }
      CommandLineOut += PROescapeWindowsCommandLineArgument(Arguments[i]);
   }
   COL_VAR(CommandLineOut);
   return CommandLineOut;
}

int PROpid(){
   COL_FUNCTION(PROpid);
#ifdef _WIN32
   int Pid = _getpid();
#else
   int Pid = getpid();
#endif
   COL_VAR(Pid);
   return Pid;
}
