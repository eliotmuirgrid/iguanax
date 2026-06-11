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
#include <BLUA/BLUAtask.h>
#include <BLUA/BLUAstop.h>

#include <LUA/LUAmessage.h>
#include <LUA/LUAutils.h>

#include <SCK/SCKloop.h>

#include <SDB/SDBcapi.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBreader.h>

#include <COL/COLioCheck.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


BLUAtask BLUAdequeuedTask(const COLstring& Message, const COLvar& Meta) {
   BLUAtask Task;
   Task.Id = BLUA_ON_DEQUEUE;
   Task.Payload["message"] = Message;
   if (!Meta.isNull()) {
      Task.Payload["meta"] = Meta;
   }
   return Task;
}

// LUA THREAD
void BLUAhandleDequeue(lua_State*& L, const COLvar& Payload, int* pResult) {
   COL_FUNCTION(BLUAhandleDequeue);
   COLASSERT(L);
   // Note: In principle, we don't want to touch BLUAinstance from the lua thread to avoid race conditions.
   // However, our current design makes the lua_State only accessible from the lua thread. Since we are not
   // modifying BLUAinstance here and m_DequeuePosition should not change in the middle of a dequeue cycle,
   // it should not to be a problem. This is definitely not ideal.
   BLUAinstance2* pInstance = BLUAinstanceGet(L);
   const SDBmessageId& MessageId = pInstance->m_DequeuePosition.MessageId;
   LUAsetMessage(L, MessageId.ComponentId, MessageId.DateTime, MessageId.Index);
   if (!BLUAgetCalledFunction(COL_T("main"), L, pResult)){
      COL_TRC("No main function found.");
      return;
   }
   int ArgCount = 1;
   LUApushString(L, Payload["message"]);
   if (Payload.exists("meta")) {
      Payload["meta"].toLua(L);
      ArgCount++;
   }
   *pResult = lua_pcall(L, ArgCount, 0, 0);
}

void BLUAinstanceDequeued(BLUAinstance2* pInstance) {
   COL_FUNCTION(BLUAinstanceDequeued);
#ifndef IFW_RELEASE
   COLASSERT(COLisMainThread());
#endif
   const COLstring& ComponentId = pInstance->m_Config.CoreConfig.Guid;
   int RetryDelay = pInstance->m_DequeueRetryDelay;
   COL_VAR(RetryDelay);
   if (-1 != RetryDelay) {
      COL_TRC("Dequeue failed. Schedule retry.");
      SDBIlogComponent("Translator failed to process the message. Requested retry in " + COLintToString(RetryDelay) + " ms.", ComponentId, "", pInstance->m_DequeuePosition.MessageId);
      BLUAinstanceRetryDequeueSetup(pInstance);
      return;
   }
   COL_TRC("Message dequeue successful!");
   COL_VAR2(pInstance->m_DequeuePosition, ComponentId);
   //SDBIlogComponent("Translator successfully processed the message. Committing queue position.", ComponentId, "", pInstance->m_DequeuePosition);
   SDBIcommitPosition(pInstance->m_DequeuePosition, ComponentId);


   // TODO: probably should have a dedicated PositionCommitted callback function to
   // do the following, and call BLUAinstanceDequeueMessage() in there
   pInstance->m_Config.Status.LastActivity = time(NULL);
   SDBIincrementStatsCount(ComponentId);
   pInstance->m_DequeueListIndex++;
}



static bool BLUAcheckPendingShutdown(BLUAinstance2* pInstance) {
   COLclosure0* pShutdown = pInstance->m_pShutdown;
   if (pShutdown == NULL) {
		COL_TRC("pointer is null");
      return false;
   }
   COL_WRN("Need to shut down now");
   pShutdown->runAndDelete();
   pInstance->m_pShutdown = NULL;
   return true;
}

static void BLUAinstanceQueueNotEmpty(BLUAinstance2* pInstance, COLstring QueueId) {
   COL_FUNCTION(BLUAinstanceQueueNotEmpty);
#ifndef IFW_RELEASE
   COLASSERT(COLisMainThread());
#endif
   COL_TRC("New messages pushed to queue " << QueueId << ". Add it back to the dequeue list");
   pInstance->m_DequeueList.push_back(QueueId);
   if (pInstance->m_DequeueList.size() == 1) {
      COL_TRC("Dequeue message from queue " << QueueId << " now");
      BLUAinstanceDequeueMessage(pInstance);
   } else {
      COL_TRC("Dequeue in progress");
   }
}

