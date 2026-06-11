//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CPUIDquery
//
// Description:
//
// Implementation
//
// Author: Steven Dodd
// Date:   Friday, September 10th, 2010 @ 10:15:49 AM
//
//---------------------------------------------------------------------------

#include "CPUIDprecomp.h"
#pragma hdrstop

#include "CPUIDquery.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <string.h>

#if defined(_WIN32)

#include <intrin.h>

void CPUIDquery(int CPUInfo[4], int InfoType){
   __cpuid(CPUInfo, InfoType);
}

// See IX-2375 - x86 assembler won't work on ARM - Eliot
// Note when crossing compiling for Intel Mac from ARM Mac, __x86_64__ is defined instead of __aarch64__.
// Luckily, the function below does't break and "VirtualApple @ 2.50GHz" is reported
#elif !(defined(__arm__) || defined(__aarch64__))

void CPUIDquery(int CPUInfo[4], int InfoType){
   asm volatile(
      "movl    %4, %%eax      \n\t"
      "cpuid                  \n\t"
      "movl    %%eax, %0      \n\t"
      "movl    %%ebx, %1      \n\t"
      "movl    %%ecx, %2      \n\t"
      "movl    %%edx, %3      \n\t"
      : "=r"(CPUInfo[0]), "=r"(CPUInfo[1]), "=r"(CPUInfo[2]), "=r"(CPUInfo[3])
      : "r"(InfoType)
      : "%rax", "%rbx", "%rcx", "%rdx"
   );
}

#else
void CPUIDquery(int CPUInfo[4], int InfoType){   memset(CPUInfo, 0, 4); }
#endif

COLstring CPUIDqueryBrandString() {
   COL_FUNCTION(CPIIDqueryBrandString);
   COL_STATIC_ASSERT(sizeof(int)==4);

   COLstring Result;
   int CPUInfo[4] = { 0 };

   char buf[64];
   memset(buf, 0, sizeof(buf));

   COL_DUMP("Buffer:", buf, sizeof(buf));

   CPUIDquery(CPUInfo, 0x80000000); 
   if (CPUInfo[0] >= 0x80000004){ // check if cpu supports brand string query
      CPUIDquery(CPUInfo, 0x80000002);
      memcpy(buf, (const char*)CPUInfo, 16);

      CPUIDquery(CPUInfo, 0x80000003);
      memcpy(buf + 16, (const char*)CPUInfo, 16);

      CPUIDquery(CPUInfo, 0x80000004);
      memcpy(buf + 32, (const char*)CPUInfo, 16);

      Result.append(buf);
      COL_DUMP("Result:", Result.c_str(), Result.size());
      Result = COLstripWhiteSpace(Result, true, true);
      COL_DUMP("Result:", Result.c_str(), Result.size());
   }
   COL_VAR(Result);

   return Result;
}
