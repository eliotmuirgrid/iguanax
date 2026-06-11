//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLbacktrace
//
// Description:
// POSIX Implementation of COLbacktrace
//
// Author: Eliot Muir
// Date:   Wed 09/09/1998
//---------------------------------------------------------------------------
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <stdlib.h>

#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring COLbacktrace(){
   const int MaxFrames = 250;
   void* CallStack[MaxFrames];
   COLostreamString bt;
   bt << "\nStack trace captured " << COLdateTime::currentTime() << '\n';
   // NOTE: This line is required for stripped relocated binaries
   // to sync stack addresses with addrline on Linux and atos on Mac.
   bt << "COLbacktrace: " << (void*)&COLbacktrace << '\n';
   const int StackDepth = ::backtrace(CallStack, MaxFrames);
   if (char** SymbolArray = ::backtrace_symbols(CallStack, StackDepth)) {
      for (int i = 0; i < StackDepth; i++) {
         if (const char* Symbol = SymbolArray[i]) {
            bt << Symbol << '\n';
         } else {
            bt << i << ": " << CallStack[i] << '\n';
         }
      }
      ::free(SymbolArray);
   } else {
      for (int i = 0; i < StackDepth; i++) {
         bt << i << ": " << CallStack[i] << '\n';
      }
   }
   return bt.str();
}

void COLbacktrace2(COLostreamString& bt){
   const int MaxFrames = 250;
   void* CallStack[MaxFrames];
   const int StackDepth = ::backtrace(CallStack, MaxFrames);
   for (int i = 0; i < StackDepth; i++) {
      Dl_info info;
      if (dladdr(CallStack[i], &info) && info.dli_sname) {
         int status = 0;
         char* demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
         const char* func_name = (status == 0 && demangled) ? demangled : info.dli_sname;
         bt << i << "\t" << CallStack[i] << "\t" << func_name << " + " << (char*)CallStack[i] - (char*)info.dli_saddr << newline;
         free(demangled);
      } else {
         bt << i << "\t" << CallStack[i] << "\t[Unknown Symbol]" << newline;
      }
   }
   bt << newline;
}
