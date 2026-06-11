#ifndef __COL_THREAD_POOL_H__
#define __COL_THREAD_POOL_H__
//-----------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  COLthreadPool
//
// Author:  Andrew Vajoczki
//
// Description
//
// Dynamic thread pool to schedule tasks on threads.
//-----------------------------------------------------------------------------

#include <COL/COLmutex.h>
#include <COL/COLthread.h>
#include <COL/COLthreadQueue.h>

class COLclosure0;

class COLthreadPool {
public:
   enum ShutdownType { shutdown_after_pending_tasks, shutdown_immediate };

   // Create a thread pool with a default number of threads.
   COLthreadPool(int minThreads = 1, int maxThreads = 100);

   // Wait for all threads to complete their tasks and wait
   // for all threads to be joined.
   // Note: this call must be invoked on the same thread in which 
   // the object was created and started.
   // Also note that this call will block until all threads have joined.
   virtual ~COLthreadPool();

   // Start the thread pool in a background thread.
   // Note: this method may only be invoked on the same thread
   // from which the object was created.
   void start();

   // Schedule a task to be run in an *approximately* FIFO manner;
   // applications must not assume a strict ordering of task execution.
   // True FIFO execution of tasks cannot be guaranteed due to the
   // nature of thread scheduling. Any new task may be run on any
   // available thread in the pool. Tasks must be threadsafe.
   // The COLclosure task input parameter must have been heap
   // allocated and its ownership will be immediately transfered
   // to this instance. The caller must not access the task object
   // in any way after invoking this function, as it may have already
   // been executed and deleted by the time this routine returns.
   // ScheduleTask() may be called from any thread.
   void scheduleTask(COLclosure0* pTask);

   // Signal all worker threads to quit.
   // This function may be called from any thread.
   // Pending tasks in queues may not be executed if ShutdownType is 
   // `shutdown_immediate'.
   // Note: the COLthreadPool may never shutdown if the ShutdownType
   // is `shutdown_after_pending_tasks' and there is a perpetual supply
   // of pending tasks.  Use `shutdown_after_pending_tasks' only for
   // a finite number of tasks.
   // The call to shutdown() itself does not block nor does it
   // wait for the threads in the pool to complete.
   void shutdown(ShutdownType type = shutdown_immediate);

   // Get a snapshot of the total number of threads in the pool.
   int getTotalNumberOfThreads() const;

   // Get a snapshot of the number of tasks still pending execution.
   int getPendingTaskCount() const;

   // Add 'n' additional threads to the pool.
   void addThreads(int n);

 private:
   COLthreadPool(const COLthreadPool&);            // disallow
   COLthreadPool& operator=(const COLthreadPool&); // disallow

   // _addThreads() may only be called within the _controllerThread,
   // namely, by the functions: addThreads() and _controllerThreadRunner()
   void _addThreads(int n);

   // Increase _numThreadsAvailable by n while protected by _mutex
   void increaseAvailableWorkerThreadCount(int n = 1);

   // Decrease _numThreadsAvailable by n while protected by _mutex
   void decreaseAvailableWorkerThreadCount(int n = 1);

   // Return _numThreadsAvailable while protected by _mutex
   int getAvailableWorkerThreadCount() const;

   // Master thread controlling the thread queue responsible for spawning
   // worker threads (_workerThreadRunner) and eventually joining with them.
   static void* _controllerThreadRunner(void* arg);

   // all scheduleTask() COLclosure0 are run within this worker thread function
   static void* _workerThreadRunner(void* arg);

   friend class COLdeferredClosure;

   int                          _minThreads;
   int                          _maxThreads;
   int                          _numThreadsAvailable;
   int                          _immediateShutdownRequested;
   int                          _wasStarted;
   COLthreadID                  _threadWhereCreated;
   COLthread                    _controllerThread;
   COLmutex                     _mutex;
   COLthreadQueue<COLthread*>   _threadJoinQueue;
   COLthreadQueue<COLclosure0*> _controllerThreadQueue;
   COLthreadQueue<COLclosure0*> _taskQueue;
};

#endif // end of defensive include

