//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLthreadPool
//
// Description: test COLthreadPool
//---------------------------------------------------------------------------
#include "COLexamplePrecomp.h"
#pragma hdrstop

#include <stdio.h>
#include <time.h>

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>
#include <COL/COLthreadPool.h>
#include <COL/COLclosure.h>
#include <COL/COLmutex.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLmutex g_CountMutex;
static int      g_Count = 0;

static void Task()
{
   // this function should take 0.25 seconds to execute.
   COL_FUNCTION(Task);
   COL_TRC("This function should take 0.25 seconds to execute");
   COLthread::sleep(0.125);
   COL_TRC("halfway point");
   COLthread::sleep(0.125);

   COLlocker guard(g_CountMutex);
   ++g_Count;
   COL_VAR(g_Count);
}

static void testTasks()
{
   COL_FUNCTION(testTasks);

   const time_t timeBegin = time(0);
   COL_VAR(timeBegin);

   COL_TRC("creating thread pool");

   // We are using a block here to limit the scope of threadPool.
   // Note that COLthreadPool::shutdown() only initiates the shutdown,
   // But shutdown itself does not block.
   // The thread pool only joins with all its threads upon ~COLthreadPool.

   {
      // 2 threads in pool initially (min 1, max 2)
      COLthreadPool threadPool(1, 2);
   
      COL_TRC("pre-populating thread pool with 15 tasks");
      for (int i = 0; i < 15; i++) {
         threadPool.scheduleTask(COLnewClosure0(Task));
      }
   
      COL_TRC("starting the thread pool");
      threadPool.start();
   
      // this is not strictly necessary, but it gives
      // the pool an opportunity to execute tasks
      // before pool.shutdown(COLthreadPool::shutdown_immediately)
      // is called below
      //COLthread::sleep(0.1);
   
      // Add 3 additional threads to pool
      // for a total of 5 threads in the pool.
      threadPool.addThreads(3);
   
      COL_TRC("populating thread pool with 17 more tasks");
      for (int i = 0; i < 17; i++) {
         threadPool.scheduleTask(COLnewClosure0(Task));
      }
   
      // (15 + 17) tasks each taking 0.25 seconds is 8 seconds,
      // but with tasks spread over 5 threads it should take
      // 1.6 seconds for all to complete.
      // Naturally, this is all subject to machine load and
      // operating system thread scheduling.

      COL_TRC("before thread pool shutdown");
   
      // Note that COLthreadPool::shutdown() only initiates the shutdown.
      // COLthreadPool::shutdown() itself does not block.
      // If shutdown_immediate is in effect it is quite possible
      // that this program will not run any tasks and only
      // take 0.1 seconds to execute.
      //threadPool.shutdown(COLthreadPool::shutdown_immediate);
      threadPool.shutdown(COLthreadPool::shutdown_after_pending_tasks);

      COL_TRC("after thread pool shutdown");

      // block while pool threads are joined in ~COLthreadPool()
   }

   COL_VAR(g_Count);
   TST_ASSERT_EQUALS(32, g_Count);

   const time_t timeEnd = time(0);
   COL_VAR(timeEnd);

   const time_t timeDiff = timeEnd - timeBegin;
   COL_VAR(timeDiff);

   TST_ASSERT(timeDiff >= 1);
   TST_ASSERT(timeDiff <= 4); // should take 2s, but use 4s due to machine load

   COL_TRC("waiting for tasks to complete");
}

TSTtestable* COLthreadPoolTests()
{
   TST_START_COLLECTION("COLthreadPoolTests")
     TST_ADD_TEST_FUNCTION(testTasks);
   TST_END_FIXTURE
}

