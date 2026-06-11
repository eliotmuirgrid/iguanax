#ifndef __SDB_CONTENT_H__
#define __SDB_CONTENT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBcontent
//
// Description:
//
// Factory functions which take opaque pointers to content and do various operations to them.
//
// Author: Eliot Muir
// Date:   Friday 24 February 2023 - 10:58AM
// ---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class COLstring;

// Consolidated types since they are all just tracings
//    SDB_INFO     ->   SDB_TRACE
//    SDB_DEBUG    ->   SDB_TRACE
//    SDB_WARNING  ->   SDB_TRACE + #warning
//    SDB_ERROR    ->   SDB_TRACE + #error
//    SDB_SUCCESS  ->   SDB_TRACE
// Old logs don't have tags, so we will add #warning/#error if needed during read

enum SDBtype{
   SDB_NULL     = 0,
   SDB_MARKER   = 1,
   SDB_POSMAP1  = 2,    // v1 position map with SDBmessageId
   SDB_POSITION = 3,
   SDB_MESSAGE  = 4,
   SDB_TRACE    = 5,
   SDB_INFO     = 6,
   SDB_DEBUG    = 7,
   SDB_WARNING  = 8,
   SDB_ERROR    = 9,
   SDB_SUCCESS  = 10,
   SDB_POSMAP   = 11,   // v2 position map with SDBid
   // add more types here
   SDB_EOT  // end of type list
};

void SDBcontentMapInit();

class SDBcontent{
public:
   COLuint64 Type;
};
COLostream& operator<<(COLostream& Stream, const SDBcontent& Obj);

SDBcontent* SDBcontentCreate (      COLuint64   Type);
void        SDBcontentDestroy(      SDBcontent* pContent);
int         SDBcontentSize   (const SDBcontent* pContent);
int         SDBreadContent   (      SDBcontent* pContent, const char* pBuffer, int BufferSize);
int         SDBwriteContent  (const SDBcontent* pContent,       char* pBuffer, int BufferSize);

COLuint64   SDBcontentType   (const char* pBuffer, int BufferSize);

const char* SDBtypeAsString(SDBtype Type);
SDBtype SDBtypeFromString(const COLstring& Type);

#endif // end of defensive include