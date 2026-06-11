// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBpositionMap
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Saturday 25 March 2023 - 04:37PM
// ---------------------------------------------------------------------------

#include <SDB/SDBpositionMap.h>

#include <SDB/SDBdir.h>
#include <SDB/SDBnumber.h>
#include <SDB/SDBstring.h>

#include <COL/COLmap.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

SDBpositionMap::SDBpositionMap(const COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>& Positions)
 : PositionMap(Positions) {
}

bool operator==(const SDBpositionMap& lhs, const SDBpositionMap& rhs) {
   auto& lPositionMap = lhs.PositionMap;
   auto& rPositionMap = rhs.PositionMap;
   if (lPositionMap.size() != rPositionMap.size()) { return false; }
   for (auto Queue = lPositionMap.cbegin(); Queue != lPositionMap.cend(); Queue++) {
      const COLstring& QueueId = Queue.key();
      if (rPositionMap.count(QueueId) == 0) { return false; }
      const COLhashmap<COLstring, SDBid>& lConsumers = Queue.value();
      const COLhashmap<COLstring, SDBid>& rConsumers = rPositionMap[QueueId];
      if (lConsumers.size() != rConsumers.size()) { return false; }
      for (auto Consumer = lConsumers.cbegin(); Consumer != lConsumers.cend(); Consumer++) {
         const COLstring& ConsumerId = Consumer.key();
         if (rConsumers.count(ConsumerId) == 0) { return false; }
         if (rConsumers[ConsumerId] != Consumer.value()) { return false; }
      }
   }
   return true;
}

COLostream& operator<<(COLostream& Stream, const SDBpositionMap& Obj) {
   if (Obj.PositionMap.size() == 0) {
      Stream << "[Empty]";
   }
   auto& PositionMap = Obj.PositionMap;
   for (auto Queue = PositionMap.cbegin(); Queue != PositionMap.cend(); Queue++) {
      auto& Consumers = Queue.value();
      for (auto Consumer = Consumers.cbegin(); Consumer != Consumers.cend(); Consumer++) {
         Stream << "{" << Consumer.key() << " -> " << Consumer.value() << "}" << newline;
      }
   }
   return Stream;
}

int SDBpositionSize(const COLstring& ConsumerId, const SDBid& Position) {
   return SDBstringSize(ConsumerId)
        + SDBstringSize(Position.MessageId.ComponentId)
        + SDBnumberSize(Position.MessageId.DateTime)
        + SDBnumberSize(Position.MessageId.Index)
        + SDBnumberSize(Position.SequenceId)
        + SDBnumberSize(Position.QueueId);
}

int SDBqueuePositionSize(const COLstring& QueueId, const COLhashmap<COLstring, SDBid>& Positions) {
   int Size = SDBstringSize(QueueId) + SDBnumberSize(Positions.size());    // queue id and number of positions
   for (const auto& Position : Positions) {
      int PositionSize = SDBpositionSize(Position.first, Position.second);
      Size += SDBnumberSize(PositionSize) + PositionSize;            // position
   }
   return Size;
}

int SDBpositionMapSize(const SDBpositionMap* pPositionMap) {
   COL_FUNCTION(SDBpositionMapSize);
   const auto& Map = pPositionMap->PositionMap;
   int Size = SDBnumberSize(SDB_POSMAP) + SDBnumberSize(Map.size());                      // type and number of queues
   for (auto Queue = Map.cbegin(); Queue != Map.cend(); Queue++) {
      int QueuePositionSize = SDBqueuePositionSize(Queue.key(), Queue.value());
      Size += SDBnumberSize(QueuePositionSize) + QueuePositionSize;
   }
   return Size;
}

static int SDBwriteQueuePosition(const COLstring& ConsumerId, const SDBid& Position, char* pBuffer) {
   COL_FUNCTION(SDBwriteQueuePosition);
   int Size = SDBpositionSize(ConsumerId, Position);
   char* pOut = pBuffer;
   pOut += SDBwriteNumber(Size,                           pOut);
   pOut += SDBwriteString(ConsumerId,                     pOut);
   pOut += SDBwriteString(Position.MessageId.ComponentId, pOut);
   pOut += SDBwriteNumber(Position.MessageId.DateTime,    pOut);
   pOut += SDBwriteNumber(Position.MessageId.Index,       pOut);
   COLASSERT(Position.SequenceId >= 0);
   pOut += SDBwriteNumber(Position.SequenceId,            pOut);
   COLASSERT(Position.QueueId >= 0);
   pOut += SDBwriteNumber(Position.QueueId,               pOut);
   return pOut - pBuffer;
}

static int SDBwriteQueuePositions(const COLstring& QueueId, const COLhashmap<COLstring, SDBid>& Positions, char* pBuffer) {
   COL_FUNCTION(SDBwriteQueuePositions);
   int Size = SDBqueuePositionSize(QueueId, Positions);
   char* pOut = pBuffer;
   pOut += SDBwriteNumber(Size, pOut);
   pOut += SDBwriteString(QueueId, pOut);
   pOut += SDBwriteNumber(Positions.size(), pOut);
   // convert unordered hashmap to ordered map for consistent serialization
   COLmap<COLstring, SDBid> OrderedPositions;
   for (const auto& Entry : Positions) { OrderedPositions[Entry.first] = Entry.second; }
   for (const auto& Position : OrderedPositions) {
      pOut += SDBwriteQueuePosition(Position.first, Position.second, pOut);
   }
   return pOut-pBuffer;
}

