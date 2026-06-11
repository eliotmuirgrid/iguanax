#ifndef __SCK_SCHEDULE_FIRE_H__
#define __SCK_SCHEDULE_FIRE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKscheduleFire
//
// Description:
//
// This used to be a method on SCKschedule but I turned it into a function.
//
// Author: Eliot Muir 
// Date:   Friday 13 October 2023 - 12:30PM
// ---------------------------------------------------------------------------

struct COLtimeStamp;
class SCKschedule;

void SCKscheduleFire(SCKschedule* pSchedule, const COLtimeStamp& Now);

#endif // end of defensive include
