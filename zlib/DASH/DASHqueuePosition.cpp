// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DASHqueuePosition
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Wednesday 13 September 2023 - 01:42PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <DASH/DASHqueuePosition.h>
#include <SDB/SDBcapi.h>
COL_LOG_MODULE;

static SDBmessageId DASHgetEarliest(const COLarray<SDBmessageId>& QueuePositions) {
   COL_FUNCTION(DASHgetEarliest);
   SDBmessageId Earliest = QueuePositions[0];
   for(const auto& it : QueuePositions) {
      if(it.DateTime < Earliest.DateTime) { Earliest = it; }
      else if(it.DateTime == Earliest.DateTime) { Earliest = (it.Index < Earliest.Index) ? it : Earliest; }
   }
   return Earliest;
}

// Used by DASHqueueComparePosition
COLuint64 DASHfindEarliestQueuePosition(const COLstring& ComponentId){
   COL_FUNCTION(DASHfindEarliestQueuePosition);
   const COLarray<SDBmessageId> QueuePositions = SDBIgetQueuePosition(ComponentId);
   if(!QueuePositions.size()) { return 0; }
   const SDBmessageId Earliest = DASHgetEarliest(QueuePositions);
   return Earliest.DateTime;
}