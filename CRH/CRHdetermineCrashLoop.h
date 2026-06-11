#ifndef __CRH_DETERMINE_CRASH_LOOP_H__
#define __CRH_DETERMINE_CRASH_LOOP_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CRHdetermineCrashLoop
//
// Description:
//
// Returns whether Iguana is in a crash loop. Does this by comparing the current
// time to the last crash file time and the second latest crash file time. If 
// the second latest crash was within CRH_CRASH_LOOP_THRESHOLD of the latest crash
// and the latest crash was withing CRH_CRASH_LOOP_THRESHOLD of the current time
// then Iguana is in a crash loop.
//
// Author: Vismay Shah 
// Date:   Tuesday 25 February 2025 - 12:31PM
// ---------------------------------------------------------------------------

bool CRHdetermineCrashLoop();

#endif // end of defensive include
