// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKloopTimer
//
// Description:
//
// Implementation. Extracted out addTimer and cancelTimer for easy less noisy tracing
//
// Author: Eliot Muir 
// Date:   Thursday 12 October 2023 - 11:11AM
// ---------------------------------------------------------------------------

#include <SCK/SCKloop.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int SCKloop::addTimer(COLauto<COLclosure0> pClosure, int TimeInMilliseconds, bool Repeating, const char* TimerName) {
   COLASSERT(m_StartThread == 0 || m_StartThread == COLthreadGetCurrentThreadID());  // must be called from the main thread
   int Timer = m_Schedule.add(pClosure, TimeInMilliseconds, Repeating);
   COL_VAR(TimerName);
   COL_TRC("Creating timer: " << Timer);
   return Timer;
}

void SCKloop::cancelTimer(int TimerId) {
   COLASSERT(m_StartThread == 0 || m_StartThread == COLthreadGetCurrentThreadID());  // must be called from the main thread
   COL_TRC("Cancelling timer: " << TimerId);
   m_Schedule.cancel(TimerId);
}
