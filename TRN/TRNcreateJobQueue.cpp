// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNcreateJobQueue
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 08 June 2023 - 10:04AM
// ---------------------------------------------------------------------------
#include <TRN/TRNcreateJobQueue.h>
#include <TRN/TRNjobQueue.h>
#include <TRN/TRNjobRunner.h>
#include <TRN/TRNannotationJob.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TRNresetJobCancelled(TRNannotationJob* pJob){
   COL_FUNCTION(TRNresetJobCancelled);
   COL_TRC("Resetting job cancelled flag");
   pJob->m_AnnotationState.m_Cancelled = false;
}

void TRNsetJobCancelledFlag(TRNannotationJob* pJob){
   COL_FUNCTION(TRNsetJobCancelledFlag);
   COL_TRC("Setting job cancelled flag and signaling event to wake up blocking tasks");
   pJob->m_AnnotationState.m_Cancelled = true;
   if (pJob->m_AnnotationState.m_pEvent) {
      COL_TRC("Early timer fired, signal worker thread execution");
      pJob->m_AnnotationState.m_pEvent->signal();
   }
}

void TRNcreateJobQueue(const COLstring& ComponentId, SCKloop* pLoop, COLhashmap<COLstring, TRNjobQueue*>* pTranslatorJobQueues){
   COL_FUNCTION(TRNcreateJobQueue);
   COL_VAR(pTranslatorJobQueues->count(ComponentId));
   if (!pTranslatorJobQueues->count(ComponentId)){
      COL_TRC("No job queue exists for " << ComponentId << ", creating one");
      TRNjobQueue* pQueue = new TRNjobQueue();
      pQueue->pJob = new TRNannotationJob();
      pQueue->pRemoveFromMap = COLnewClosure0(&TRNremoveQueueFromMap, ComponentId, pTranslatorJobQueues);
      pTranslatorJobQueues->add(ComponentId, pQueue);
      TRNsetupShutdownTimer(pQueue, pLoop);
      COL_TRC("Starting job runner thread");
      TRNjobRunnerStart(pQueue, pLoop);
   }
}