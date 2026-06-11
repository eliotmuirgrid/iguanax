//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLdebugNewDelete
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   Sunday, November 22nd, 2009 @ 07:36:27 PM
//---------------------------------------------------------------------------

#include "COLdebugNewDelete.h"
#include "COLwindows.h"
#include "COLminimumInclude.h"

#include <stdlib.h>
#include <stdio.h>

#ifndef _WIN32
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#endif

#ifndef MAP_NORESERVE
#  define MAP_NORESERVE (0)
#elif !defined(MAP_ANONYMOUS) && defined(__APPLE__)
#  define MAP_ANONYMOUS MAP_ANON
#endif

static void* PageAllocate(void* pSuggested, unsigned int PageSize){
#ifdef _WIN32
   void* pAllocated = ::VirtualAlloc(pSuggested, PageSize, MEM_RESERVE ,PAGE_NOACCESS);
   if (pAllocated){
      return pAllocated;
   }
   return (void*)-1;
#else
   void* pAllocated = ::mmap((char*)pSuggested,
                             PageSize,
                             PROT_NONE,
                             MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, 
                             -1, 
                             0);
   if (pAllocated == MAP_FAILED){
      pAllocated = (void*)-1;
   }
   return pAllocated;
#endif
}

static void PageDeallocate(void* pRegion, unsigned int PageSize){
#ifdef _WIN32
   ::VirtualFree(pRegion,0,MEM_RELEASE);
#else
   ::munmap((char*)pRegion,PageSize);
#endif
}

void COLgobble32bitAddressSpace(){
   if (sizeof(void*) == sizeof(COLuint64)){
      //Only applicable on 64 bit platforms
#ifdef _WIN32
      SYSTEM_INFO SysInfo;
      ::GetSystemInfo(&SysInfo);
      unsigned int PageSize = SysInfo.dwAllocationGranularity;
#else
      unsigned int PageSize = ::sysconf(_SC_PAGE_SIZE);
#endif
      unsigned int AllocatedPages = 0;
      unsigned int SkippedPages = 0;
      void *pStart = 0;
      while( ((COLuint64)pStart) < COL_UINT_32_MAX){
         void* pAllocated = PageAllocate(pStart, PageSize);
         if (pAllocated  != (void*)-1){
            if (pAllocated == pStart){
               AllocatedPages++; //Allocated at expected location
            } else {
               //Allocated at a different location, unallocate and consider this page unreserved
               SkippedPages++;
               PageDeallocate(pAllocated,PageSize);
            }
         } else {
            SkippedPages++; //could not allocate at all
         }
         pStart = (unsigned char*)pStart + PageSize;
      }
      printf("PageSize : %u\n",PageSize);
      printf("Allocated Pages : %u (%u bytes)\n",AllocatedPages,PageSize*AllocatedPages);
      printf("Skipped Pages : %u (%u bytes)\n",SkippedPages,SkippedPages*PageSize);
   } else{
      printf("Only applicable on 64bit platforms\n");
   }
}
