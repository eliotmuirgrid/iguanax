#ifndef __MZIP_MEMORY_H__
#define __MZIP_MEMORY_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: MZIPmemory
//
// Description:
//
// Supplemental struct for ioapi_mem.c for the write buffer.
//---------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

/* Pass pointer to your struct in the opaque parameter */
typedef struct MZIPmemory_s {
   char* Memory;
   int   Size;
} MZIPmemory_t;

#ifdef __cplusplus
}
#endif

#endif

