// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBpositionMapBuggy
//
// Description:
//
// The initial release of v2 position map in v10.1.114 has a queue position
// map and queue position size serialization issue. This file contains the
// old logic that reads buggy position maps. See IX-4246
//
// Author: John Qi
// Date:   Saturday 25 March 2023 - 04:37PM
// ---------------------------------------------------------------------------

#include <SDB/SDBpositionMap.h>

#include <SDB/SDBnumber.h>
#include <SDB/SDBstring.h>

#include <COL/COLhashmap.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


static int SDBreadBuggyQueuePosition(COLhashmap<COLstring, SDBid>& Positions, const char* pBuffer, int BufSize) {
   COL_FUNCTION(SDBreadBuggyQueuePosition);
   const char* pIn = pBuffer;
   int Amount = 0;
   do {
      COLuint64 Size = 0, DateTime = 0, Index = 0, SequenceId = 0, QueueId = 0;
      COLstring ConsumerId, ComponentId;
      Amount = SDBreadNumber(&Size,        pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; Size -= Amount; if (BufSize < Size) { Amount = -1; break; }
      Amount = SDBreadString(&ConsumerId,  pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; Size -= Amount;
      Amount = SDBreadString(&ComponentId, pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; Size -= Amount;
      Amount = SDBreadNumber(&DateTime,    pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; Size -= Amount;
      Amount = SDBreadNumber(&Index,       pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; Size -= Amount;
      Amount = SDBreadNumber(&SequenceId,  pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; Size -= Amount;
      Amount = SDBreadNumber(&QueueId,     pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; Size -= Amount;
      COL_VAR(Size);
      COLASSERT(0 == Size);      // amount parsed should match the Size
      Positions[ConsumerId] = SDBid(SDBmessageId(ComponentId, DateTime, Index), SequenceId, QueueId);
      return pIn - pBuffer;
   } while (false);
   if (Amount < -1) {
      COL_ERR("Failed to read number: " << Amount);
   } else {
      COL_ERR("Not enough data for a complete position");
   }
   return Amount;
}

int SDBreadBuggyQueuePositionMap(COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>& PositionMap, const char* pBuffer, int BufSize) {
   COL_FUNCTION(SDBreadBuggyQueuePositionMap);
   const char* pIn = pBuffer;
   int Amount = 0;
   do {
      COLstring QueueId;
      COLuint64 Size = 0, PositionSize = 0;
      Amount = SDBreadNumber(&Size,         pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; Size -= Amount; if (BufSize < Size) { Amount = -1; break; }
      Amount = SDBreadString(&QueueId,      pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; Size -= Amount;
      Amount = SDBreadNumber(&PositionSize, pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; Size -= Amount;
      COL_VAR2(QueueId, PositionSize);
      COLhashmap<COLstring, SDBid>& Map = PositionMap[QueueId];
      for (int i = 0; i < PositionSize; ++i) {
         Amount = SDBreadBuggyQueuePosition(Map, pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; Size -= Amount;
      }
      COL_VAR(Size);
      COLASSERT(0 == Size);      // amount parsed should match the Size
      return pIn - pBuffer;
   } while (false);
   if (Amount < -1) {
      COL_ERR("Failed to read number: " << Amount);
   } else {
      COL_ERR("Not enough data for a complete set of positions");
   }
   return Amount;
}
