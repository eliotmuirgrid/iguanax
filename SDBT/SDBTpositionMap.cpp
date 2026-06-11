// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBTpositionMap
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Friday 31 March 2023 - 01:00PM
// ---------------------------------------------------------------------------

#include <SDBT/SDBTpositionMap.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void SDBTcreateTestPositionMapData(COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pOut, int Components, int Index, time_t Date) {
   COL_FUNCTION(SDBTcreateTestPositionMap);
   for (int i = 1; i <= Components; ++i) {
      COLstring QueueId = "Component_" + COLintToString(i);
      for (int j = Components+1; j <= 2*Components; ++j) {
         COLstring ConsumerId = "Component_" + COLintToString(j);
         (*pOut)[QueueId][ConsumerId].MessageId = SDBmessageId(QueueId, Date*1000, Index);
      }
   }
}

SDBpositionMap SDBTcreateTestPositionMap(int Components, int Index, time_t Date) {
   COL_FUNCTION(SDBTcreateTestPositionMap);
   SDBpositionMap Map;
   SDBTcreateTestPositionMapData(&Map.PositionMap, Components, Index, Date);
   return Map;
}

COLstring SDBTcreateSerializedTestPositionMap(const SDBpositionMap& Map) {
   COL_FUNCTION(SDBTcreateSerializedTestPositionMap);
   COLstring Buffer;
   Buffer.setCapacity(SDBpositionMapSize(&Map));
   int Amount = SDBwritePositionMap(&Map, Buffer.get_buffer(), Buffer.capacity()-1);
   Buffer.setSize(Amount);
   return Buffer;
}

COLstring SDBTcreateSerializedTestPositionMap(int Components, int Index, time_t Date) {
   COL_FUNCTION(SDBTcreateSerializedTestPositionMap);
   SDBpositionMap Map = SDBTcreateTestPositionMap(Components, Index, Date);
   COL_VAR(Map);
   COLstring Buffer;
   Buffer.setCapacity(SDBpositionMapSize(&Map));
   int Amount = SDBwritePositionMap(&Map, Buffer.get_buffer(), Buffer.capacity()-1);
   Buffer.setSize(Amount);
   return Buffer;
}
