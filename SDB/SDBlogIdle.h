#ifndef __SDB_LOG_IDLE_H__
#define __SDB_LOG_IDLE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBlogIdle
//
// Description:
//
// Everything related to closing idle log file handles
//
// Author: John Qi
// Date:   Tuesday 25 April 2023 - 05:09PM
// ---------------------------------------------------------------------------

#include <COL/COLmap.h>
#include <SDB/SDBlogThread.h>

class COLstring;
class SCKloop;

void SDBcloseIdleHandles(COLmap<COLstring, SDBlogHandle>& HandleMap);

int  SDBstartIdleHandleTimer(SCKloop* pLoop, COLthreadQueue<SDBlogTask>* pLogTaskQueue);
void SDBstopIdleHandleTimer(SCKloop* pLoop, int& TimerId);

#endif // end of defensive include
