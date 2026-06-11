#ifndef __SDB_PURGE_COMPONENT_H__
#define __SDB_PURGE_COMPONENT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBpurgeComponent
//
// Description:
//
// functions for purging a component's log files
//
// Author: John Q
// Date:   Tue 15 Oct 2024 14:56:43 EDT
// ---------------------------------------------------------------------------
#include <COL/COLminimumInclude.h>
#include <SDB/SDBdir.h>

class COLstring;
class SDBmessageId;

// returns now - <DaysToKeep> in milliseconds unless <StopId>.DateTime is older
COLuint64 SDBgetPurgeLimit(const SDBmessageId& StopId, int DaysToKeep);

// deletes all component log files that meet the purge criteria and returns the oldest file after the purge.
// returns the oldest file after the purge or an empty string if no log file was deleted.
COLstring SDBpurgeComponentLogs(const COLstring& ComponentId, const SDBmessageId& StopId, int DaysToKeep, const COLstring &RootDir = SDBroot());


// for unit testing
COLstring SDBgetPurgeLimitFile(const COLstring& ComponentId, const SDBmessageId& StopId, int DaysToKeep, const COLstring &RootDir = SDBroot());

#endif // end of defensive include
