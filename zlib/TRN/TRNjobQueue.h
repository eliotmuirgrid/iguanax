#ifndef __TRN_JOB_QUEUE_H__
#define __TRN_JOB_QUEUE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNjobQueue
//
// Description:
//
// Threadsafe job queue system for translator requests. A map of these queues is 
// maintained by DBDworld. Queues last for TRN_KEEP_ALIVE_TIME after which they 
// are destroyed, cleaning up all members.
//
// Author: Vismay Shah 
// Date:   Monday 08 May 2023 - 10:49AM
// ---------------------------------------------------------------------------
#include <COL/COLclosure.h>
#include <COL/COLhashmap.h>
#include <COL/COLlist.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>

#include <time.h>

class SCKloop;
class TRNannotationJob;

struct TRNjobQueue{
   bool IsRunning = false;
   bool IsShutdown = false;
   int ShutdownTimerId = 0;
   time_t LastActivity;
   TRNannotationJob* pJob;
   COLclosure0* pRemoveFromMap;
   COLlist<COLclosure2<TRNjobQueue*, COLvar*>* > JobList;
};

COLostream& operator<<(COLostream& Stream, const TRNjobQueue& Queue);

// This is done on the main thread
void TRNremoveQueueFromMap(COLstring ComponentId, COLhashmap<COLstring, TRNjobQueue*>* pJobMap);

void TRNclearJobQueue(TRNjobQueue* pJobQueue);
void TRNshutdownAllJobs(COLclosure0* pCallback, COLhashmap<COLstring, TRNjobQueue*>* pJobMap, SCKloop* pLoop);

#endif // end of defensive include
