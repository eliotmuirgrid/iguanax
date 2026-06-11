//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKschedule
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tue  6 Jan 2015 11:07:19 EST
//---------------------------------------------------------------------------

#include <SCK/SCKschedule.h>
#include <SCK/SCKscheduleFire.h>

#include <COL/COLlist.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void SCKtraceTimerCollection(const COLhashmap<int, COLauto<SCKtimerInfo> >& Col){
   COL_FUNCTION(SCKtraceTimerCollection);
   for (COLhashmap<int, COLauto<SCKtimerInfo> >::const_iterator i = Col.cbegin(); i != Col.cend(); ++i){
      COL_WRN("Timer ID " << i->first << " not cleaned up!");
   }
}

SCKtimerInfo::SCKtimerInfo(const COLtimeStamp& NextTime, int Interval, COLauto<COLclosure0> pClosure, bool Repeating) 
: m_NextTime(NextTime), m_Interval(Interval), m_pClosure(pClosure), m_Repeating(Repeating){
   COL_METHOD(SCKtimerInfo::SCKtimerInfo);
   COL_VAR(NextTime);
}

SCKtimerInfo::~SCKtimerInfo(){
   COL_METHOD(SCKtimerInfo::~SCKtimerInfo);
}

SCKschedule::SCKschedule() : m_NextTimerId(0){
   COL_METHOD(SCKschedule::SCKschedule);
}

SCKschedule::~SCKschedule(){
   COL_METHOD(SCKschedule::~SCKschedule);
   COL_VAR2(m_ScheduleCollection.size(), m_TimerCollection.size());
   if (m_ScheduleCollection.size() > 0){
      SCKtraceTimerCollection(m_TimerCollection);
   }
   if (m_TimerCollection.size() > 0){
      COL_ERR("There are " << m_TimerCollection.size() << " timers not cleared up.");
      for (auto i = m_TimerCollection.begin(); i != m_TimerCollection.end(); i++){
         COL_ERR("Timer ID:" << i->first << " " << i->second);
      }
   }
   //COLASSERT(m_ScheduleCollection.size() == 0);  // Canary to see that timers were cleaned up
   COLASSERT(m_TimerCollection.size() == 0);     // ditto
};

int SCKschedule::add(COLauto<COLclosure0> pClosure, int IntervalInMilliseconds, bool Repeating){
   COL_METHOD(SCKschedule::add);
   m_NextTimerId++;
   while (isSet(m_NextTimerId)){ // prevent re-use of registered timers if int loops around
      COL_TRC("Timer id " << m_NextTimerId << " already in use.");
      m_NextTimerId++;
   }
   COL_VAR(m_NextTimerId);
   COLtimeStamp NewTime;
   COLcurrentTimeStamp(&NewTime);
   COL_VAR(IntervalInMilliseconds);
   NewTime.addMilliseconds(IntervalInMilliseconds);
   COL_VAR(NewTime);
   m_TimerCollection.add(m_NextTimerId, new SCKtimerInfo(NewTime, IntervalInMilliseconds, pClosure, Repeating));
   m_ScheduleCollection.add(NewTime, m_NextTimerId);
   return m_NextTimerId;
}

bool SCKschedule::isSet(int TimerId) const{
   COL_METHOD(SCKschedule::isSet);
   COL_VAR(TimerId);
   return m_TimerCollection.count(TimerId) != 0;
}

void SCKschedule::cancel(int TimerId){
   COL_METHOD(SCKschedule::cancel);
   COL_VAR(TimerId);
   COLhashmap<int, COLauto<SCKtimerInfo> >::iterator i = m_TimerCollection.find(TimerId);
   COL_VAR(i);
   if (!i){
      COL_ERR("Warning timer ID " << TimerId << " not found for cancel operation.");
      return;
   }
   COLtimeStamp ScheduledTime = i->second->m_NextTime;
   COL_VAR2(ScheduledTime.tv_sec, ScheduledTime.tv_usec);
   m_TimerCollection.remove(i);
   // Scan through m_ScheduleCollection to remove items with the right timer ID.
   COLavlTreePlace j = m_ScheduleCollection.findFirst(ScheduledTime);
   COL_VAR(j);
   while (j && m_ScheduleCollection.key(j) == ScheduledTime){
      COL_VAR(m_ScheduleCollection[j]);
      if (m_ScheduleCollection[j] == TimerId){
         COL_TRC("Found the timer event.");
         m_ScheduleCollection.remove(j);
         return;
      }
      j = m_ScheduleCollection.next(j);
   }
   COL_ERR("We couldn't find the event.");
   COLASSERT(m_ScheduleCollection.size() == m_TimerCollection.size());
}

void SCKschedule::setTimerId(int TimerId){
   COL_METHOD(SCKschedule::setTimerId);
   COL_TRC("Setting timer id to " << TimerId);
   m_NextTimerId = TimerId;
}

int SCKschedule::registeredTimerCount(){
   COL_METHOD(SCKschedule::registeredTimerCount);
   return m_TimerCollection.size();
}

// This invokes the timers.
void SCKschedule::fire(){
   COL_METHOD(SCKschedule::fire);
   COLtimeStamp Now;
   COLcurrentTimeStamp(&Now);
   COL_VAR(Now);
   SCKscheduleFire(this, Now);
}

void SCKschedule::clear(){
   COL_METHOD(SCKschedule::clear);
   COL_VAR2(m_ScheduleCollection.size(), m_TimerCollection.size());
   m_ScheduleCollection.clear();
   m_TimerCollection.clear();
}

int SCKschedule::timeTillNextEvent(int IntervalInMilliseconds){
   //Interval is measured in milliseconds.
   COLtimeStamp Now;
   COLcurrentTimeStamp(&Now);
   COLtimeStamp NextTime(Now);
   NextTime.addMilliseconds(IntervalInMilliseconds); //time with "IntervalInMilliseconds" wait.

   if (m_ScheduleCollection.empty()) { return IntervalInMilliseconds; } //If there are no events scheduled yet.
   COLtimeStamp NextEvent = m_ScheduleCollection.begin()->first;
   if (NextEvent < NextTime){
      COL_VAR2(Now, NextEvent);
      int NewInterval = COLtimeStampDiffInMillisecond(NextEvent, Now);
      COL_VAR(NewInterval);
      if (NewInterval < 0){ return 0; }
      return NewInterval; //return value is in Milliseconds
   }
   return IntervalInMilliseconds;
}