static void BLUAhandleBadReaderConfig(BLUAinstance2* pInstance, const COLstring& QueueId, const COLstring& ComponentId) {
   SDBmessageId Position = SDBIcurrentPosition(QueueId, ComponentId);
   COLstring Error;
   COLostream Stream(Error);
   Stream << "Unable to dequeue messages from queue '" << QueueId << "'.\n";
   Stream << "The queue position '" << Position.ComponentId << "-" << Position.DateTime << "-" << Position.Index << "' points to a log file that no longer exists.\n";
   Stream << "There is a potential loss of queued messages that's beyond Iguana's control. Please make sure all queued messages are present and accounted for.\n\n";
   Stream << "The queue '" << QueueId << "' must be manually repositioned to a valid position to restore normal operations.";
   SDBIlogComponent(Error, ComponentId, "#error");
   pInstance->m_Config.Status.Light = CFG_LIGHT::ERR;
   pInstance->m_Config.Status.ErrorCount++;
   pInstance->m_Dequeueing = false;
   BLUAinstanceScheduleAllStop(pInstance);
}

static void BLUAinstanceMessageDequeued(int Code, SDBmessage* pMessage, BLUAinstance2* pInstance) {
   COL_FUNCTION(BLUAinstanceMessageDequeued);
#ifndef IFW_RELEASE
   COLASSERT(COLisMainThread());
#endif
   if (BLUAcheckPendingShutdown(pInstance)) { return; }  // dequeued a message but not processed yet, safe to shutdown
   const COLstring& ComponentId = pInstance->m_Config.CoreConfig.Guid;
   COLASSERT(pInstance->m_DequeueListIndex < pInstance->m_DequeueList.size());
   COLstring QueueId = pInstance->m_DequeueList[pInstance->m_DequeueListIndex];
   COL_VAR4(ComponentId, QueueId, Code, pMessage);
   if (SDB_END_OF_LOG == Code) {
      COL_TRC("Current queue is empty. Set up notification and wait for new messages");
      // QueueId caused a crash, maybe m_DequeueList was modified by a worker thread. See IX-2347
      // https://interfaceware.atlassian.net/browse/IX-2347?focusedCommentId=254821
      SDBIlogComponent("Queue '" + QueueId + "' is empty. Wait for new messages.", ComponentId);
      COL_VAR2(QueueId, ComponentId);
      SDBIsetQueueNotEmpty(QueueId, ComponentId, COLnewClosure0(&BLUAinstanceQueueNotEmpty, pInstance, QueueId));
      for (auto Queue = pInstance->m_DequeueList.begin(); Queue != pInstance->m_DequeueList.end(); Queue++) {
         if (QueueId == *Queue) {
            COL_TRC("Remove queue " << QueueId << " from the dequeue list");
            pInstance->m_DequeueList.erase(Queue);
            // pInstance->m_DequeueListIndex now points to the next queue in pInstance->m_DequeueList
            break;
         }
      }
      if (pInstance->m_DequeueList.size()) {
         COL_TRC("Dequeue from next queue");
         BLUAinstanceDequeueMessage(pInstance);
      } else {
         pInstance->m_Dequeueing = false;
      }
      return;
   }
   if (SDB_READER_BAD_CONFIG == Code) {
      BLUAhandleBadReaderConfig(pInstance, QueueId, ComponentId);
      return;
   }
   if (Code) {
      COLstring ErrMsg = "Failed to dequeue a message from " + QueueId + ". Error " + COLintToString(Code) + " : " + COLerrorString(Code) + ". Try again in the next round.";
      COL_ERR(ErrMsg);
      SDBIlogComponent(ErrMsg, ComponentId, "#error");
      COLASSERT(pInstance->m_DequeueList.size() > 0);    // size can't be 0 if we just tried to dequeue and failed
      // Dequeue happens in a round-robin fashion. If we failed to dequeue a message from the current queue, we will try again in the next round.
      pInstance->m_DequeueListIndex++;    // dequeue from next queue
      BLUAinstanceDequeueMessage(pInstance);
      return;
   }
   COL_VAR(*pMessage);
   // schedule to handle this message
   SDBIlogComponent("Dequeued message from '" + pMessage->Id.MessageId.ComponentId + "'.", ComponentId, "", pMessage->Id.MessageId);
   SDBIlogComponent(pMessage->Data, ComponentId, "", pMessage->Id.MessageId);
   //SDBIlogComponent("Processing the message using Translator.", ComponentId, "", pMessage->Id);
   pInstance->m_JobQueue.put(BLUAdequeuedTask(pMessage->Data, pMessage->Meta));
   pInstance->m_DequeuePosition = pMessage->Id;
   COL_VAR2(pInstance->m_DequeuePosition, ComponentId);
}

