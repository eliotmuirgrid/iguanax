#ifndef __SDB_QUEUE_COUNT_H__
#define __SDB_QUEUE_COUNT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBqueueCount
//
// Description:
//
// Counts the number of queued messages from the last 2 log files for a given component.
//
// Author: Eliot Muir
// Date:   Thursday 14 September 2023 - 01:46PM
// ---------------------------------------------------------------------------
#include <COL/COLmap.h>

class COLstring;
class SDBsystem;

// returns the total queue count for ComponentId
int SDBqueueCount(SDBsystem* pSystem, const COLstring& ComponentId);

// returns itemized queue count for ComponentId
COLmap<COLstring, int> SDBqueueCountDetail(SDBsystem* pSystem, const COLstring& ComponentId);

#endif // end of defensive include
