#ifndef __SDB_PURGE_MANUAL_H__
#define __SDB_PURGE_MANUAL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBpurgeManual
//
// Description:
//
// functions for manually purging a component's log files
//
// Author: John Q
// Date:   Tue 15 Oct 2024 16:56:26 EDT
// ---------------------------------------------------------------------------
#include <COL/COLclosure.h>

class COLstring;
class SDBsystem;

// Removes all log files prior to the one that contains the earliest un-dequeued message.
void SDBpurgeLog(const SDBsystem* pSystem, const COLstring& ComponentId, COLclosure2<COLstring, COLstring>* pCallback=NULL);

#endif // end of defensive include
