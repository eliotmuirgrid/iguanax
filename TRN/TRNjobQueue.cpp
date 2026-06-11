// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNjobQueue
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Monday 08 May 2023 - 10:49AM
// ---------------------------------------------------------------------------

#include <TRN/TRNjobQueue.h>
#include <TRN/TRNjobRunner.h>
#include <TRN/TRNannotationJob.h>
#include <SDB/SDBcapi.h>

#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

void TRNclearJobQueue(TRNjobQueue* pJobQueue){
   COL_FUNCTION(TRNclearJobQueue);
   COL_TRC("Clearing all entries in job list");
   COL_VAR(pJobQueue->JobList.size());
   while (pJobQueue->JobList.size()){
      delete pJobQueue->JobList.front();
      pJobQueue->JobList.pop_front();
   }
   COL_VAR(pJobQueue->JobList.size());
}

static void TRNcheckShutdown(COLclosure0* pCallback, COLhashmap<COLstring, TRNjobQueue*>* pJobMap, SCKloop* pLoop){
   COL_FUNCTION(TRNcheckShutdown);
   if(pJobMap->size() == 0){
      COL_TRC("No more running jobs, safe to call shutdown callback");
      pCallback->runAndDelete();
   } else{
      COL_TRC("Jobs still running, check again soon");
      pLoop->addTimer(COLnewClosure0(&TRNcheckShutdown, pCallback, pJobMap, pLoop), 100, false);
   }
}

void TRNshutdownAllJobs(COLclosure0* pCallback, COLhashmap<COLstring, TRNjobQueue*>* pJobMap, SCKloop* pLoop){
   COL_FUNCTION(TRNshutdownAllJobs);
   COL_VAR(pJobMap->size());
   SDBIlogService("Stopping Translator annotation jobs...", "#shutdown");
   if (pJobMap->size() == 0) {
      COL_TRC("No running job queues to shutdown");
      pCallback->runAndDelete();
      return;
   }
   for (auto i = pJobMap->begin(); i != pJobMap->end(); i++){
      TRNjobRunnerSendShutdownCommand(i->second, pLoop);
   }
   int CheckShutdown = pLoop->addTimer(COLnewClosure0(&TRNcheckShutdown, pCallback, pJobMap, pLoop), 100, false);
   COL_VAR(CheckShutdown);
}

void TRNremoveQueueFromMap(COLstring ComponentId, COLhashmap<COLstring, TRNjobQueue*>* pJobMap){
   COL_FUNCTION(TRNremoveQueueFromMap);
   pJobMap->removeKey(ComponentId);
}

COLostream& operator<<(COLostream& Stream, const TRNjobQueue& Queue){
   Stream << "JOB QUEUE compile = " << Queue.pJob->m_CompileNeeded;
   return Stream;
}
