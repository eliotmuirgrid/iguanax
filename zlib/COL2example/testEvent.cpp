// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testEvent
//
// Description:
//
// Implementation of tests
//
// Author: Vismay Shah
// Date:   Thursday 18 September 2025 - 5:02PM
// ---------------------------------------------------------------------------
#include "testEvent.h"

#include <UNIT/UNITapp.h>

#include <SCK/SCKloop.h>
#include <memory> // for weak_ptr

#include <COL/COLevent.h>
#include <COL/COLstring.h>
#include <COL/COLthreadPool.h>
#include <COL/COLutils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// Generic utility functions
static void setupSleepTimer(SCKloop* pLoop, int TimeToSleep, COLclosure0* pSleepJob, int* pSleepTimerId){
   COL_FUNCTION(setupSleepTimer);
   *pSleepTimerId = pLoop->addTimer(pSleepJob, TimeToSleep);
   COL_TRC("Set up a sleep timer for " << TimeToSleep << " ms with id " << *pSleepTimerId);
}

static void cancelSleepTimer(SCKloop* pLoop, int TimerId){
   COL_FUNCTION(cancelSleepTimer);
   if(pLoop->isSet(TimerId)){
      COL_TRC("Cancelling sleep timer, id: " << TimerId);
      pLoop->cancelTimer(TimerId);
   }
}

static void shutdownLoop(SCKloop* pLoop){
   COL_TRC("Shutdown event loop");
   pLoop->shutdown();
}

// Old followed by new
struct StateMockupOld{
   COLevent m_Event;
};

struct StateMockupNew{
   std::shared_ptr<COLevent> m_pEvent;
   StateMockupNew(){ m_pEvent = std::make_shared<COLevent>(); }
};

static void sleepDoneOld(COLevent* pEvent){
   COL_FUNCTION(sleepDoneOld);
   COL_TRC("Sleep timer done. Signal worker thread");
   pEvent->signal();
}

static void sleepDoneNew(std::weak_ptr<COLevent> pEvent){
   COL_FUNCTION(sleepDoneNew);
   if (std::shared_ptr<COLevent> pReal = pEvent.lock()) {
      COL_TRC("Sleep timer done. Signal worker thread");
      pReal->signal();
   }
}

static void signalEarlyOld(SCKloop* pLoop, StateMockupOld* pState, int* pTimerId){
   COL_FUNCTION(signalEarlyOld);
   COL_TRC("Cancelling timer " << *pTimerId);
   pLoop->cancelTimer(*pTimerId);
   COL_TRC("Early timer fired, signal worker thread execution");
   pState->m_Event.signal();
   delete pState; // simulate cleanup
}

static void signalEarlyNew(SCKloop* pLoop, StateMockupNew* pState, std::weak_ptr<COLevent> pEvent, int* pTimerId){
   COL_FUNCTION(signalEarlyNew);
   // COL_TRC("set a timer with no delay to call cancel sleep timer");
   COL_TRC("Cancelling timer " << *pTimerId);
   pLoop->cancelTimer(*pTimerId);
   COL_TRC("Early timer fired, signal worker thread execution");
   if (std::shared_ptr<COLevent> pReal = pEvent.lock()) {
      pReal->signal();
   }
   delete pState; // simulate cleanup
}

static void testEventWorkerOld(SCKloop* pLoop, StateMockupOld* pState, COLevent* pEvent, int* pTimerId){
   COL_FUNCTION(testEventWorkerOld);
   COL_TRC("Starting worker thread");
   COLclosure0* pOnSleepDone = COLnewClosure0(&sleepDoneOld, pEvent);
   int TimeToSleep = 500;
   COL_TRC("Scheduling a 500ms sleep");
   pLoop->post(COLnewClosure0(&setupSleepTimer, pLoop, TimeToSleep, pOnSleepDone, pTimerId)); // after 500ms, we wake up
   COL_TRC(*pTimerId);
   // Wait for the event to be signaled from sleep completion
   COL_TRC("Now waiting on lua thread for sleep to be done.");
   pEvent->wait();

   COL_TRC("After wait");
   pLoop->post(COLnewClosure0(&cancelSleepTimer, pLoop, *pTimerId));
   COL_TRC("Worker thead done - resuming (no loop shutdown).");
}

