#ifndef __SDB_FLUSH_H__
#define __SDB_FLUSH_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBflush
//
// Description:
//
// Functions for flushing the journal and logs periodically
//
// Author: John Qi
// Date:   Thursday 09 March 2023 - 03:49PM
// ---------------------------------------------------------------------------

#include <COL/COLclosure.h>
#include <COL/COLhashmap.h>
#include <COL/COLstring.h>
#include <SDB/SDBdir.h>
#include <SDB/SDBmessageId.h>

class SDBsystem;

// Flush log files when the journal file size grows beyond SDB_LOG_FLUSH_THRESHOLD
#define SDB_LOG_FLUSH_SIZE_THRESHOLD 134217728  // 128 MiB
// Flush log files at least every SDB_DEFAULT_MAX_LOG_FLUSH_INTERVAL seconds
#define SDB_DEFAULT_MAX_LOG_FLUSH_INTERVAL 10   // seconds
#define SDB_MAX_LOG_FLUSH_INTERVAL_ENV_NAME "IFW_MAX_LOG_FLUSH_INTERVAL"

int SDBgetMaxLogFlushInterval();

// starts the flush cycle for the system
void SDBstartFlushCycle(SDBsystem* pSystem, COLclosure0* pCallback);

// flush the system as soon as possible
void SDBflushSystemNow(SDBsystem* pSystem);

// stops the flush cycle and flush buffers one last time.
// note: log, enqueue, and position commit should be disabled before stopping the flush cycle
void SDBstopFlushCycle(SDBsystem* pSystem, COLclosure0* pCallback);

#endif // end of defensive include
