#ifndef __COL_DEBUG_NEW_DELETE_H__
#define __COL_DEBUG_NEW_DELETE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: COLdebugNewDelete
//
// Description:
//
// A very low-tech custom new/delete implementation for NON-VALGRIND testing.
// This allocator's reason of being is to try to get incorrectly initialized
// code to crash while not impeding its speed too much and not serializing
// its threads as valgrind does.
//
// Note: This is not really a header file.
//       It is code to be #include'd in the same .cpp file as main().
//
// Eliot - I would leave reading this file till a second pass of COL.
//
// Author: Andrew Vajoczki
// Date: 2008-12-11
//---------------------------------------------------------------------------

#if defined(CHM_ENABLE_DEBUG_NEW_DELETE)

#undef NDEBUG
#include <assert.h>

#include <new>
#include <memory.h>
#include <cstdlib>

enum
{
   COLdebugNewDelete_MAGIC         = 0xa8b45db3,
   COLdebugNewDelete_UNINITIALIZED = 0xda,
   COLdebugNewDelete_DELETED       = 0xde,
   COLdebugNewDelete_TRAILING      = 0xdc
};

struct COLdebugNewDelete {
   size_t Size;
   size_t Magic;
#if defined(__APPLE__)
   // operator new on Mac supposed to return 16 byte aligned memory
   size_t Unused1;
   size_t Unused2;
#endif
};

void* operator new(size_t Size)
#if defined(__linux__)
   throw()
#else
   throw(std::bad_alloc)
#endif
{
   unsigned char* Block = (unsigned char*)malloc(
      sizeof(COLdebugNewDelete) + Size + 1);
   COLdebugNewDelete* Header = (COLdebugNewDelete*)Block;
   Header->Size = Size;
   Header->Magic = Size ^ COLdebugNewDelete_MAGIC;
   unsigned char* p = Block + sizeof(COLdebugNewDelete);
   p[Size] = COLdebugNewDelete_TRAILING;
   memset(p, COLdebugNewDelete_UNINITIALIZED, Size);
   //printf("operator new(%d) = %p\n", Size, p);
#if defined(__APPLE__)
   assert(sizeof(size_t)==4);
   assert(((unsigned long)Block & 15) == 0);
   assert(((unsigned long)p & 15) == 0);
   assert(sizeof(COLdebugNewDelete)==16);
#endif
   return p;
}

void operator delete(void* p)
{
   //printf("operator delete(%p)\n", p);
   if (p)
   {
      COLdebugNewDelete* Header = (COLdebugNewDelete*)p - 1;
      assert(COLdebugNewDelete_MAGIC == (Header->Magic ^ Header->Size));
      assert(COLdebugNewDelete_TRAILING == ((unsigned char*)p)[Header->Size]);
      memset(Header, COLdebugNewDelete_DELETED,
         sizeof(COLdebugNewDelete) + Header->Size + 1);
      free(Header);
   }
}

#endif

//
// Attempts to allocate address space in the first 4 GB of memory, to 
// make future calls to malloc return pointers in the higher address range.
// This is useful for testing 64bit pointer issues where pointers are cast around
// as integers that are possibly too small.
//
// Make sure to call this as early in the program as possible.
// 
// Reports some allocation information to std out.
//
void COLgobble32bitAddressSpace();

#endif // end of defensive include