int SDBwritePositionMap(const SDBpositionMap* pPositionMap, char* pBuffer, int BufferSize) {
   COL_FUNCTION(SDBwritePositionMap);
   if (BufferSize < SDBpositionMapSize(pPositionMap)) {
      COL_ERR("Not enough space in buffer to write a position");
      return -1;
   }
   char* pOut = pBuffer;
   pOut += SDBwriteNumber(pPositionMap->Type, pOut);
   pOut += SDBwriteNumber(pPositionMap->PositionMap.size(), pOut);
   // convert unordered hashmap to ordered map for consistent serialization
   COLmap<COLstring, COLhashmap<COLstring, SDBid>> OrderedPositionMap;
   for (const auto& Entry : pPositionMap->PositionMap) { OrderedPositionMap[Entry.first] = Entry.second; }
   for (const auto& Map : OrderedPositionMap) {
      pOut += SDBwriteQueuePositions(Map.first, Map.second, pOut);
   }
   return pOut-pBuffer;
}

static int SDBreadQueuePosition(COLhashmap<COLstring, SDBid>& Positions, const char* pBuffer, int BufSize) {
   COL_FUNCTION(SDBreadQueuePosition);
   const char* pIn = pBuffer;
   int Amount = 0;
   do {
      COLuint64 Size = 0, DateTime = 0, Index = 0, SequenceId = 0, QueueId = 0;
      COLstring ConsumerId, ComponentId;
      Amount = SDBreadNumber(&Size,        pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; if (BufSize < Size) { Amount = -1; break; }
      Amount = SDBreadString(&ConsumerId,  pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; Size -= Amount;
      Amount = SDBreadString(&ComponentId, pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; Size -= Amount;
      Amount = SDBreadNumber(&DateTime,    pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; Size -= Amount;
      Amount = SDBreadNumber(&Index,       pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; Size -= Amount;
      Amount = SDBreadNumber(&SequenceId,  pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; Size -= Amount;
      Amount = SDBreadNumber(&QueueId,     pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; Size -= Amount;
      COL_VAR(Size);
      // v10.1.114 incorrectly serializes "sizeof(position_size) + position_size" instead of "position_size"
      // Size will be off by 1 or 2 (if DateTime, Index, SequenceId, QueueId = 0 && ComponentId.size() > 91)
      // If need to extend QueuePosition to include more fields
      //    if v10.1.114 is already an ancient version, likely safe to extend but report error if Size != 0
      //    if v10.1.114 is still recent enough, best to create v3 position map instead
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

static int SDBreadQueuePositions(COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>& PositionMap, const char* pBuffer, int BufSize) {
   COL_FUNCTION(SDBreadQueuePositions);
   const char* pIn = pBuffer;
   int BufSizeIn = BufSize;
   int Amount = 0;
   do {
      COLstring QueueId;
      COLuint64 Size = 0, PositionSize = 0;
      Amount = SDBreadNumber(&Size,         pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; if (BufSize < Size) { Amount = -1; break; }
      Amount = SDBreadString(&QueueId,      pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; Size -= Amount;
      Amount = SDBreadNumber(&PositionSize, pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; Size -= Amount;
      COL_VAR2(QueueId, PositionSize);
      COLhashmap<COLstring, SDBid>& Map = PositionMap[QueueId];
      for (int i = 0; i < PositionSize; ++i) {
         Amount = SDBreadQueuePosition(Map, pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount; Size -= Amount;
      }
      COL_VAR(Size);
      // v10.1.114 incorrectly serializes "sizeof(position_size) + position_size" instead of "position_size" for each queue position
      // Size here will be off by "number of position * (1 or 2)"
      // If need to extend QueuePositionMap to include more fields
      //    if v10.1.114 is already an ancient version, likely safe to extend but report error if Size != 0
      //    if v10.1.114 is still recent enough, best to create v3 position map instead
      return pIn - pBuffer;
   } while (false);
   if (Amount < -1) {
      COL_ERR("Failed to read number: " << Amount);
   } else {
      COL_ERR("Not enough data for a complete set of positions");
      // could be dealing with a buggy queue position map written by initial releases of v10.1.114. See IX-4246.
      return SDBreadBuggyQueuePositionMap(PositionMap, pBuffer, BufSizeIn);
   }
   return Amount;
}

int SDBreadPositionMap(SDBpositionMap* pPositionMap, const char* pBuffer, int BufSize) {
   COL_FUNCTION(SDBreadPositionMap);
   const char* pIn = pBuffer;
   int Amount = 0;
   do {
      COLuint64 QueueSize = 0;
      Amount = SDBreadNumber(&pPositionMap->Type, pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Amount = SDBreadNumber(&QueueSize,          pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      COL_VAR(QueueSize);
      if (SDB_POSMAP != pPositionMap->Type) { COL_ERR("Not a position map."); return -2; }
      for (int i = 0; i < QueueSize; i++) {
         Amount = SDBreadQueuePositions(pPositionMap->PositionMap, pIn, BufSize);
         if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      }
      return pIn - pBuffer;
   } while (false);
   if (Amount < -1) {
      COL_ERR("Failed to read number: " << Amount);
   } else {
      COL_ERR("Not enough data for a complete position map");
   }
   return Amount;
}

int SDBpositionMapSize(const SDBcontent* pPosition) {
   return SDBpositionMapSize((const SDBpositionMap*)pPosition);
}

int SDBwritePositionMap(const SDBcontent* pPosition, char* pBuffer, int BufferSize) {
   return SDBwritePositionMap((const SDBpositionMap*)pPosition, pBuffer, BufferSize);
}

int SDBreadPositionMap(SDBcontent* pPosition, const char* pBuffer, int BufferSize) {
   return SDBreadPositionMap((SDBpositionMap*)pPosition, pBuffer, BufferSize);
}

COLostream& SDBprintPositionMap(const SDBcontent* pPosition, COLostream& Stream) {
   return Stream << *(const SDBpositionMap*)pPosition;
}
