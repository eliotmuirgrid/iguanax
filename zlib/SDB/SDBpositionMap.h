#ifndef __SDB_POSITION_MAP_H__
#define __SDB_POSITION_MAP_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBpositionMap
//
// Description:
//
// Queue position map for the journal file. version 2 with SDBid
//
// Author: John Qi
// Date:   Saturday 25 March 2023 - 04:37PM
// ---------------------------------------------------------------------------

#include <COL/COLhashmap.h>
#include <COL/COLminimumInclude.h>
#include <COL/COLstring.h>
#include <SDB/SDBcontent.h>
#include <SDB/SDBid.h>

class SDBpositionMap{
public:
   SDBpositionMap() {};
   SDBpositionMap(const COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>& Positions);

   COLuint64 Type = SDB_POSMAP;  // Must be the first data member to allow casting from or to SDBcontent*
   COLhashmap<COLstring, COLhashmap<COLstring, SDBid>> PositionMap;
};

bool operator==(const SDBpositionMap& lhs, const SDBpositionMap& rhs);
inline bool operator!=(const SDBpositionMap& lhs, const SDBpositionMap& rhs) { return !(lhs == rhs); }
COLostream& operator<<(COLostream& Stream, const SDBpositionMap& Obj);

int SDBpositionMapSize (const SDBpositionMap* pPosition);
int SDBwritePositionMap(const SDBpositionMap* pPosition, char* pBuffer, int BufferSize);
int SDBreadPositionMap (SDBpositionMap* pPosition, const char* pBuffer, int BufferSize);

// SDBcontent interface
int         SDBpositionMapSize (const SDBcontent* pPosition);
int         SDBwritePositionMap(const SDBcontent* pPosition,       char* pBuffer, int BufferSize);
int         SDBreadPositionMap (      SDBcontent* pPosition, const char* pBuffer, int BufferSize);
COLostream& SDBprintPositionMap(const SDBcontent* pPosition, COLostream& Stream);
inline SDBcontent* SDBcreatePositionMap() { return (SDBcontent*) new SDBpositionMap(); }
inline void SDBdeletePositionMap(SDBcontent* pObject) { delete (SDBpositionMap*)pObject; }


// internal

int SDBreadBuggyQueuePositionMap(COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>& PositionMap, const char* pBuffer, int BufSize);


// unit test

int SDBpositionSize(const COLstring& ConsumerId, const SDBid& Position);
int SDBqueuePositionSize(const COLstring& QueueId, const COLhashmap<COLstring, SDBid>& Positions);

#endif // end of defensive include
