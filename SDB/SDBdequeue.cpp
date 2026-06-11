// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBdequeue
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Thursday 16 March 2023 - 09:55PM
// ---------------------------------------------------------------------------

#include <SDB/SDBdequeue.h>

#include <SCK/SCKloop.h>

#include <SDB/SDBmessage.h>
#include <SDB/SDBreader.h>
#include <SDB/SDBcapi.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

SDBdequeue::~SDBdequeue() {
   if (HA) {
      delete pDequeueM;
   } else {
      delete pDequeueS;
   }
}

SDBdequeue* SDBcreateDequeue(const SDBmessageId& Config, COLlist<std::shared_ptr<SDBmessage>>* pQueueCache, const COLstring& EncryptionKey) {
   COL_FUNCTION(SDBcreateDequeue);
   COL_VAR(Config);
   SDBdequeue* pDequeue = new SDBdequeue();
   pDequeue->HA = !SDBroot2().is_null();
   COL_VAR(pDequeue->HA);
   if (pDequeue->HA) {
      pDequeue->pDequeueM = SDBcreateDequeueMulti(Config, pQueueCache, EncryptionKey);
   } else {
      pDequeue->pDequeueS = SDBcreateDequeueSingle(Config, pQueueCache, EncryptionKey);
   }
   return pDequeue;
}

void SDBsetOnDequeued(SDBdequeue* pDequeue, COLauto<COLclosure2<int, SDBmessage*>> pCallback) {
   COL_FUNCTION(SDBsetOnDequeued);
   if (pDequeue->HA) {
      SDBsetOnDequeuedMulti(pDequeue->pDequeueM, pCallback);
   } else {
      SDBsetOnDequeuedSingle(pDequeue->pDequeueS, pCallback);
   }
}

void SDBpolluteCache(SDBdequeue* pDequeue) {
   if (pDequeue->HA) {
      SDBpolluteCacheMulti(pDequeue->pDequeueM);
   } else {
      SDBpolluteCacheSingle(pDequeue->pDequeueS);
   }
}

void SDBdequeueMessage(SDBdequeue* pDequeue, SCKloop* pLoop) {
   COL_FUNCTION(SDBdequeueMessage);
   if (pDequeue->HA) {
      SDBdequeueMessageMulti(pDequeue->pDequeueM, pLoop);
   } else {
      SDBdequeueMessageSingle(pDequeue->pDequeueS, pLoop);
   }
}

void SDBremoveCommittedMessage(SDBdequeue* pDequeue, SDBid Position) {
   COL_FUNCTION(SDBremoveCommittedMessage);
   if (pDequeue->HA) {
      SDBremoveCommittedMessageMulti(pDequeue->pDequeueM, Position);
   } else {
      SDBremoveCommittedMessageSingle(pDequeue->pDequeueS, Position);
   }
}

void SDBmovePosition(SDBdequeue* pDequeue, SDBid NewPosition) {
   COL_FUNCTION(SDBmovePosition);
   if (pDequeue->HA) {
      SDBmovePositionMulti(pDequeue->pDequeueM, NewPosition);
   } else {
      SDBmovePositionSingle(pDequeue->pDequeueS, NewPosition);
   }
}
