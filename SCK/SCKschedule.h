#ifndef __SCK_SCHEDULE_H__
#define __SCK_SCHEDULE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKschedule
//
// Description:
//
// This class has the logic for implementing timer functionality with
// the SCK library.  It is part of SCKloop.  This
// can be thought of as part of the internal implementation of
// SCKloop but is separated for the purpose of separation of concerns.
//
// Author: Eliot Muir
// Date:   Tue  6 Jan 2015 11:07:19 EST
//---------------------------------------------------------------------------

#include <COL/COLmap.h>
#include <COL/COLhashmap.h>
#include <COL/COLclosure.h>
#include <COL/COLdateTime.h>
#include <COL/COLauto.h>
#include <time.h>

class SCKtimerInfo{
public:
   SCKtimerInfo(const COLtimeStamp& NextTime, int Interval, COLauto<COLclosure0> pClosure, bool Repeating);
   ~SCKtimerInfo();

   COLauto<COLclosure0> m_pClosure;
   bool m_Repeating;
   COLtimeStamp m_NextTime;
   int m_Interval; //In milliseconds
};

// Used when transferring timers into list 
struct TimerIdAndInfo { 
   int TimerId;     
   COLauto<SCKtimerInfo> TimerInfo;
};

class SCKschedule{
public:
   SCKschedule();
   ~SCKschedule();

   int add(COLauto<COLclosure0> pClosure, int IntervalInMilliseconds = 0, bool Repeating = false);

   bool isSet(int TimerId) const;
   void cancel(int TimerId);

   // Dangerous - only for unit testing
   void setTimerId(int TimerId);
   int registeredTimerCount();

   // This invokes the timers.
   void fire();

   void clear();

   int timeTillNextEvent(int IntervalInMilliseconds); //Returns the number of milliseconds until the next event if less than the passed in IntervalInMilliseconds.

   int m_NextTimerId;
   COLhashmap<int, COLauto<SCKtimerInfo> > m_TimerCollection;
   COLmap<COLtimeStamp, int> m_ScheduleCollection;

private:
   SCKschedule(const SCKschedule& Orig);
   SCKschedule& operator=(SCKschedule& Orig);
};

#endif // end of defensive include
