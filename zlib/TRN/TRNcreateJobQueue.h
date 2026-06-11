#ifndef __TRN_CREATE_JOB_QUEUE_H__
#define __TRN_CREATE_JOB_QUEUE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNcreateJobQueue
//
// Description:
//
// Helper function which creates a TRNjobQueue for a translator component, adds
// it to the TranslatorJobQueues hashmap, and starts the job runner. Used by 
// the ide/annotation/* endpoints.
//
// Author: Vismay Shah 
// Date:   Thursday 08 June 2023 - 10:04AM
// ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>

class COLstring;
class SCKloop;
class TRNannotationJob;
struct TRNjobQueue;

void TRNcreateJobQueue(const COLstring& ComponentId, SCKloop* pLoop, COLhashmap<COLstring, TRNjobQueue*>* pTranslatorJobQueues);
void TRNresetJobCancelled(TRNannotationJob* pJob);
void TRNsetJobCancelledFlag(TRNannotationJob* pJob);

#endif // end of defensive include
