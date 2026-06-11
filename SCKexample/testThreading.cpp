//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testThreading 
//
// Description:
//
// Integration level test for demonstration of the SCKloop with multi-thread-
// ing. It is highly recommended that you trace the tests in this module using
// the colorized trace .collog file.
//
// Run this with tracing on SCKloop ant testThreading, e.g.
//
// ./SCKexample --test "testThreading/[*]" --t "SCKloop testThreading"
//
// Author: Trevor Hilton
// Date:   17-July-2019 
//---------------------------------------------------------------------------

#include "testThreading.h"

#include <UNIT/UNITapp.h>

#include <SCK/SCKloop.h>

#include <COL/COLlocker.h>
#include <COL/COLmutex.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLstack.h>
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


// ThreadStack gives a means to track state
// between threads. It extends a COLstack
// and uses thread locking in the popAndGetSize()
// method so that while on a thread, only one
// of the executing threads will ever see 
// a size of 0, and can signal a termination,
// if needed.
class ThreadStack : public COLstack<int> {
public:
   ThreadStack() {}
   ~ThreadStack() {}

   COLmutex CriticalSection;

   COLindex popAndGetSize();
};

COLindex ThreadStack::popAndGetSize() {
   COL_METHOD(ThreadStack::popAndGetSize);
   COL_TRC("Entering critical section...");
   COLlocker Lock(CriticalSection);
   COL_TRC("Executing critical section...");
   COLstack::pop();
   COLindex NewSize = COLstack::size();
   COL_TRC("Finished executing critical section, returning.");
   return NewSize;
}

// helper callback to shutdown the SCKloop
static void ShutdownFunction(SCKloop* pLoop) {
   COL_FUNCTION(ShutdownFunction);
   COL_TRC("Back on the SCKloop start thread to shutdown...");
   pLoop->shutdown();
}

// simple function that can be used as a callback for a job
// on the event loop, or on a thread task.
static void JobFunction(SCKloop* pLoop) {
   COL_FUNCTION(JobFunction);
   COL_TRC("Build the callback function to post back to the event loop...");
   COLclosure0* pCallback = COLnewClosure0(&ShutdownFunction, pLoop);
   pLoop->post(pCallback);
}

// function that makes use of the ThreadStack to execute stuff in
// parallel on separate threads. at the end of execution, it checks
// the ThreadStack to see if it should be the one to terminate the
// SCKloop. 
static void ParallelJobFunction(SCKloop* pLoop, ThreadStack* pStack) {
   COL_FUNCTION(ParallelJobFunction);
   COL_TRC("Working on parallel job.");
   // begin parallel code...
   COL_TRC("This is where we execute the code we want in parallel...");
   // end parallel code...
   int JobStackSize = pStack->popAndGetSize();
   COL_TRC("There are " << JobStackSize << " jobs left...");
   // when there are no tasks pending, we can signal the shutdown:
   if ( JobStackSize == 0 ) {
      COLclosure0* pCallback = COLnewClosure0(&ShutdownFunction, pLoop);
      pLoop->post(pCallback);
   }
}

// This test demonstrates how to use a simple timer on the SCKloop
// to execute code on the main thread.
static void testOnSameThread() {
   COL_FUNCTION(testOnSameThread);
   COLthreadPool ThreadPool;
   ThreadPool.start();
   SCKloop Loop(&ThreadPool);

   COL_TRC("Register JobFunction as a callback in a COLclosure with");
   COL_TRC("reference to the SCKloop object...");
   COLclosure0* pCallback = COLnewClosure0(&JobFunction, &Loop);
   
   COL_TRC("Create the timer to execute the callback on the event loop...");
   int TimerId = Loop.addTimer(pCallback, 500, false);
   
   COL_TRC("We're starting the loop...");
   Loop.start();
   
   COL_TRC("...the loop was shutdown and is done.");
}

// This will initiate a job on a separate thread, that eventually comes back
// to the main thread to shutdown the SCKloop. This makes use of the threadPool
// on the SCKloop object to schedule a task that will be run on a separate thread.
static void testOnSeparateThread() {
   COL_FUNCTION(testOnSeparateThread);
   COLthreadPool ThreadPool;
   ThreadPool.start();
   SCKloop Loop(&ThreadPool);
   COL_TRC("Scheduling the Job as a task on the thread pool...");
   Loop.threadPool()->scheduleTask(COLnewClosure0(JobFunction, &Loop));

   COL_TRC("We're starting the loop...");
   Loop.start();
   
   COL_TRC("...the loop was shutdown and is done.");
}

// This will use a loop to schedule multiple tasks simultaneously on separate
// threads.
static void testOnMultipleThreads() {
   COL_FUNCTION(testOnMultipleThreads);
   COLthreadPool ThreadPool;
   ThreadPool.start();
   SCKloop Loop(&ThreadPool);
   ThreadStack Stack;
   for (int i = 0; i < 5; i++) {
      COL_TRC("Scheduling Job " << i << " as a task on the thread pool...");
      Stack.push(i);
      Loop.threadPool()->scheduleTask(COLnewClosure0(ParallelJobFunction, &Loop, &Stack));
   }

   COL_TRC("We're starting the loop...");
   Loop.start();

   COL_TRC("...the loop was shutdown and is done.");

}

void testThreading(UNITapp* pApp) {
   pApp->add("threading/same",&testOnSameThread);
   pApp->add("threading/separate",&testOnSeparateThread);
   pApp->add("threading/multiple",*testOnMultipleThreads);
}
