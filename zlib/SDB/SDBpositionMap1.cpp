// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBpositionMap2
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Saturday 25 March 2023 - 04:37PM
// ---------------------------------------------------------------------------

#include <SDB/SDBpositionMap1.h>

#include <SDB/SDBdir.h>
#include <SDB/SDBnumber.h>
#include <SDB/SDBstring.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

SDBpositionMap1::SDBpositionMap1(const COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>& Positions)
 : PositionMap(Positions) {
}

bool operator==(const SDBpositionMap1& lhs, const SDBpositionMap1& rhs) {
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

COLostream& operator<<(COLostream& Stream, const SDBpositionMap1& Obj) {
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

int SDBpositionMapSize1(const SDBpositionMap1* pPositionMap) {
   COL_FUNCTION(SDBpositionMapSize1);
   const auto& Map = pPositionMap->PositionMap;
   int Size = SDBnumberSize(SDB_POSMAP1) + SDBnumberSize(Map.size());   // type and number of queues
   for (auto Queue = Map.cbegin(); Queue != Map.cend(); Queue++) {
      const auto& Consumers = Queue.value();
      Size += SDBstringSize(Queue.key()) + SDBnumberSize(Consumers.size());  // queue id and number of positions
      for (auto Consumer = Consumers.begin(); Consumer != Consumers.end(); Consumer++) {
         const auto& Position = Consumer.value();
         Size += SDBnumberSize(Position.MessageId.Index) + SDBnumberSize(Position.MessageId.DateTime) + SDBstringSize(Position.MessageId.ComponentId) + SDBstringSize(Consumer.key());   // position and consumer id
      }
   }
   return Size;
}

static int SDBwriteQueuePosition1(const COLstring& ConsumerId, const SDBid& Position, char* pBuffer) {
   COL_FUNCTION(SDBwriteQueuePosition1);
   char* pOut = pBuffer;
   pOut += SDBwriteNumber(Position.MessageId.Index,       pOut);
   pOut += SDBwriteNumber(Position.MessageId.DateTime,    pOut);
   pOut += SDBwriteString(Position.MessageId.ComponentId, pOut);
   pOut += SDBwriteString(ConsumerId,           pOut);
   return pOut-pBuffer;
}

static int SDBwriteQueuePositions1(const COLstring& QueueId, const COLhashmap<COLstring, SDBid>& Positions, char* pBuffer) {
   COL_FUNCTION(SDBwriteQueuePositions1);
   char* pOut = pBuffer;
   pOut += SDBwriteString(QueueId, pOut);
   pOut += SDBwriteNumber(Positions.size(), pOut);
   for (auto Position = Positions.cbegin(); Position != Positions.cend(); Position++) {
      pOut += SDBwriteQueuePosition1(Position.key(), Position.value(), pOut);
   }
   return pOut-pBuffer;
}

int SDBwritePositionMap1(const SDBpositionMap1* pPositionMap, char* pBuffer, int BufferSize) {
   COL_FUNCTION(SDBwritePositionMap1);
   if (BufferSize < SDBpositionMapSize1(pPositionMap)) {
      COL_ERR("Not enough space in buffer to write a position");
      return -1;
   }
   char* pOut = pBuffer;
   pOut += SDBwriteNumber(pPositionMap->Type, pOut);
   pOut += SDBwriteNumber(pPositionMap->PositionMap.size(), pOut);
   for (auto i = pPositionMap->PositionMap.cbegin(); i != pPositionMap->PositionMap.cend(); i++) {
      pOut += SDBwriteQueuePositions1(i.key(), i.value(), pOut);
   }
   return pOut-pBuffer;
}

static int SDBreadQueuePosition1(COLhashmap<COLstring, SDBid>& Positions, const char* pBuffer, int BufSize) {
   COL_FUNCTION(SDBreadQueuePosition1);
   const char* pIn = pBuffer;
   int Amount = 0;
   do {
      COLuint64 Index = 0, DateTime = 0;
      COLstring ConsumerId, ComponentId;
      Amount = SDBreadNumber(&Index,       pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Amount = SDBreadNumber(&DateTime,    pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Amount = SDBreadString(&ComponentId, pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Amount = SDBreadString(&ConsumerId,  pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Positions[ConsumerId] = SDBid(SDBmessageId(ComponentId, DateTime, Index), 0, 0);
      return pIn - pBuffer;
   } while (false);
   if (Amount < -1) {
      COL_ERR("Failed to read number: " << Amount);
   } else {
      COL_ERR("Not enough data for a complete position");
   }
   return Amount;
}

static int SDBreadQueuePositions1(COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>& PositionMap, const char* pBuffer, int BufSize) {
   COL_FUNCTION(SDBreadQueuePositions1);
   const char* pIn = pBuffer;
   int Amount = 0;
   do {
      COLstring QueueId;
      COLuint64 PositionSize = 0;
      Amount = SDBreadString(&QueueId,      pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Amount = SDBreadNumber(&PositionSize, pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      COL_VAR(PositionSize);
      for (int i = 0; i < PositionSize; ++i) {
         Amount = SDBreadQueuePosition1(PositionMap[QueueId], pIn, BufSize);
         if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      }
      return pIn - pBuffer;
   } while (false);
   if (Amount < -1) {
      COL_ERR("Failed to read number: " << Amount);
   } else {
      COL_ERR("Not enough data for a complete set of positions");
   }
   return Amount;
}

int SDBreadPositionMap1(SDBpositionMap1* pPositionMap, const char* pBuffer, int BufSize) {
   COL_FUNCTION(SDBreadPositionMap1);
   const char* pIn = pBuffer;
   int Amount = 0;
   do {
      COLuint64 QueueSize = 0;
      Amount = SDBreadNumber(&pPositionMap->Type, pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Amount = SDBreadNumber(&QueueSize,          pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      COL_VAR(QueueSize);
      if (SDB_POSMAP1 != pPositionMap->Type) { COL_ERR("Not a position map."); return -2; }
      for (int i = 0; i < QueueSize; i++) {
         Amount = SDBreadQueuePositions1(pPositionMap->PositionMap, pIn, BufSize);
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

int SDBpositionMapSize1(const SDBcontent* pPosition) {
   return SDBpositionMapSize1((const SDBpositionMap1*)pPosition);
}

int SDBwritePositionMap1(const SDBcontent* pPosition, char* pBuffer, int BufferSize) {
   return SDBwritePositionMap1((const SDBpositionMap1*)pPosition, pBuffer, BufferSize);
}

int SDBreadPositionMap1(SDBcontent* pPosition, const char* pBuffer, int BufferSize) {
   return SDBreadPositionMap1((SDBpositionMap1*)pPosition, pBuffer, BufferSize);
}

COLostream& SDBprintPositionMap1(const SDBcontent* pPosition, COLostream& Stream) {
   return Stream << *(const SDBpositionMap1*)pPosition;
}