static void testEventWorkerNew(SCKloop* pLoop, StateMockupNew* pState, std::weak_ptr<COLevent> pEvent, int* pTimerId){
   COL_FUNCTION(testEventWorkerNew);
   COL_TRC("Starting worker thread");
   COLclosure0* pOnSleepDone = COLnewClosure0(&sleepDoneNew, pEvent);
   int TimeToSleep = 500;
   COL_TRC("Scheduling a 500ms sleep");
   pLoop->post(COLnewClosure0(&setupSleepTimer, pLoop, TimeToSleep, pOnSleepDone, pTimerId)); // after 500ms, we wake up
   COL_TRC(*pTimerId);
   // Wait for the event to be signaled from sleep completion
   COL_TRC("Now waiting on lua thread for sleep to be done.");
   if (std::shared_ptr<COLevent> pReal = pEvent.lock()) {
      pReal->wait();
   } else {
      COLASSERT("event is gone???");
   }
   COL_TRC("After wait");
   pLoop->post(COLnewClosure0(&cancelSleepTimer, pLoop, *pTimerId));
   COL_TRC("Worker thead done - resuming (no loop shutdown).");
}

static void testEventOld(){
   COL_FUNCTION(testEventOld);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   int TimerId = -1;
   
   StateMockupOld* pState = new StateMockupOld();
   Loop.threadPool()->scheduleTask(COLnewClosure0(&testEventWorkerOld, &Loop, pState, &pState->m_Event, &TimerId));

   // Add a timer to pEvent->signal after 500ms
   COLclosure0* pSignalJob = COLnewClosure0(&signalEarlyOld, &Loop, pState, &TimerId);
   int FastTimer = Loop.addTimer(pSignalJob, 500);

   // Add a timer to shutdown event loop
   COLclosure0* pShutdown = COLnewClosure0(&shutdownLoop, &Loop);
   int ShutdownTimer = Loop.addTimer(pShutdown, 2000);
   COL_TRC("Starting event loop");
   Loop.start();
   
   COL_TRC("Worker exited - sleep for a bit just incase test needs to run a bit longer");
   COLsleep(1000);
   COL_TRC("Test done");
}

static void testEventNew(){
   COL_FUNCTION(testEventNew);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   int TimerId = -1;
   
   StateMockupNew* pState = new StateMockupNew();
   std::weak_ptr<COLevent> wp = pState->m_pEvent;
   std::weak_ptr<COLevent> wp2 = pState->m_pEvent;
   Loop.threadPool()->scheduleTask(COLnewClosure0(&testEventWorkerNew, &Loop, pState, wp, &TimerId));

   // Add a timer to pEvent->signal after 500ms
   COLclosure0* pSignalJob = COLnewClosure0(&signalEarlyNew, &Loop, pState, wp2, &TimerId);
   int FastTimer = Loop.addTimer(pSignalJob, 500);

   // Add a timer to shutdown event loop
   COLclosure0* pShutdown = COLnewClosure0(&shutdownLoop, &Loop);
   int ShutdownTimer = Loop.addTimer(pShutdown, 2000);
   COL_TRC("Starting event loop");
   Loop.start();
   
   COL_TRC("Worker exited - sleep for a bit just incase test needs to run a bit longer");
   COLsleep(1000);
   COL_TRC("Test done");
}

void testEvent(UNITapp* pApp) {
   // pApp->add("event/old", &testEventOld);  // IX-4199 Disabled, this test WILL segfault.
   pApp->add("event/new", &testEventNew);
}
