//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAqueue
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Friday May 14 2021
//---------------------------------------------------------------------------

#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAqueue.h>

#include <LUA/LUAawait.h>

#include <SDB/SDBcapi.h>

#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


static COLstring BLUAstringifyMessageId(const SDBid& Id) {
   COLstring Out;
   if (!Id.isNull()) {
      Out = Id.MessageId.ComponentId + "-" + COLuint64ToString(Id.MessageId.DateTime) + "-" + COLuint64ToString(Id.MessageId.Index);
      COL_VAR(Out);
   }
   return Out;
}

static void BLUAinstanceMessageEnqueued(SDBid EnqueuedId, std::weak_ptr<struct LUAsynchronize> pAsync, BLUAinstance2* pInstance) {
   COL_FUNCTION(BLUAinstanceMessageEnqueued);
   COL_TRC("Enqueue completed for messages up to and including " << EnqueuedId);
   SDBid& PendingId = pInstance->m_PendingMessage;
   COLASSERT(!PendingId.isNull());     // don't expect Pending Id to be NULL. let's see if I am wrong
   const COLstring& ComponentId = PendingId.MessageId.ComponentId;
   if (PendingId <= EnqueuedId) {
      COL_TRC("Message " << PendingId << " enqueued.");
      SDBIincrementStatsCount(ComponentId, false);
      //SDBIlogComponent("Message committed to queue.", PendingId.ComponentId, "", PendingId);
      PendingId.reset();
   } else {
      // if EnqueuedId < PendingId, PendingId message is still in the buffer. it will be flushed in the next round.
      // don't expect this to happen, but log a warning in case it does.
      COL_ERR("Unexpected message id " << EnqueuedId << ". Expecting a message id >= " << PendingId);
      COLstring Warning = "Enqueued message " + BLUAstringifyMessageId(EnqueuedId) + ", before " + BLUAstringifyMessageId(PendingId);
      SDBIlogComponent(Warning, ComponentId, "#warning", EnqueuedId.MessageId);
   }

   pInstance->m_Config.Status.LastActivity = time(NULL);
   COL_TRC("Finished enqueueing message");
   LUAasync(pAsync);    // notify BLUAinstanceEnqueue()
}

// Main thread
static void BLUAinstanceEnqueueOnMainThread(std::weak_ptr<struct LUAsynchronize> pAsync, BLUAinstance2* pInstance, COLauto<BLUAenqueueData> pData, SDBid* pId, COLstring* pError) {
   COL_FUNCTION(BLUAinstanceEnqueueOnMainThread);
   // IX-2935 enable if message rate limits are enabled
   // if (DRXpushCountOverLimit()){
   //    COLstring Error = "Could not enqueue message: you have exceeded the number of messages allowed per minute";
   //    SDBIlogService(Error, "#error");
   //    *pError = Error;
   //    LUAasync(pAsync);
   //    return;
   // }
   // DRXincrementCount();

   // TODO: find a way to avoid registering callback every time we push
   const COLstring& QueueId = pInstance->m_Config.CoreConfig.Guid;
   *pId = SDBIenqueueMessage(&pData->Data, QueueId, pData->Tags, pData->RelatedId, &pData->Meta, pError);
   if (!pId->isNull()) {
      SDBIsetMessageEnqueued(QueueId, COLnewClosure1(&BLUAinstanceMessageEnqueued, pAsync, pInstance));
      // BLUAinstanceMessageEnqueued() resets m_PendingMessage, so it should be nil here
      if (!pInstance->m_PendingMessage.isNull()) {
         COLstring Warning = "Overwriting enqueue Id " + BLUAstringifyMessageId(pInstance->m_PendingMessage) + " with " + BLUAstringifyMessageId(*pId);
         SDBIlogComponent(Warning, QueueId, "#warning", pInstance->m_PendingMessage.MessageId);
      }
      pInstance->m_PendingMessage = *pId;
   } else {
      COL_ERR(*pError);
      LUAasync(pAsync);    // notify BLUAinstanceEnqueue()
   }
}

// Lua thread
COLstring BLUAinstanceEnqueue(BLUAinstance2* pInstance, COLauto<BLUAenqueueData> pData, COLstring* pError) {
   COL_FUNCTION(BLUAinstanceEnqueue);
   SDBid Id;
   COLclosure1<std::weak_ptr<struct LUAsynchronize>>* pFunction = COLnewClosure1(&BLUAinstanceEnqueueOnMainThread, pInstance, pData, &Id, pError);
   bool Success = LUAawait(pFunction, pInstance->m_pLoop, pInstance->m_ForceKill);
   if (!Success) {
      // TODO: add timeout parameter to queue.push and report timeout if desired
      COL_WRN("LUAwait() timed out.");
   }
   return BLUAstringifyMessageId(Id);
}
