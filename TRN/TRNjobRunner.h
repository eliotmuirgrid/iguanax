#ifndef __TRN_JOB_RUNNER_H__
#define __TRN_JOB_RUNNER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNjobRunner
//
// Description:
//
// Creates a worker thread with a lifetime. The thread takes tasks from the 
// TRNjobQueue and sequentially runs them, responding on main thread when done.
// The thread shuts itself down if TRN_KEEP_ALIVE_TIME is exceeded.
//
// Author: Vismay Shah 
// Date:   Monday 08 May 2023 - 10:48AM
// ---------------------------------------------------------------------------
struct TRNjobQueue;
class COLvar;
class SCKloop;
#include <COL/COLweb.h>

void TRNrunJob(COLclosure2<TRNjobQueue*, COLvar*>* pJob, TRNjobQueue* pRequestQueue, SCKloop* pLoop);
void TRNaddJob(COLclosure2<TRNjobQueue*, COLvar*>* pJob, TRNjobQueue* pRequestQueue, SCKloop* pLoop);


void TRNjobRunnerStart              (TRNjobQueue* pRequestQueue, SCKloop* pLoop);
void TRNsetupShutdownTimer          (TRNjobQueue* pRequestQueue, SCKloop* pLoop);
void TRNjobRunnerShutdown           (TRNjobQueue* pRequestQueue, SCKloop* pLoop);
void TRNjobRunnerSendShutdownCommand(TRNjobQueue* pRequestQueue, SCKloop* pLoop);

// New closure interface
void TRNshutdownTask              (TRNjobQueue* pRequestQueue, COLvar* pResult);
void TRNannotationTask            (TRNjobQueue* pRequestQueue, COLvar* pResult, COLvar Input, COLaddress Address);
void TRNannotationUpdateTask      (TRNjobQueue* pRequestQueue, COLvar* pResult, COLvar Input, COLaddress Address);
void TRNannotationWindowTask      (TRNjobQueue* pRequestQueue, COLvar* pResult, COLvar Input, COLaddress Address);
void TRNannotationFunctionListTask(TRNjobQueue* pRequestQueue, COLvar* pResult, COLvar Input, COLaddress Address);

#endif // end of defensive include
