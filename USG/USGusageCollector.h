#ifndef __USG_USAGE_COLLECTOR_H__
#define __USG_USAGE_COLLECTOR_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USGusageCollector
//
// Description:
//
// Iguana usage
//
// Author: John Q
// Date:   Tue Oct  2 10:16:34 EDT 2024
// ---------------------------------------------------------------------------

#include <COL/COLclosure.h>

class SCKloop;

void USGstartUsageCollectionTimer(SCKloop* pLoop, COLclosure0* pCollect);
void USGstopUsageCollectionTimer();

#endif // end of defensive include