void BLUAinstanceDequeueMessage(BLUAinstance2* pInstance) {
   COL_FUNCTION(BLUAinstanceDequeueMessage);
#ifndef IFW_RELEASE
   COLASSERT(COLisMainThread());
#endif
   if (BLUAcheckPendingShutdown(pInstance)) { return; }
   if (pInstance->m_DequeueList.size() == 0) {
      COL_WRN("No source queue(s) to dequeue messages from");
      return;
   }
   if (pInstance->m_DequeueListIndex >= pInstance->m_DequeueList.size()) {
      pInstance->m_DequeueListIndex = 0;
   }
   COLASSERT(pInstance->m_DequeueListIndex < pInstance->m_DequeueList.size());
   const COLstring& QueueId = pInstance->m_DequeueList[pInstance->m_DequeueListIndex];
   const COLstring& ComponentId = pInstance->m_Config.CoreConfig.Guid;
   COL_TRC("Dequeueing a message from " << QueueId << " -> " << ComponentId);
   //SDBIlogComponent("Dequeueing a message from '" + QueueId + "'.", ComponentId);
	pInstance->m_Dequeueing = true;
   SDBIdequeueMessage(QueueId, ComponentId);
}



void BLUAinstanceSetUpDequeue(BLUAinstance2* pInstance) {
   COL_FUNCTION(BLUAinstanceSetUpDequeue);
#ifndef IFW_RELEASE
   // Commented out because unit tests (BLUA/BadScript) run on worker threads
   // COLASSERT(COLisMainThread());
#endif
   const CFGcore& Config = pInstance->m_Config.CoreConfig;
   const COLstring& ComponentId = Config.Guid;
   for (int i = 0; i < Config.Sources.size(); ++i) {
      const COLstring& QueueId = Config.Sources[i];
      // make sure a consumer exists for each source queue
      // this may harmlessly duplicate the SDBIcreateQueueConsumer() call during component linking.
      // It's necessary for the case where 1. link component, 2. restart iguana, 3. start the linked component
      SDBIcreateQueueConsumer(QueueId, ComponentId);
      // set dequeue callback
      SDBIsetPositionCommitted(QueueId, ComponentId, COLnewClosure0(&BLUAinstanceDequeueMessage, pInstance));
      // create queue reader
      SDBIcreateDequeue(QueueId, ComponentId, COLnewClosure2(&BLUAinstanceMessageDequeued, pInstance));
      pInstance->m_DequeueList.push_back(QueueId);
      pInstance->m_DequeueListIndex = 0;
   }
}

void BLUAinstanceTearDownDequeue(BLUAinstance2* pInstance) {
   COL_FUNCTION(BLUAinstanceTearDownDequeue);
   COL_TRC("Removing queue readers");
   const CFGcore& Config = pInstance->m_Config.CoreConfig;
   const COLstring& ComponentId = Config.Guid;
   for (int i = 0; i < Config.Sources.size(); ++i) {
      const COLstring& QueueId = Config.Sources[i];
      // remove dequeue callbacks
      SDBIsetPositionCommitted(QueueId, ComponentId);
      // destroy queue reader
      SDBIdeleteDequeue(QueueId, ComponentId);
   }
   // in case component stops for a Lua error, reset retry timer and QueueNotEmpty callbacks. IX-4194
   BLUAinstanceStopDequeue(pInstance);
}

// reset retry timer and queue-not-empty callbacks when user stops the component
void BLUAinstanceStopDequeue(BLUAinstance2* pInstance) {
   COL_FUNCTION(BLUAinstanceStopDequeue);
   // prevent dequeue retry timer from starting the dequeue cycle
   if (pInstance->m_pDequeueRetryTimer) {
      COL_VAR(pInstance->m_pDequeueRetryTimer);
      pInstance->m_pLoop->cancelTimer(pInstance->m_pDequeueRetryTimer);
      pInstance->m_pDequeueRetryTimer = 0;
   }
   // prevent queue-not-empty from starting the dequeue cycle
   const CFGcore& Config = pInstance->m_Config.CoreConfig;
   const COLstring& ComponentId = Config.Guid;
   for (int i = 0; i < Config.Sources.size(); ++i) {
      const COLstring& QueueId = Config.Sources[i];
      // deregister queue-not-empty callback
      SDBIsetQueueNotEmpty(QueueId, ComponentId);
   }
}
