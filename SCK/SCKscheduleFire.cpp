// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKscheduleFire
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Friday 13 October 2023 - 12:30PM
// ---------------------------------------------------------------------------

#include <SCK/SCKscheduleFire.h>
#include <SCK/SCKschedule.h>

#include <COL/COLlist.h>
#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


void SCKscheduleFire(SCKschedule* pSchedule, const COLtimeStamp& Now){
   COL_FUNCTION(SCKscheduleFire);
   COLlist<TimerIdAndInfo> TimersToFire;
   COLavlTreePlace i = pSchedule->m_ScheduleCollection.first();
   // iterate through the collection and capture all of the timer ids and their information into a list
   while (i && pSchedule->m_ScheduleCollection.key(i) <= Now){
      int TimerId = pSchedule->m_ScheduleCollection[i];
      COL_VAR(TimerId);
      COLhashmap<int, COLauto<SCKtimerInfo> >::iterator j = pSchedule->m_TimerCollection.find(TimerId); 
      COLASSERT(j); // No reason for the timer to not exist
      COL_TRC(COL_T("Adding info of timerID ") << TimerId << COL_T(" to list")); 
      TimerIdAndInfo IdAndInfo = {j->first, j->second};
      TimersToFire.push_back(IdAndInfo);
      COLavlTreePlace Next = pSchedule->m_ScheduleCollection.next(i);
      COL_TRC(COL_T("Removing ")  << TimerId << COL_T("from collections"));
      pSchedule->m_TimerCollection.remove(j);
      pSchedule->m_ScheduleCollection.remove(i);
      i = Next;
   }
   // Iterate through the list and call the callbacks
   COLlist<TimerIdAndInfo>::iterator it = TimersToFire.begin();
   for (;it != TimersToFire.end(); it++) {  
      if (it->TimerInfo->m_Repeating) { // First re-schedule any repeating timers
         COL_TRC("Adding back repeating timer");
         COLtimeStamp NextTime = Now;
         NextTime.addMilliseconds(it->TimerInfo->m_Interval);
         COL_TRC("Schedule for " << NextTime.tv_sec << "seconds, " << NextTime.tv_usec << "microseconds.");
         it->TimerInfo->m_NextTime = NextTime;
         pSchedule->m_TimerCollection.add(it->TimerId, it->TimerInfo);
         pSchedule->m_ScheduleCollection.add(NextTime, it->TimerId);  
      }
      try{
         COL_TRC("running callback"); 
         it->TimerInfo->m_pClosure->run(); 
         COL_TRC("finished running callback"); 
      } catch(const COLerror& Error){ 
         // Eliot - IX-1857 Important that we violently stop when this occurs.
         COLabortWithMessage("Exception thrown from timer event " + COLintToString(it->TimerId) + Error.description());
      }
   }
   COLASSERT(pSchedule->m_ScheduleCollection.size() == pSchedule->m_TimerCollection.size());
}