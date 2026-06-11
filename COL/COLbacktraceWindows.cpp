//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLbacktrace
//
// Description:
//
// Window implementation.
//
// Author: Eliot Muir
// Date:   Wed 09/09/1998
//---------------------------------------------------------------------------
#include <COL/COLwindows.h>

#include <dbghelp.h>
#include <stdio.h>

#include <COL/COLerror.h>
#include <COL/COLlocker.h>
#include <COL/COLmutex.h>
#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLmutex s_COLbackTraceMutex;

bool COLdebugShutdown(COLstring* pError){
   COL_FUNCTION(COLdebugShutdown);
   HANDLE Process = GetCurrentProcess();
   BOOL Success = SymCleanup(Process);
   if (!Success){
      long Err = ::GetLastError();
      *pError = "Error unloading symbols:" + COLerrorString(Err) + " Code = " + Err;
      COL_VAR(*pError);
      return false;
   }
   return true;
}

bool COLdebugInit(COLstring* pError){
   COL_FUNCTION(COLdebugInit);
   COLlocker Locker(s_COLbackTraceMutex);
   HANDLE Process = GetCurrentProcess();
   COL_VAR(Process);
   DWORD Options = SymSetOptions(SYMOPT_EXACT_SYMBOLS);
   COL_VAR(Options);
   COLstring SearchPath;
   if (!COLdebugSearchPath().is_null()){
      SearchPath = COLdebugSearchPath();
   }
   COL_VAR(SearchPath);
   BOOL Success = SymInitialize(Process, SearchPath.c_str(), TRUE);
   if (!Success){
      long Err = ::GetLastError();
      *pError = "Error loading symbols:" + COLerrorString(Err) + " Code = " + Err;
      COL_VAR(*pError);
      return true;
   }
   COL_VAR(Success);
   COL_TRC("Successfully loaded symbols.");
   return true;
}

COLstring COLbacktrace(){
   COLlocker Locker(s_COLbackTraceMutex);
   COLostreamString bt;
   COLstring Error;
   if (!COLdebugInit(&Error)){
      bt << "\nStack trace not enabled " << COLdateTime::currentTime() << '\n';
      return bt.str();
   }

   bt << "\nStack trace captured " << COLdateTime::currentTime() << '\n';
   HANDLE Process = GetCurrentProcess();
   COL_VAR(Process);
   BOOL Success;
   // there could be more than 128 frames on the stack, but 128 should work in most cases
   void* Stack[128];
   int Frames = RtlCaptureStackBackTrace(0, 128, Stack, NULL);
   COL_VAR(Frames);

   char Buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
   SYMBOL_INFO* pSymbol = (SYMBOL_INFO*)Buffer;
   pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
   pSymbol->MaxNameLen = MAX_SYM_NAME;
   for (int i = 0; i < Frames; ++i) {
      Success = SymFromAddr(Process, (DWORD64)Stack[i], NULL, pSymbol);
      if (!Success) {
         long Err = ::GetLastError();
         COL_TRC("Error resolving symbol " << i << ": " << COLerrorString(Err));
      }
      bt << i << "\t0x" << pSymbol->Address << "\t" << pSymbol->Name << '\n';
   }
   bool SuccessShutdown = COLdebugShutdown(&Error);
   COL_VAR2(SuccessShutdown, Error);
   return bt.str();
}

void COLbacktrace2(COLostreamString& bt) {
   // there could be more than 128 frames on the stack, but 128 should work in most cases
   void* Stack[128];
   int Frames = RtlCaptureStackBackTrace(0, 128, Stack, NULL);
   char Buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)] = {0};
   SYMBOL_INFO* pSymbol = (SYMBOL_INFO*)Buffer;
   pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
   pSymbol->MaxNameLen = MAX_SYM_NAME;
   DWORD64 displacement = 0;
   for (int i = 0; i < Frames; ++i) {
      if (!SymFromAddr(GetCurrentProcess(), (DWORD64)Stack[i], &displacement, pSymbol)) {
         // long Err = ::GetLastError();
         // COL_TRC("Error resolving symbol " << i << ": " << COLerrorString(Err));
         bt << i << "\t0x" << pSymbol->Address << "\t0x" << displacement << "\t\t[Unknown Symbol]\n";
      } else {
         bt << i << "\t0x" << pSymbol->Address << "\t0x" << displacement << "\t\t" << pSymbol->Name << "\n";
      }
   }
   bt << newline;
}
