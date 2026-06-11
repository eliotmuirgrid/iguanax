//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTimer
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tue  6 Jan 2015 11:26:16 EST
//---------------------------------------------------------------------------

#include "testTimer.h"

#include <limits.h> // INT_MAX and INT_MIN

#include <COL/COLthreadPool.h>
#include <COL/COLutils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <UNIT/UNITapp.h>

#include <SCK/SCKscheduleFire.h>
#include <SCK/SCKschedule.h>
#include <SCK/SCKloop.h>

// Tick1 is used to assert that a timer event doesn't happen in certain cases
static void Tick1() {
   COL_FUNCTION(Tick1);
   COLASSERT(1 == 0);
}

// Tick2 is used to increment a counter to easily determine how many timer events were triggered
static void Tick2(int* pCount) {
   COL_FUNCTION(Tick2);
   (*pCount)++;
   COL_VAR(*pCount);
}

// CountDown is used to count down on a timer and exit a socket loop afterwards
static void CountDown(int* pCount, SCKloop* pLoop){
   COL_FUNCTION(CountDown);
   (*pCount)--;
   COL_VAR(*pCount);
   if (*pCount == 0){
      COL_TRC("Blast off");
      pLoop->shutdown();
   }
}

static void testTick(){
   COL_FUNCTION(testTick);
   int Count = 5;
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   int TimerId = Loop.addTimer(COLnewClosure0(&CountDown, &Count, &Loop), 250, true);
   Loop.start();
   UNIT_ASSERT_EQUALS(0, Count);   
   Loop.cancelTimer(TimerId);
}

static void cancelTimer(int TimerId, SCKschedule* pTimer){
   COL_FUNCTION(cancelTimer);
   pTimer->cancel(TimerId);
}

// testing whether SCKschedule::fire() handles firing invalid/delted timers correctly
static void testInvalidTimerFire(){
   COL_FUNCTION(testInvalidTimerFire);
   SCKschedule Timer;
   // Set up two timers to cancel each other
   int TimerId1 = Timer.add(COLnewClosure0(&cancelTimer, 2, &Timer), 500);
   int TimerId2 = Timer.add(COLnewClosure0(&cancelTimer, 1, &Timer), 500);
   for (int i = 0; i < 4; i++){
      COL_TRC("Sleep");
      COLsleep(250);
      Timer.fire();
      COL_VAR2(Timer.isSet(TimerId1), Timer.isSet(TimerId2));
   }
   Timer.clear();
   UNIT_ASSERT(true); // If this test doesn't crash then it has passed.
}

// For a repeating timer, when the callback is called it is expected to be re-scheduled in the collection already
static void checkTimerSet(int TimerId, SCKschedule* pTimer){
   COL_FUNCTION(checkTimerSet);
   COL_VAR(pTimer->isSet(TimerId));
   COLASSERT(pTimer->isSet(TimerId));
}

// testing whether SCKschedule can handle check to see if a repeating timer is set properly
static void testIsSet(){
   COL_FUNCTION(testIsSet);
   SCKschedule Timer;
   bool TimerNotSetFlag = false;
   // Set up repeating timer with a callback which checks to see if it is still set 
   int TimerId1 = Timer.add(COLnewClosure0(&checkTimerSet, 1, &Timer), 500, true);
   for (int i = 0; i < 4; i++){
      COL_TRC("Sleep");
      COLsleep(250);
      COL_TRC("Pre-fire");
      COL_VAR(Timer.isSet(TimerId1));
      Timer.fire();
      COL_TRC("Post-fire");
      COL_VAR(Timer.isSet(TimerId1));
   }
   Timer.clear();
   UNIT_ASSERT(true); // If this test doesn't crash then it has passed.
}

static void testSmallerTimeTillNextEvent(){
   COL_FUNCTION(testSmallerTimeTillNextEvent);
   COLtimeStamp Now;
   COLcurrentTimeStamp(&Now);
   int Count1 = 10;
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   int TimerId = Loop.addTimer(COLnewClosure0(&CountDown, &Count1, &Loop), 100, true);
   Loop.start();
   COLtimeStamp FinishTime;
   COLcurrentTimeStamp(&FinishTime);
   int Result = COLtimeStampDiffInMillisecond(FinishTime, Now);
   COL_VAR(Result);
   UNIT_ASSERT(Result >= 1000 && Result <= 10000);
   Loop.cancelTimer(TimerId);
}

static void doNothing(){
   COL_FUNCTION(doNothing);
   COL_TRC("did nothing");
}

static void testTimerIdReuse(){
   COL_FUNCTION(testTimerIdReuse);
   SCKschedule Timer;
   Timer.setTimerId(0); // The add logic increments before registering a timer id - this means the first registered timer have the id 1
   int Timer1 = Timer.add(COLnewClosure0(&doNothing), 500, false);
   COL_VAR(Timer1);
   Timer.setTimerId(Timer1 - 1);  // emulate trying to re-use a timer id
   int Timer2 = Timer.add(COLnewClosure0(&doNothing), 500, false);
   COL_VAR(Timer2);
   UNIT_ASSERT_EQUALS(1, Timer1);
   UNIT_ASSERT_EQUALS(2, Timer2);
   UNIT_ASSERT_EQUALS(2, Timer.registeredTimerCount());
   Timer.clear();
}

static void testTimerIdRollover(){
   COL_FUNCTION(testTimerIdRollover);
   SCKschedule Timer;
   Timer.setTimerId(INT_MAX); // The add logic increments before registering a timer id - this should overflow so the timer gets an id of INT_MIN
   int Timer1 = Timer.add(COLnewClosure0(&doNothing), 500, false);
   Timer.clear();
   UNIT_ASSERT_EQUALS(INT_MIN, Timer1);
}

static void touchBool(bool* pResult){
   COL_FUNCTION(touchBool);
   *pResult = true;
}

static void testTimerLong(){
   COL_FUNCTION(testTimerLong);
   bool Result = false;
   int LongTime = 100000;
   SCKschedule Schedule;
   COLtimeStamp Now; 
   COLcurrentTimeStamp(&Now);
   int TimerId = Schedule.add(COLnewClosure0(&touchBool, &Result), LongTime * 1000, false);
   COL_VAR(Now);
   int TimeIncrement = (LongTime-500) * 1000;
   COL_TRC("Now add " <<  TimeIncrement << " milliseconds");
   Now.addMilliseconds(TimeIncrement);
   COL_VAR(Now);
   COL_VAR(Result);
   UNIT_ASSERT_EQUALS(false, Result);
   SCKscheduleFire(&Schedule, Now);
   COL_VAR(Result);
   UNIT_ASSERT_EQUALS(false, Result);
   TimeIncrement = 600 * 1000;
   COL_TRC("Now add " << TimeIncrement << " milliseconds");
   Now.addMilliseconds(TimeIncrement);
   COL_VAR(Now);
   SCKscheduleFire(&Schedule, Now);
   COL_VAR(Result);
   UNIT_ASSERT_EQUALS(true, Result);
}

void testTimer(UNITapp* pApp){
   pApp->add("timer/tick", &testTick);
   pApp->add("timer/isSet", &testIsSet);
   pApp->add("timer/invalidTimerFire", &testInvalidTimerFire);
   pApp->add("timer/smallerTimeTillNextEvent", &testSmallerTimeTillNextEvent);
   pApp->add("timer/reuse", &testTimerIdReuse);
   pApp->add("timer/rollover", &testTimerIdRollover);
   pApp->add("timer/long", &testTimerLong);
}
