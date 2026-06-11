#ifndef __SDB_POSITION_MAP_1_H__
#define __SDB_POSITION_MAP_1_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBpositionMap2
//
// Description:
//
// Queue position map for the journal file. version 1. old
//
// Author: John Qi
// Date:   Saturday 25 March 2023 - 04:37PM
// ---------------------------------------------------------------------------

#include <COL/COLhashmap.h>
#include <COL/COLminimumInclude.h>
#include <COL/COLstring.h>
#include <SDB/SDBcontent.h>
#include <SDB/SDBid.h>

class SDBpositionMap1{
public:
   SDBpositionMap1() {};
   SDBpositionMap1(const COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>& Positions);

   COLuint64 Type = SDB_POSMAP1;  // Must be the first data member to allow casting from or to SDBcontent*
   COLhashmap<COLstring, COLhashmap<COLstring, SDBid>> PositionMap;
};

bool operator==(const SDBpositionMap1& lhs, const SDBpositionMap1& rhs);
inline bool operator!=(const SDBpositionMap1& lhs, const SDBpositionMap1& rhs) { return !(lhs == rhs); }
COLostream& operator<<(COLostream& Stream, const SDBpositionMap1& Obj);

int SDBpositionMapSize1 (const SDBpositionMap1* pPosition);
int SDBwritePositionMap1(const SDBpositionMap1* pPosition, char* pBuffer, int BufferSize);
int SDBreadPositionMap1 (SDBpositionMap1* pPosition, const char* pBuffer, int BufferSize);

// SDBcontent interface
int         SDBpositionMapSize1 (const SDBcontent* pPosition);
int         SDBwritePositionMap1(const SDBcontent* pPosition,       char* pBuffer, int BufferSize);
int         SDBreadPositionMap1 (      SDBcontent* pPosition, const char* pBuffer, int BufferSize);
COLostream& SDBprintPositionMap1(const SDBcontent* pPosition, COLostream& Stream);
inline SDBcontent* SDBcreatePositionMap1() { return (SDBcontent*) new SDBpositionMap1(); }
inline void SDBdeletePositionMap1(SDBcontent* pObject) { delete (SDBpositionMap1*)pObject; }

#endif // end of defensive include
